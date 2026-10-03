# Project conventions

Use idiomatic C++20 and Apps Hungarian throughout first-party code. The reference
is [Charles Simonyi's Hungarian Notation](https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa260976%28v%3Dvs.60%29).
Use the semantic domain and meaningful operations to choose tags. Do not invent a
storage-type dialect such as `iCount`, `strName`, or `vecButtons`.

## Names and constructions

A variable name is a lowercase semantic tag followed, when needed, by a Capitalized
qualifier. Use a bare tag when it is unambiguous: `rw`, `col`, `mch`. The complete
constructed tag stays lowercase: `mpcolrwPivot`, not `mpColRwPivot`.

| Construction | Meaning | Project example |
| --- | --- | --- |
| `pX` | Explicit pointer to X | `pslvDay`, `pchLim` |
| `dX` | Difference in X's coordinate/domain | `drwNeighbor`, `dposRotation` |
| `cX` | Count of X | `crw`, `cbtn`, `cusArgs` |
| `rgX` / `iX` | Indexed sequence of X / its index | `rgbtn` / `ibtn`, `rgpt` / `iptFirst` |
| `mpXY` | Mapping from X to Y | `mpcolrwPivot`, `mpdevrgdevOutputs_` |
| `setX` | Membership set (project extension) | `setdevActive` |
| `qX` | Pending work queue (project extension) | `qcelRemovals` |
| `optX` | Optional result (C++ extension) | `optcostBest`, `mpjvoptcostMemo` |

The original `f` flag tag is valid: `fHorizontal`, `fRepeated`. Its qualifier must
describe the condition when true. Likewise, `p` is a valid construction, not a
forbidden storage prefix. Other original constructions (`bX`, `cbX`, `cwX`, `grpX`,
`dnX`, `eX`, `hX`) and string contracts (`sz`, `st`) can be registered if needed;
do not apply byte/word counts or C-string contracts to unrelated modern objects.

Use these standard qualifiers with their precise meanings:

- `First`: first member of an interval; `Last`: inclusive upper endpoint.
- `Lim`: exclusive upper endpoint; `Max`: absolute/allocated exclusive bound;
  `Mac`: current exclusive bound/count. Do not name an inclusive maximum `Max`.
- `Nil`: distinguished absence; `T`: temporary. Prefer a useful domain qualifier
  when more than one value of the same tag is in scope.

Name procedures with an initial capital, a return tag for value-producing
operations, and a short action: `RgusReadInput`, `CostSolveJoltage`, `FRegionFits`,
`SetInput`. Use the capitalized domain tag for structures (`Mch`, `Shp`, `Reg`).
Callable variables remain variables (`fnSearch`, `fnRecurSearch`). Apply the same
rules to constants, members, parameters, lambdas, structured bindings, and tests.

## Domain vocabulary

Tags describe logical values, not their `int`, `string`, vector, or span storage.
Prefer an existing precise tag or a composition to adding another tag.

| Tags | Domain |
| --- | --- |
| `rw`, `col` | Grid or matrix row and column indices; keep distinct from spatial coordinates |
| `x`, `y`, `z`, `xy` | Original spatial coordinates; `xy` is an axis-agnostic helper coordinate |
| `pt`, `cel`, `delta`, `seg`, `edg`, `link` | Spatial point, indexed grid cell, displacement pair, polygon segment, weighted graph edge, connected endpoint pair |
| `pos`, `mov` | Dial position and dial rotation instruction |
| `id`, `dev` | Numeric puzzle identity and device name; neither is a positional index |
| `dig`, `bnk`, `jol` | Battery digit, bank of digits, and joltage |
| `blk`, `rng` | Worksheet column block and interval/subrange |
| `mch`, `btn`, `ictr`, `jv`, `par`, `chc` | Machine, button's affected counters, counter index, full joltage vector, parity vector, and parity-choice record |
| `shp`, `var`, `reg`, `plc` | Gift shape, oriented variant, packing region, and placement |
| `vst`, `dsu` | Device visit state and disjoint-set circuit state |
| `area`, `len`, `dist`, `coef`, `cost` | Area, length, distance, algebraic coefficient, optimization cost |
| `cnt`, `iter`, `off` | Event count, iteration ordinal, text/bit offset; use `cX`/`iX` when X exists |
| `bit`, `mask`, `f`, `ch` | Single bit, domain bitset, proposition, character |
| `us`, `txt` | Unvalidated/input text and constructed/validated output text; trimming alone does not validate input |
| `grid`, `mat` | Spatial grid and algebraic coefficient matrix; qualifiers identify the content |
| `slv`, `regy`, `fac`, `drg` | Solver, solver registry, solver factory, day registration |
| `wkr`, `item`, `fn`, `gen`, `hash` | Worker, generic indexed work item, callable, sample generator, hash result |
| `path`, `in`, `out`, `rc`, `err`, `it`, `bms` | Path, input/output stream, exit status, error, lookup iterator, benchmark iteration state |
| `val` | Arithmetic operand or generic utility value; use a domain-specific tag whenever one fits |
| `arg`, `opt`, `prs`, `rec` | Tooling argument, parsed options, argument parser, process/database record |

For example, `mpishprgplc` maps a shape index to its possible placements;
`mpiptiptParent` maps a point index to its parent point index. A C++ container
change does not change either relationship. `jv` and `btn` are deliberately short
logical tags for collections with domain operations of their own.

## C++20 and tooling adaptations

- References are named as the referenced domain, not as explicit pointers. Smart
  pointers use `pX` when accessed as pointers. A borrowed sequence such as argv or
  a span uses `rgX`; its element type, not its ownership, determines X.
- Keep a trailing `_` on encapsulated state where the class uses it. Keep `DayNN`
  names for numbered solver subclasses; their common interface has tag `slv`.
  Templates use capitalized role names such as `Val`, `Fn`, and `SlvDay`.
- Preserve language/library contracts: `main`, operators, external API identifiers,
  GoogleTest/Benchmark macros and generated names, standard CMake names/options,
  Python `self`, `cls`, dunder names and unittest hooks. File and target names remain
  stable. Python helpers and project-owned CMake variables follow semantic naming
  by review; clang-tidy covers C++ declarations.
- Do not force 1990s string representations, raw ownership, or word-sized storage
  into this C++20 project to accommodate a name.

## Guardrails and verification

`.clang-tidy` lists the registered tags and compositions. All four declaration
regexes must agree. Extend them only for a documented domain/construction, with
positive and negative checker coverage. Do not weaken the checker or add broad
exclusions or `NOLINT` to hide violations. A checker validates spelling, not meaning:
review that `rw` really is a row, a `pX` is a pointer, and a `Lim` is exclusive.

- Configure with tests and benchmarks enabled so the compilation database covers
  every first-party translation unit. Then run
  `python3 tools/check_style.py --build-dir build` and
  `python3 tools/test_style_guardrails.py`. Missing tools, omitted sources, naming
  violations, formatting errors, and compilation errors must fail validation.
- Run `cmake --build build --parallel` and
  `ctest --test-dir build --output-on-failure`. Preserve every puzzle answer. Use
  the README sanitizer build when changing algorithms or memory handling.
- Keep this file, `.clang-tidy`, `.clang-format`, README, and CI consistent. Document
  Homebrew development tools. Preserve the dependency-free CLI build.
- CI runs tests, sanitizer tests, and naming/format checks on pushes and pull
  requests. **Do not run benchmarks in CI.** Benchmark source is linted only.
  Performance measurements require the proper private inputs and reference M4;
  do not invent measurements for naming-only changes.
- Preserve exact integer algorithms, input validation, and exception propagation.
  A naming migration must not change algorithms or command-line behavior.
