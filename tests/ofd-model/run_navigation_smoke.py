#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
# SPDX-License-Identifier: GPL-3.0-or-later
"""Run real-widget checks using an already-built Unix Makefiles reader target.

Only the small smoke entry point is compiled. Reuse the application's objects
and libraries, replacing main, instead of building the monolithic unit tests.
"""

import argparse
import json
import os
from pathlib import Path
import shlex
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("build", type=Path, help="completed reader CMake build directory")
    args = parser.parse_args()
    reader = args.build.resolve() / "reader"
    target = reader / "CMakeFiles/deepin-reader.dir"
    flags = {}
    for line in (target / "flags.make").read_text().splitlines():
        key, separator, value = line.partition(" = ")
        if separator:
            flags[key] = shlex.split(value)
    if "-DOFD_SUPPORT_ENABLED" not in flags["CXX_DEFINES"]:
        parser.error("reader must have OFD support enabled")
    link = shlex.split((target / "link.txt").read_text())
    main_object = "CMakeFiles/deepin-reader.dir/main.cpp.o"
    if link.count(main_object) != 1 or link.count("-o") != 1:
        parser.error("unsupported reader link command; use the Unix Makefiles generator")
    moc = json.loads((reader / "CMakeFiles/deepin-reader_autogen.dir/AutogenInfo.json").read_text())["QT_MOC_EXECUTABLE"]
    source = Path(__file__).resolve().with_name("navigation_smoke.cc")
    with tempfile.TemporaryDirectory(prefix="navigation-smoke-", dir=args.build.resolve()) as directory:
        work = Path(directory)
        obj = work / "navigation_smoke.o"
        binary = work / "navigation-smoke"
        subprocess.run([moc, str(source), "-o", str(work / "navigation_smoke.moc")], check=True)
        subprocess.run([link[0], *flags["CXX_DEFINES"], *flags["CXX_INCLUDES"],
                        *flags["CXX_FLAGS"], "-I", str(work), "-c", str(source), "-o", str(obj)],
                       cwd=reader, check=True)
        link[link.index(main_object)] = str(obj)
        link[link.index("-o") + 1] = str(binary)
        link = [arg for arg in link if not arg.startswith("-Wl,--dependency-file=")]
        subprocess.run(link, cwd=reader, check=True)
        environment = dict(os.environ, QT_QPA_PLATFORM="offscreen", QT_LOGGING_RULES="*.debug=false")
        for name in ("XDG_CONFIG_HOME", "XDG_DATA_HOME", "XDG_CACHE_HOME", "TMPDIR"):
            path = work / name.lower()
            path.mkdir()
            environment[name] = str(path)
        subprocess.run([str(binary)], cwd=reader, env=environment, check=True, timeout=90)


if __name__ == "__main__":
    main()
