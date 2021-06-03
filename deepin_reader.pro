TEMPLATE = subdirs

#QMAKE_CXX = ccache $$QMAKE_CXX //linux使用ccache加速c++编译速度

CONFIG  += ordered

SUBDIRS += 3rdparty/deepin-pdfium

SUBDIRS += reader

SUBDIRS += tests

