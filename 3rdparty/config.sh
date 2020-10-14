#!/bin/bash

mkdir deepin-pdfium
cd deepin-pdfium
git init
git remote add origin https://gitee.com/pigzhang/deepin-pdfium.git
git fetch origin 2c49f7169bd828672200a25974c3e4ddcb72c461
git reset --hard FETCH_HEAD
qmake
make -j16
make clean
