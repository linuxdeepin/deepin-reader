TEMPLATE = subdirs

#QMAKE_CXX = ccache $$QMAKE_CXX //linux使用ccache加速c++编译速度

CONFIG  += ordered

###安全漏洞检测
#QMAKE_CXX += -g -fsanitize=undefined,address -O2
#QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
#QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2

SUBDIRS += 3rdparty/deepin-pdfium

SUBDIRS += htmltopdf

SUBDIRS += reader

###relseas不编译ut
CONFIG += debug_and_release
CONFIG(debug, debug|release) {
    message("debug!!!")
       SUBDIRS += tests
}else{
    message("release!!!")
}
