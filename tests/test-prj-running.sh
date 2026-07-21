#!/bin/bash

# SPDX-FileCopyrightText: 2023-2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

set -u

builddir=build
reportdir=build-ut

# Resolve project root (parent of the tests/ directory that holds this script)
script_dir="$(cd "$(dirname "$0")" && pwd)"
project_root="$(cd "${script_dir}/.." && pwd)"

build_path="${project_root}/${builddir}"
report_path="${project_root}/${reportdir}"

# Fresh build directory to ensure a clean coverage run
rm -rf "${build_path}"
mkdir -p "${build_path}"

# Fresh report directory
rm -rf "${report_path}"
mkdir -p "${report_path}"

cd "${build_path}"

# Configure project with unit tests and coverage instrumentation enabled
cmake -DCMAKE_SAFETYTEST_ARG="CMAKE_SAFETYTEST_ARG_ON" \
      -DDOTEST=ON \
      -DBUILD_TESTS=ON \
      -DUSE_PDFIUM_BUNDLE=ON \
      -DCMAKE_BUILD_TYPE=Debug \
      "${project_root}"

# Compile tests target
make -j"$(nproc)" test-deepin-reader

# Ensure report directory used by gtest exists inside the build tree
mkdir -p "${build_path}/report"

# Run tests and produce XML report
./tests/test-deepin-reader --gtest_output=xml:"${build_path}/report/report_deepin-reader.xml"

# Directory that holds coverage artifacts (build tree of the project)
workdir="${build_path}"

# Reset any stale coverage counters before recapture
lcov --directory "${workdir}" --zerocounters || true

# Re-run tests so .gcda files reflect a clean run
./tests/test-deepin-reader --gtest_output=xml:"${build_path}/report/report_deepin-reader.xml"

# Collect coverage data
lcov -d "${workdir}" -c -o ./coverage.info

# Keep only reader sources, drop tests themselves
lcov --extract ./coverage.info '*/reader/*' -o ./coverage.info
lcov --remove  ./coverage.info '*/tests/*' -o ./coverage.info

# Generate HTML report
genhtml -o ./html ./coverage.info

# Rename main index for downstream tooling
mv ./html/index.html ./html/cov_deepin-reader.html

# Publish artifacts into project report dir
cp -r html  "${report_path}/"
cp -r "${build_path}/report" "${report_path}/"
cp -r asan*.log* "${report_path}/asan_deepin-reader.log" 2>/dev/null || true

exit 0
