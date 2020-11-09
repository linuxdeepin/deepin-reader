TEMPLATE = subdirs

CONFIG += ordered

CONFIG(release, debug|release) {
SUBDIRS += 3rdparty/deepin-pdfium
}

CONFIG(debug, debug|release) {
SUBDIRS += 3rdparty/deepdf
}

SUBDIRS += src
