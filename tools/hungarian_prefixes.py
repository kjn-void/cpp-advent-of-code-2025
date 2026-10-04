"""Verify gd-style Systems Hungarian prefixes against the declared C++ types.

clang-tidy checks capitalization and the `m_` member prefix. This module checks
what a spelling rule cannot: that each variable, parameter, member, and
structured binding starts with the prefix its actual type requires. Types come
from clang-query, so `auto` declarations are checked against their deduced type.
"""

from dataclasses import dataclass
from pathlib import Path
import re
import subprocess

declaration_kinds = ("ParmVarDecl", "VarDecl", "FieldDecl", "BindingDecl")
declaration_flags = {
    "used", "referenced", "invalid", "constexpr", "static", "inline", "nrvo", "cinit",
    "callinit", "listinit", "parenlistinit", "implicit", "extern", "tls", "destroyed",
}

signed_integer_types = {
    "signed char", "short", "int", "long", "long long", "__int128",
}
unsigned_integer_types = {
    "unsigned char", "unsigned short", "unsigned int", "unsigned long", "unsigned long long",
    "unsigned __int128", "char8_t", "char16_t", "char32_t",
}
floating_point_types = {"float", "double", "long double"}

# Standard class templates use the prefix gd uses for them; other classes use
# their lowercase name without namespace, template arguments, or underscores.
standard_class_prefixes = {
    "basic_string": "string", "basic_string_view": "string", "string": "string",
    "string_view": "string", "vector": "vector",
    "array": "array", "pair": "pair", "map": "map", "unordered_map": "map",
    "multimap": "map", "set": "set", "unordered_set": "set", "queue": "queue",
    "deque": "deque", "optional": "optional", "span": "span", "function": "function",
    "tuple": "tuple", "atomic": "atomic",
}
pointer_classes = {"unique_ptr", "shared_ptr", "weak_ptr"}
# Library member aliases say nothing about the value; use the type they name.
member_alias_names = {
    "type", "value_type", "reference", "const_reference", "element_type", "mapped_type",
    "key_type", "size_type", "difference_type", "pointer", "const_pointer",
}
expression_types_pattern = re.compile(r"'([^']*)':'([^']*)'")

escape_pattern = re.compile(r"[a-z][a-z0-9]*(?:_[a-z0-9]+)*_")
camel_suffix_pattern = re.compile(r"(?:[A-Z0-9][A-Za-z0-9]*)?")
pointer_pattern = re.compile(r"p[a-z]*(?:[A-Z0-9][A-Za-z0-9]*)?")
match_line_pattern = re.compile(
    r"^(?P<kind>" + "|".join(declaration_kinds) + r") 0x[0-9a-f]+ <(?P<range>[^>]*)> "
    r"(?P<location>\S+)(?P<rest>.*)$"
)
diagnostic_pattern = re.compile(r"^(?P<file>/[^:]+):(?P<line>\d+):(?P<column>\d+): note: \"root\" binds here")


@dataclass(frozen=True)
class Declaration:
    kind: str
    path: Path
    line: int
    column: int
    name: str


@dataclass
class DeclarationTypes:
    types: list
    is_enumeration: bool = False


def declaration_matcher(file_pattern):
    return (
        "match namedDecl(anyOf(varDecl(), fieldDecl(), bindingDecl()), "
        f'isExpansionInFileMatching("{file_pattern}"), unless(isImplicit()))'
    )


def enumeration_matcher(file_pattern):
    enum_type = "hasType(hasUnqualifiedDesugaredType(enumType()))"
    return (
        f"match namedDecl(anyOf(varDecl({enum_type}), fieldDecl({enum_type}), bindingDecl({enum_type})), "
        f'isExpansionInFileMatching("{file_pattern}"))'
    )


def template_parameter_matcher(file_pattern):
    return f'match templateTypeParmDecl(isExpansionInFileMatching("{file_pattern}"))'


def run_query(query_path, source_paths, query_arguments, file_pattern, working_directory, compiler_arguments=None):
    command = [
        str(query_path), *query_arguments, *map(str, source_paths),
        "-c", "set output dump", "-c", declaration_matcher(file_pattern),
        "-c", "set output diag", "-c", enumeration_matcher(file_pattern),
        "-c", "set output dump", "-c", template_parameter_matcher(file_pattern),
    ]
    if compiler_arguments is not None:
        command += ["--", *compiler_arguments]  # Without a compilation database.
    process = subprocess.run(command, text=True, capture_output=True, check=False, cwd=working_directory)
    if process.returncode:
        raise RuntimeError("clang-query failed:\n" + (process.stdout + process.stderr)[-8000:])
    if re.search(r"^\S+: error: ", process.stdout + process.stderr, re.M):
        raise RuntimeError("clang-query reported compiler errors:\n" + (process.stdout + process.stderr)[-8000:])
    return process.stdout


