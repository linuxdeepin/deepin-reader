#ifndef PDFCONTROL_H
#define PDFCONTROL_H

#include <QObject>

namespace PdfControl {

const QString PAGE_PREF = QObject::tr("page ");
const QString PAGES     = QObject::tr("pages");

const QString DLT_BK    = QObject::tr("delete bookmark") ;
const QString ADD_BK    = QObject::tr("add bookmark");
const QString DEL_KEY   = QObject::tr("Del");

const QString DOUB_VIEW     = QObject::tr("Doub View");
const QString ADAP_HEIGHT   = QObject::tr("Adaptate Height");
const QString ADAP_WIDTH    = QObject::tr("Adaptate Width");
const QString ROTAT_TO_L    = QObject::tr("Rotated To Left");
const QString ROTAT_TO_R    = QObject::tr("Rotated To Right");

const QString COPY_CONT = QObject::tr("Copy");
const QString DLT_NOTE  = QObject::tr("Del Note");
const QString ADD_NOTE  = QObject::tr("add note");

const QString SEARCH_RES_CONT = QObject::tr("个结果");
}

#endif // PDFCONTROL_H
