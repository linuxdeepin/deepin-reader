TEMPLATE = subdirs

###安全漏洞检测
#QMAKE_CXX += -g -fsanitize=undefined,address -O2
#QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
#QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2
# 添加库
LIBS += -ldjvulibre -ljpeg 


SUBDIRS += src
