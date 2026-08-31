#!/bin/bash

# SPDX-FileCopyrightText: 2023-2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

set -u

export builddir=build-ut
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

# DB 隔离: 重定向 Qt AppDataLocation,避免读写用户数据及跨运行状态残留
export XDG_DATA_HOME="${build_path}/ut-testdata"
rm -rf "${XDG_DATA_HOME}"
mkdir -p "${XDG_DATA_HOME}"

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
# If the first run segfaulted, .gcda files won't exist (atexit not called on SIGSEGV).
# This re-run gives another chance; we also add a SIGSEGV handler as safety net.
rm -rf "${XDG_DATA_HOME}"
mkdir -p "${XDG_DATA_HOME}"
set +e
./tests/test-deepin-reader --gtest_output=xml:"${build_path}/report/report_deepin-reader.xml"
retest_exit_code=$?
set -e

# libdjvulibre21 全局析构 bug: 进程退出阶段 free() 非法指针 → SIGABRT(134),
# 此时所有测试已通过且 XML 已写出。若 exit=134 且 XML failures=0,则视为通过
is_djvu_exit_crash() {
    [ "$1" -eq 134 ] || return 1
    local xml="${build_path}/report/report_deepin-reader.xml"
    [ -f "$xml" ] || return 1
    grep -q 'failures="0"' "$xml" 2>/dev/null
}

if is_djvu_exit_crash "$test_exit_code"; then
    echo "Note: first run exit 134 (djvulibre exit-time abort), but all tests passed — ignoring"
    test_exit_code=0
fi
if is_djvu_exit_crash "$retest_exit_code"; then
    echo "Note: re-run exit 134 (djvulibre exit-time abort), but all tests passed — ignoring"
    retest_exit_code=0
fi

# Use the worst exit code between first and second run
test_exit_code=$((test_exit_code || retest_exit_code))

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
# If report_path differs from build_path, copy; otherwise files are already in place
if [ "${report_path}" != "${build_path}" ]; then
    cp -r html  "${report_path}/"
    cp -r "${build_path}/report" "${report_path}/"
fi
cp -r asan*.log* "${report_path}/asan_deepin-reader.log" 2>/dev/null || true

# 生成摘要 JSON
echo "==> Generating summary JSON: ${report_path}/ut-summary.json"

python3 "${scriptdir}/gen-ut-summary.py"

exit $test_exit_code
