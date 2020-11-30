#!/bin/bash

workdir=. #编译路径

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$workdir/../3rdparty/deepin-pdfium/lib

executable=test-deepin-reader #可执行程序的文件名

build_dir=$workdir

result_coverage_dir=$build_dir/coverage

result_report_dir=$build_dir/report/report.xml

#下面是覆盖率目录操作，一种正向操作，一种逆向操作
extract_info="*/deepin-reader/reader/*"  #针对当前目录进行覆盖率操作

remove_info="*/deepin-reader/test/*" #排除当前目录进行覆盖率操作

$build_dir/$executable --gtest_output=xml:$result_report_dir

lcov -d $build_dir -c -o $build_dir/coverage.info

lcov --remove $build_dir/coverage.info $remove_info --output-file $build_dir/coverage.info

lcov --extract $build_dir/coverage.info $extract_info --output-file  $build_dir/coverage.info

lcov --list-full-path -e $build_dir/coverage.info –o $build_dir/coverage-stripped.info

genhtml -o $result_coverage_dir $build_dir/coverage.info

nohup x-www-browser $result_coverage_dir/index.html &

nohup x-www-browser $result_report_dir &
 
lcov -d $build_dir –z

exit 0
