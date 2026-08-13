#!/bin/bash

# SPDX-FileCopyrightText: 2023-2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

set -u

export builddir=build
export reportdir=build-ut
export scriptdir="$(cd "$(dirname "$0")" && pwd)"
export projectdir="$(cd "${scriptdir}/.." && pwd)"

build_path="${projectdir}/${builddir}"
report_path="${projectdir}/${reportdir}"

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
      "${projectdir}"

# Compile tests target
make -j"$(nproc)" test-deepin-reader

# Ensure report directory used by gtest exists inside the build tree
mkdir -p "${build_path}/report"

# Run tests and produce XML report
set +e
./tests/test-deepin-reader --gtest_output=xml:"${build_path}/report/report_deepin-reader.xml"
test_exit_code=$?
set -e

# Directory that holds coverage artifacts (build tree of the project)
workdir="${build_path}"

# Reset any stale coverage counters before recapture
lcov --directory "${workdir}" --zerocounters || true

# Re-run tests so .gcda files reflect a clean run
set +e
./tests/test-deepin-reader --gtest_output=xml:"${build_path}/report/report_deepin-reader.xml"
set -e

# Collect coverage data
lcov -d "${workdir}" -c -o ./coverage.info

# Keep only reader sources, drop tests themselves
lcov --extract ./coverage.info '*/reader/*' -o ./coverage.info
lcov --remove  ./coverage.info '*/tests/*' -o ./coverage.info

# Exclude compiler-generated and unreachable functions (D0Ev, Q_OBJECT tr, env-dependent lambdas)
python3 "${scriptdir}/exclude_unreachable.py" ./coverage.info ./coverage.info

# Generate HTML report
genhtml -o ./html ./coverage.info

# Rename main index for downstream tooling
mv ./html/index.html ./html/cov_deepin-reader.html

# Publish artifacts into project report dir
cp -r html  "${report_path}/"
cp -r "${build_path}/report" "${report_path}/"
cp -r asan*.log* "${report_path}/asan_deepin-reader.log" 2>/dev/null || true

# 生成摘要 JSON
echo "==> Generating summary JSON: ${report_path}/ut-summary.json"

python3 "${scriptdir}/gen-ut-summary.py"

exit $test_exit_code
