#!/usr/bin/env python3
"""Check first-party C++ formatting and Apps Hungarian declaration names."""

import argparse
import json
import os
import re
from pathlib import Path
import shutil
import subprocess
import sys

pathRoot = Path(__file__).resolve().parents[1]
rgpathSourceRoots = ("src", "tests", "benchmarks")


def PathFindTool(usName):
    if pathTool := shutil.which(usName):
        return pathTool
    # Homebrew LLVM is keg-only; do not require a global PATH modification.
    if sys.platform == "darwin" and (pathBrew := shutil.which("brew")):
        recPrefix = subprocess.run(
            [pathBrew, "--prefix", "llvm"], text=True, capture_output=True, check=False
        )
        pathTool = Path(recPrefix.stdout.strip()) / "bin" / usName
        if recPrefix.returncode == 0 and pathTool.is_file():
            return str(pathTool)
    raise RuntimeError(f"Missing {usName}; see README.md development tools")


def RgargPlatform():
    """Homebrew clang-tidy needs the SDK selected by Apple's active toolchain."""
    if sys.platform != "darwin":
        return []
    recSdk = subprocess.run(
        ["xcrun", "--show-sdk-path"], check=True, text=True, capture_output=True
    )
    pathSdk = Path(recSdk.stdout.strip())
    pathHeaders = pathSdk / "usr/include/c++/v1"
    if not pathHeaders.is_dir():
        raise RuntimeError(f"Missing C++ headers in selected SDK: {pathHeaders}")
    return ["-isysroot", str(pathSdk), "-isystem", str(pathHeaders)]


def RgpathSource():
    return sorted(
        pathSource
        for pathDirectory in rgpathSourceRoots
        for pathSource in (pathRoot / pathDirectory).rglob("*")
        if pathSource.suffix in {".cpp", ".h"}
    )


def RgpathTranslationUnits(pathBuild, rgpathSources):
    pathDatabase = pathBuild / "compile_commands.json"
    if not pathDatabase.is_file():
        raise RuntimeError(f"Missing {pathDatabase}; configure CMake with tests and benchmarks enabled")
    rgrecCommands = json.loads(pathDatabase.read_text())
    setpathCompiled = {
        (Path(recCommand["directory"]) / recCommand["file"]).resolve()
        for recCommand in rgrecCommands
    }
    rgpathUnits = [pathSource for pathSource in rgpathSources if pathSource.suffix == ".cpp"]
    rgtxtMissing = [str(pathSource.relative_to(pathRoot)) for pathSource in rgpathUnits if pathSource not in setpathCompiled]
    if rgtxtMissing:
        raise RuntimeError("Compilation database omits first-party sources: " + ", ".join(rgtxtMissing))
    return rgpathUnits


def CheckMainNames(usSource):
    # clang-tidy always exempts the real main function, even with
    # IgnoreMainLikeFunctions=false. Check this project's entry point explicitly.
    txtSignature = r"\bint\s+main\s*\(\s*int\s+cusArgs\s*,\s*char\s*\*\s*\*\s*rgusArgs\s*\)"
    # Comments or literals containing the preferred signature must not hide a
    # differently named real entry point.
    txtCode = re.sub(
        r'R"(?P<delim>[^ ()\\\t\n]{0,16})\(.*?\)(?P=delim)"'
        r'|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*.*?\*/',
        " ", usSource, flags=re.S,
    )
    if not re.search(txtSignature, txtCode):
        raise RuntimeError("Use int main(int cusArgs, char** rgusArgs) for the entry point")


def RunChecked(rgargCommand):
    recProcess = subprocess.run(
        rgargCommand, text=True, capture_output=True, check=False, cwd=pathRoot
    )
    print(recProcess.stdout, end="", flush=True)
    print(recProcess.stderr, end="", file=sys.stderr, flush=True)
    if recProcess.returncode:
        if os.environ.get("GITHUB_ACTIONS") == "true":
            # Expose the actual diagnostic beside the failed check, rather than
            # leaving only GitHub's generic exit-code annotation.
            txtDiagnostic = (recProcess.stdout + recProcess.stderr)[-8000:]
            txtDiagnostic = txtDiagnostic.replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A")
            print(f"::error title=Style validation::{txtDiagnostic}", flush=True)
        raise subprocess.CalledProcessError(recProcess.returncode, rgargCommand)


def RcMain():
    prsOptions = argparse.ArgumentParser(description=__doc__)
    prsOptions.add_argument("--build-dir", type=Path, default=pathRoot / "build")
    prsOptions.add_argument("--clang-tidy")
    prsOptions.add_argument("--clang-format")
    optOptions = prsOptions.parse_args()
    try:
        pathTidy = optOptions.clang_tidy or PathFindTool("clang-tidy")
        pathFormat = optOptions.clang_format or PathFindTool("clang-format")
        CheckMainNames((pathRoot / "src/main.cpp").read_text())
        rgpathSources = RgpathSource()
        rgpathUnits = RgpathTranslationUnits(optOptions.build_dir.resolve(), rgpathSources)
        RunChecked([pathFormat, "--dry-run", "--Werror", *map(str, rgpathSources)])
        rgargExtra = [f"--extra-arg={argCompiler}" for argCompiler in RgargPlatform()]
        for pathSource in rgpathUnits:
            print(f"Naming: {pathSource.relative_to(pathRoot)}", flush=True)
            RunChecked(
                [pathTidy, str(pathSource), "-p", str(optOptions.build_dir.resolve()),
                 f"--config-file={pathRoot / '.clang-tidy'}", "--quiet", *rgargExtra]
            )
        print(f"Style passed: {len(rgpathSources)} files, {len(rgpathUnits)} translation units.")
        return 0
    except (OSError, RuntimeError, ValueError, KeyError, subprocess.CalledProcessError) as errCheck:
        print(f"Style check failed: {errCheck}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(RcMain())
