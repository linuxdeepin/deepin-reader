#!/bin/bash

mkdir deepin-pdfium
cd deepin-pdfium
git init
git remote add origin https://gitee.com/pigzhang/deepin-pdfium.git
git fetch origin d4f33e767d9b921ad6e2de45ac0eb4b91afdaa91
git reset --hard FETCH_HEAD
qmake
make -j16
make clean
