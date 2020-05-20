TEMPLATE = subdirs

TRANSLATIONS += \
    ./translations/deepin-reader_en_US.ts\
    ./translations/deepin-reader_zh_CN.ts

CONFIG(release, debug|release) {
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    TRANSLATIONFILES= $$files($$PWD/translations/*.ts)
    for(tsfile, TRANSLATIONFILES) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
    #将qm文件添加到安装包
    dtk_translations.path = /usr/share/$$TARGET/translations
    dtk_translations.files = $$PWD/translations/*.qm
    INSTALLS += dtk_translations
}

SUBDIRS += \
    src

#QMAKE_CFLAGS += -g -O0
#QMAKE_CXXFLAGS += -g -O0
