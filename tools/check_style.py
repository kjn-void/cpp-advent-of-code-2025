#!/usr/bin/env python3
"""Check first-party C++ formatting and gd-style Systems Hungarian names."""

import argparse
import json
import os
import re
from pathlib import Path
import shutil
import subprocess
import sys

import hungarian_prefixes

repo_root = Path(__file__).resolve().parents[1]
source_roots = ("src", "tests", "benchmarks")


def find_tool(tool_name):
    if tool_path := shutil.which(tool_name):
        return tool_path
    # Homebrew LLVM is keg-only; do not require a global PATH modification.
    if sys.platform == "darwin" and (brew_path := shutil.which("brew")):
        brew_result = subprocess.run(
            [brew_path, "--prefix", "llvm"], text=True, capture_output=True, check=False
        )
        tool_path = Path(brew_result.stdout.strip()) / "bin" / tool_name
        if brew_result.returncode == 0 and tool_path.is_file():
            return str(tool_path)
    raise RuntimeError(f"Missing {tool_name}; see README.md development tools")


def platform_arguments():
    """Homebrew clang-tidy needs the SDK selected by Apple's active toolchain."""
    if sys.platform != "darwin":
        return []
    sdk_result = subprocess.run(
        ["xcrun", "--show-sdk-path"], check=True, text=True, capture_output=True
    )
    sdk_path = Path(sdk_result.stdout.strip())
    headers_path = sdk_path / "usr/include/c++/v1"
    if not headers_path.is_dir():
        raise RuntimeError(f"Missing C++ headers in selected SDK: {headers_path}")
    return ["-isysroot", str(sdk_path), "-isystem", str(headers_path)]


def source_files():
    return sorted(
        source_path
        for source_directory in source_roots
        for source_path in (repo_root / source_directory).rglob("*")
        if source_path.suffix in {".cpp", ".h"}
    )


def translation_units(build_directory, source_paths):
    database_path = build_directory / "compile_commands.json"
    if not database_path.is_file():
        raise RuntimeError(f"Missing {database_path}; configure CMake with tests and benchmarks enabled")
    compile_commands = json.loads(database_path.read_text())
    compiled_sources = {
        (Path(command["directory"]) / command["file"]).resolve()
        for command in compile_commands
    }
    translation_unit_paths = [source_path for source_path in source_paths if source_path.suffix == ".cpp"]
    missing_sources = [str(source_path.relative_to(repo_root)) for source_path in translation_unit_paths if source_path not in compiled_sources]
    if missing_sources:
        raise RuntimeError("Compilation database omits first-party sources: " + ", ".join(missing_sources))
    return translation_unit_paths


def check_main_names(source_text):
    # clang-tidy always exempts the real main function, even with
    # IgnoreMainLikeFunctions=false. Check this project's entry point explicitly.
    signature_pattern = (
        r"\bint\s+main\s*\(\s*int\s+iArgumentCount\s*,\s*char\s*\*\s*\*\s*ppbszArgument\s*\)"
    )
    # Comments or literals containing the preferred signature must not hide a
    # differently named real entry point.
    code = re.sub(
        r'R"(?P<delim>[^ ()\\\t\n]{0,16})\(.*?\)(?P=delim)"'
        r'|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*.*?\*/',
        " ", source_text, flags=re.S,
    )
    if not re.search(signature_pattern, code):
        raise RuntimeError("Use int main(int iArgumentCount, char** ppbszArgument) for the entry point")


def check_prefixes(query_path, translation_unit_paths, build_directory, extra_arguments):
    """Fail when a declaration's Hungarian prefix does not match its C++ type."""
    print("Prefixes: " + ", ".join(str(path.relative_to(repo_root)) for path in translation_unit_paths), flush=True)
    violations = hungarian_prefixes.check_prefixes(
        query_path, translation_unit_paths, ["-p", str(build_directory), *extra_arguments], repo_root
    )
    if violations:
        diagnostic = "\n".join(
            f"{declaration.path.relative_to(repo_root)}:{declaration.line}:{declaration.column}: "
            f"'{declaration.name}': {message}"
            for declaration, message in violations
        )
        if os.environ.get("GITHUB_ACTIONS") == "true":
            encoded = diagnostic[-8000:].replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A")
            print(f"::error title=Hungarian prefixes::{encoded}", flush=True)
        raise RuntimeError("Hungarian prefixes do not match declared types:\n" + diagnostic)


def run_checked(command_arguments):
    process = subprocess.run(
        command_arguments, text=True, capture_output=True, check=False, cwd=repo_root
    )
    print(process.stdout, end="", flush=True)
    print(process.stderr, end="", file=sys.stderr, flush=True)
    if process.returncode:
        if os.environ.get("GITHUB_ACTIONS") == "true":
            # Expose the actual diagnostic beside the failed check, rather than
            # leaving only GitHub's generic exit-code annotation.
            diagnostic = (process.stdout + process.stderr)[-8000:]
            diagnostic = diagnostic.replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A")
            print(f"::error title=Style validation::{diagnostic}", flush=True)
        raise subprocess.CalledProcessError(process.returncode, command_arguments)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=repo_root / "build")
    parser.add_argument("--clang-tidy")
    parser.add_argument("--clang-format")
    parser.add_argument("--clang-query")
    options = parser.parse_args()
    try:
        tidy_path = options.clang_tidy or find_tool("clang-tidy")
        format_path = options.clang_format or find_tool("clang-format")
        query_path = options.clang_query or find_tool("clang-query")
        check_main_names((repo_root / "src/main.cpp").read_text())
        source_paths = source_files()
        translation_unit_paths = translation_units(options.build_dir.resolve(), source_paths)
        run_checked([format_path, "--dry-run", "--Werror", *map(str, source_paths)])
        extra_arguments = [f"--extra-arg={compiler_argument}" for compiler_argument in platform_arguments()]
        for source_path in translation_unit_paths:
            print(f"Naming: {source_path.relative_to(repo_root)}", flush=True)
            run_checked(
                [tidy_path, str(source_path), "-p", str(options.build_dir.resolve()),
                 f"--config-file={repo_root / '.clang-tidy'}", "--quiet", *extra_arguments]
            )
        check_prefixes(query_path, translation_unit_paths, options.build_dir.resolve(), extra_arguments)
        print(f"Style passed: {len(source_paths)} files, {len(translation_unit_paths)} translation units.")
        return 0
    except (OSError, RuntimeError, ValueError, KeyError, subprocess.CalledProcessError) as error:
        print(f"Style check failed: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
