#ifndef FRAME_H
#define FRAME_H

#include <QObject>

//  用于　翻译

namespace Frame {

const QString sAppName = QObject::tr("deepin_reader");
const QString sDescription = QObject::tr("Document viewer is a document viewer that comes with the deep operating system. \r\n"
                                         "In addition to opening and reading PDF files, you can also do \r\n on documents"
                                         "Bookmark, comment, and highlight selected text．");
}

namespace COMMON {

const QString PAGE_PREF = QObject::tr("page ");
const QString PAGES     = QObject::tr("pages");

}

namespace BOOKMARK {

const QString DLT_BK    = QObject::tr("delete bookmark") ;
const QString ADD_BK    = QObject::tr("add bookmark");
const QString DEL_KEY   = QObject::tr("Del");
const QString COMMA     = QObject::tr(",");

}

namespace FONT {

const QString SCALE_CURRENT = QObject::tr("100%");
const QString SCALE_SIG     = QObject::tr("A");
const QString DOUB_VIEW     = QObject::tr("Doub View");
const QString ADAP_HEIGHT   = QObject::tr("Adaptate Height");
const QString ADAP_WIDTH    = QObject::tr("Adaptate Width");
const QString ROTAT_TO_L    = QObject::tr("Rotated To Left");
const QString ROTAT_TO_R    = QObject::tr("Rotated To Right");

}

namespace NOTE {

const QString COPY_CONT = QObject::tr("Copy");
const QString DLT_NOTE  = QObject::tr("Del Note");
const QString ADD_NOTE  = QObject::tr("add note");

}

namespace SEARCH {

const QString SEARCH_RES_CONT = QObject::tr("个结果");

}

#endif // FRAME_H
