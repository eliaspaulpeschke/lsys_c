from typing import Iterable
import os
import pathlib
from pathlib import Path
from argparse import ArgumentParser

PROJECT_DIR = Path(os.getcwd())

NINJA_FILE = PROJECT_DIR / "build.ninja"

SOURCE_DIR = PROJECT_DIR / "src"
BUILD_DIR = PROJECT_DIR  / "build"

SOURCES = [ x for x in SOURCE_DIR.glob("**/*.c") ]

NINJA_PRELUDE = f"""
cflags = -O0 -ggdb -pg
ldflags = -lraylib -lm -pg
bdir = {BUILD_DIR}

rule ln
    command = gcc $ldflags $in -o $out

rule cc
    depfile = $out.d
    command = gcc -MD -MF $out.d $cflags -c $in -o $out

rule peg
    command = packcc -o {SOURCE_DIR}/lsystem/parser/lsystem_parser $in

"""

def handle_args():
    p = ArgumentParser()
    p.add_argument("command", nargs="?", default="build")
    return p.parse_args()

def format_dependency_build(path: Path, rulename: str = "cc", extra_deps: list[str] = []) -> str:
    fname = path.stem
    srcpath = path.parent
    dep = f"""build $bdir/{fname}.o: {rulename} {path}""" \
        + (("| " + " ".join(extra_deps)) if len(extra_deps) > 0 else "")
    return dep

def format_main_build(sources: Iterable[Path], rulename: str = "ln") -> str:
    source_objs = ( f"$bdir/{p.stem}.o" for p in sources )
    return f"""build $bdir/aristid: {rulename} {" ".join(source_objs)}"""

def extra_deps(source: Path) -> list[str]:
    match (str(source.name)):
        case "lsystem_parser.c":
            return [str(source.parent / "lsystem_parser.peg")]
        case _:
            return []

def format_ninja_string(sources: Iterable[Path]) -> str:
    br = "\n\n\n"
    return NINJA_PRELUDE + br + format_main_build(SOURCES) \
         + br + br.join(format_dependency_build(s) for s in SOURCES) \
         + br + \
         f"""
build {SOURCE_DIR}/lsystem/parser/lsystem_parser.c: peg {SOURCE_DIR}/lsystem/parser/lsystem_parser.peg
"""      + br

def build():
    with NINJA_FILE.open("w") as njf:
        njf.write(format_ninja_string(SOURCES))
    os.system("ninja")


def main():
    args = handle_args()
    print(args.command)
    match str(args.command).strip():
        case "build":
            build()
        case "clean":
            os.system("rm -rf ./build")
        case "run":
            build()
            os.system("./build/aristid")

if __name__ == "__main__":
    main()

