#!/bin/bash

# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

builddir=build
reportdir=build-ut
rm -rf $builddir
rm -rf ../$builddir
rm -rf $reportdir
rm -rf ../$reportdir
mkdir ../$builddir
mkdir ../$reportdir
cd ../$builddir
#编译
#qmake DEFINES+=CMAKE_SAFETYTEST_ARG_ON ../deepin_reader.pro
echo "当前目录："
pwd
echo $builddir

qmake DEFINES+=CMAKE_SAFETYTEST_ARG_ON ../tests/tests.pro
make -j8

echo "编译完成！"
echo "当前目录："
pwd

echo "可执行程序的文件名"
executable=test-deepin-reader #可执行程序的文件名

#下面是覆盖率目录操作，一种正向操作，一种逆向操作
echo "针对当前目录进行覆盖率操作"
extract_info="*/deepin-reader/*" #针对当前目录进行覆盖率操作
echo "排除当前目录进行覆盖率操作"
remove_info="*tests* *build* *build-ut* *3rdparty*" #排除当前目录进行覆盖率操作

echo ">>>>> 1"
workdir=$(cd ../$(dirname $0)/$builddir; pwd)

echo "当前目录："
pwd

echo "生成asan日志和ut测试xml结果"
#生成asan日志和ut测试xml结果
./$executable --gtest_output=xml:./report/report_deepin-reader.xml



echo ">>>>> 2"
mkdir -p report
#统计代码覆盖率并生成html报告
echo ">>>>> 3"
lcov -d $workdir -c -o ./coverage.info

echo ">>>>> 4"
lcov --extract ./coverage.info $extract_info -o  ./coverage.info

echo ">>>>> 5"
lcov --remove ./coverage.info $remove_info -o  ./coverage.info

echo ">>>>> 6"
genhtml -o ./html ./coverage.info

echo ">>>>> 7"
mv ./html/index.html ./html/cov_deepin-reader.html
echo ">>>>> 8"
#对asan、ut、代码覆盖率结果收集至指定文件夹
cp -r html ../$reportdir/
cp -r report ../$reportdir/
cp -r asan*.log* ../$reportdir/asan_deepin-reader.log
echo ">>>>> 9"

exit 0