def split_location(location_text, current_path, current_line):
    """Resolve clang's absolute, line-relative, or column-only source locations."""
    if location_match := re.fullmatch(r"col:(\d+)", location_text):
        return current_path, current_line, int(location_match[1])
    if location_match := re.fullmatch(r"line:(\d+):(\d+)", location_text):
        return current_path, int(location_match[1]), int(location_match[2])
    if location_match := re.fullmatch(r"(/.+):(\d+):(\d+)", location_text):
        return Path(location_match[1]), int(location_match[2]), int(location_match[3])
    return None, current_line, 0  # Macro scratch space or an invalid location.


def parse_name_and_types(rest_text):
    tokens = rest_text.strip()
    quote_index = tokens.find("'")
    if quote_index < 0:
        return None, None
    words = [word for word in tokens[:quote_index].split() if word not in declaration_flags]
    name = words[-1] if words else None
    type_match = re.match(r"'([^']*)'(?::'([^']*)')?", tokens[quote_index:])
    return name, (type_match.group(1), type_match.group(2) or type_match.group(1))


def collect_declarations(query_output, repo_root):
    """Map each first-party declaration to every type clang reported for it."""
    declarations = {}
    enumeration_locations = set()
    template_parameter_names = set()
    current_path, current_line = None, 0
    alias_declaration = None
    for output_line in query_output.splitlines():
        if alias_declaration and output_line[:1] in (" ", "|", "`"):
            # A range-for variable prints only `value_type &`; its initializer
            # expression prints the alias together with the type it names.
            alias_types = declarations[alias_declaration].types
            for sugared_type, desugared_type in expression_types_pattern.findall(output_line):
                if normalized_type(sugared_type) == normalized_type(alias_types[-1][0]):
                    alias_types[-1] = (alias_types[-1][0], desugared_type)
                    alias_declaration = None
                    break
            continue
        alias_declaration = None
        if diagnostic_match := diagnostic_pattern.match(output_line):
            enumeration_locations.add(
                (Path(diagnostic_match["file"]), int(diagnostic_match["line"]), int(diagnostic_match["column"]))
            )
            continue
        if output_line.startswith("TemplateTypeParmDecl "):
            if parameter_match := re.search(r"depth \d+ index \d+ (\w+)\s*$", output_line):
                template_parameter_names.add(parameter_match.group(1))
            continue
        line_match = match_line_pattern.match(output_line)
        if not line_match:
            continue
        range_texts = [text.strip() for text in line_match["range"].split(",")]
        if range_texts[0].startswith(("<", "scratch", "invalid")):
            continue
        # Relative locations refer to the last location clang printed: the range end.
        for range_text in range_texts:
            current_path, current_line, _ = split_location(range_text, current_path, current_line)
        if current_path is None:
            continue
        path, line, column = split_location(line_match["location"], current_path, current_line)
        name, types = parse_name_and_types(line_match["rest"])
        if not name or path is None or not path.is_relative_to(repo_root):
            continue
        declaration = Declaration(line_match["kind"], path, line, column, name)
        declarations.setdefault(declaration, DeclarationTypes([])).types.append(types)
        if types[0] == types[1] and outer_class_name(normalized_type(types[0])) in member_alias_names:
            alias_declaration = declaration
    for declaration, declaration_types in declarations.items():
        declaration_types.is_enumeration = (declaration.path, declaration.line, declaration.column) in enumeration_locations
    return declarations, template_parameter_names


def normalized_type(type_text):
    type_text = re.sub(r"\b(?:const|volatile|struct|class|enum|typename|union)\b", " ", type_text)
    type_text = re.sub(r"\s*&+\s*$", "", type_text.strip())
    return re.sub(r"\s+", " ", type_text).strip()


def outer_class_name(type_text):
    """Return the unqualified name of the outermost class, e.g. vector for std::vector<int>."""
    depth, outer_text = 0, ""
    for character in type_text:
        if character == "<":
            depth += 1
        elif character == ">":
            depth -= 1
        elif depth == 0:
            outer_text += character
    return outer_text.strip().split("::")[-1].strip()


