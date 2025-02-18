TEMPLATE = subdirs

#QMAKE_CXX = ccache $$QMAKE_CXX //linux使用ccache加速c++编译速度
#config

# 基础 Qt 模块
versionAtLeast(QT_VERSION, 6.0.0) {
    QT += core gui widgets network dbus sql svg webchannel webenginewidgets concurrent xml core5compat

    # Qt6 specific configurations
    PKGCONFIG += dtk6widget dtk6gui dtk6core

    # Qt6 lrelease configuration
    QMAKE_LRELEASE = /usr/lib/qt6/bin/lrelease

    LIBS += -L$$PREFIX/lib/$$QMAKE_HOST.arch-linux-gnu/ -lKF6GlobalAccel -lKF6I18n
    INCLUDEPATH += $$PREFIX/include/KF6/KGlobalAccel
    INCLUDEPATH += $$PREFIX/include/KF6/KI18n

} else {
    QT += core gui widgets network sql dbus svg webchannel webenginewidgets concurrent xml

    # Qt5 specific configurations
    PKGCONFIG += dtkwidget dframeworkdbus poppler-qt5

    QMAKE_LRELEASE = lrelease

    LIBS += -L$$PREFIX/lib/$$QMAKE_HOST.arch-linux-gnu/ -lKF5GlobalAccel -lKF5I18n
    INCLUDEPATH += $$PREFIX/include/KF5/KGlobalAccel
    INCLUDEPATH += $$PREFIX/include/KF5/KI18n
}

# 添加库
LIBS += `pkg-config --libs libjpeg`
LIBS += `pkg-config --libs ddjvu`

# 如果需要包含头文件的路径，可以添加
INCLUDEPATH += `pkg-config --cflags libjpeg`
INCLUDEPATH += `pkg-config --cflags ddjvu`

CONFIG  += ordered

###安全漏洞检测
#QMAKE_CXX += -g -fsanitize=undefined,address -O2
#QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
#QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2

message("Build on host arch: $$QMAKE_HOST.arch")

SUBDIRS += 3rdparty/deepin-pdfium

SUBDIRS += htmltopdf

SUBDIRS += reader

# Execute translation script with Qt version parameter
versionAtLeast(QT_VERSION, 6.0.0) {
    system($$PWD/translate_generation.sh 6)
} else {
    system($$PWD/translate_generation.sh 5)
}

# Install translations
translations.path = $$PREFIX/share/deepin-reader/translations
translations.files = $$PWD/translations/*.qm
INSTALLS += translations

