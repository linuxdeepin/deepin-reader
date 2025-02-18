#!/bin/bash
# this file is used to auto-generate .qm file from .ts file.
# author: shibowen at linuxdeepin.com

# 获取传入的QT版本参数
QT_VERSION_MAJOR=$1

# 确保QT_VERSION_MAJOR是一个整数
if ! [[ "$QT_VERSION_MAJOR" =~ ^[0-9]+$ ]]; then
    echo "Error: QT_VERSION_MAJOR is not a valid integer."
    exit 1
fi

ts_list=(`ls translations/*.ts`)

for ts in "${ts_list[@]}"
do
    printf "\nprocess ${ts}\n"
    # 根据QT版本选择lrelease命令
    if [ "$QT_VERSION_MAJOR" -eq 6 ]; then
        /usr/lib/qt6/bin/lrelease "${ts}"
    else
        lrelease "${ts}"
    fi
done