def is_dependent(type_text, template_parameter_names):
    if "<dependent type>" in type_text or "type-parameter-" in type_text:
        return True
    if re.search(r"\bauto\b|\bdecltype\(auto\)", type_text):
        return True
    identifiers = set(re.findall(r"[A-Za-z_]\w*", type_text))
    return bool(identifiers & template_parameter_names)


def prefixes_for_type(sugared_type, desugared_type, is_enumeration):
    """Return the accepted prefixes, or "pointer"/"lambda" for those categories."""
    sugared = normalized_type(sugared_type)
    desugared = normalized_type(desugared_type)
    if "(lambda at " in desugared:
        return "lambda"
    if is_enumeration:
        return {"e"}
    if desugared.endswith("*") or "(*)" in desugared or outer_class_name(desugared) in pointer_classes:
        return "pointer"
    if desugared == "bool":
        return {"b"}
    if desugared == "char":
        return {"i", "ch"}
    if desugared in signed_integer_types:
        return {"i"}
    if desugared in unsigned_integer_types:
        return {"u"}
    if desugared in floating_point_types:
        return {"d"}
    if re.search(r"\[\d*\]$", desugared):
        return {"array"}
    prefixes = set()
    for type_text in (sugared, desugared):
        class_name = outer_class_name(type_text)
        if class_name in member_alias_names:
            continue
        if "iter" in class_name.lower():
            prefixes.add("it")
        elif class_name in standard_class_prefixes:
            prefixes.add(standard_class_prefixes[class_name])
        elif class_name:
            prefixes.add(class_name.replace("_", "").lower())
    return prefixes


def has_prefix(name_body, prefixes):
    if prefixes == "pointer":
        return bool(pointer_pattern.fullmatch(name_body))
    if prefixes == "lambda":
        return False
    return any(
        name_body.startswith(prefix) and camel_suffix_pattern.fullmatch(name_body[len(prefix):])
        for prefix in prefixes
    )


def describe(prefixes):
    if prefixes == "pointer":
        return "p..."
    if prefixes == "lambda":
        return "a lowercase name ending in _"
    return " or ".join(sorted(prefixes))


def prefix_violations(declarations, template_parameter_names):
    """Return (declaration, message) pairs for names that do not fit their type."""
    violations = []
    for declaration, declaration_types in sorted(
        declarations.items(), key=lambda item: (str(item[0].path), item[0].line, item[0].column)
    ):
        concrete_types = [
            types for types in declaration_types.types
            if not is_dependent(types[0], template_parameter_names) or not is_dependent(types[1], template_parameter_names)
        ]
        if any(is_dependent(types[0], template_parameter_names) and
               set(re.findall(r"[A-Za-z_]\w*", types[0])) & template_parameter_names
               for types in declaration_types.types):
            continue  # A template parameter's prefix depends on the instantiation.
        concrete_types = [
            types for types in concrete_types
            if types[0] != types[1] or outer_class_name(normalized_type(types[0])) not in member_alias_names
        ]
        if not concrete_types:
            continue  # Only an unresolved library alias is known; nothing to verify.
        name = declaration.name
        if declaration.kind == "FieldDecl":
            if not name.startswith("m_"):
                violations.append((declaration, "members start with m_"))
                continue
            name_body = name[2:]
        else:
            if escape_pattern.fullmatch(name):
                continue
            name_body = name
        for sugared_type, desugared_type in concrete_types:
            prefixes = prefixes_for_type(sugared_type, desugared_type, declaration_types.is_enumeration)
            if not has_prefix(name_body, prefixes):
                violations.append(
                    (declaration, f"type '{sugared_type}' requires prefix {describe(prefixes)}")
                )
                break
    return violations


def check_prefixes(query_path, source_paths, query_arguments, repo_root, compiler_arguments=None):
    # clang-query uses POSIX regular expressions; the repo root is filtered in Python.
    file_pattern = "/(src|tests|benchmarks)/"
    query_output = run_query(query_path, source_paths, query_arguments, file_pattern, repo_root, compiler_arguments)
    declarations, template_parameter_names = collect_declarations(query_output, repo_root)
    if not declarations:
        raise RuntimeError("clang-query found no first-party declarations")
    return prefix_violations(declarations, template_parameter_names)
