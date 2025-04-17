// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtCore>
#include <QString>
#include <QDebug>
#include "dpdfglobal.h"
#include "public/fpdfview.h"

#include <chardet.h>

static bool initialized = false;

const static DPdfGlobal instance = DPdfGlobal();

void DPdfGlobal::init()
{
    if (!initialized) {
        FPDF_InitLibrary();
        initialized = true;
    }
}

void DPdfGlobal::destory()
{
    if (initialized) {
        FPDF_DestroyLibrary();
        initialized = false;
    }
}

DPdfGlobal::DPdfGlobal()
{
    init();
}

DPdfGlobal::~DPdfGlobal()
{
    destory();
}

QString DPdfGlobal::textCodeType(const char *text)
{
    DetectObj *obj = detect_obj_init();
    detect(text, &obj);
    const QString &encodeind = QString(obj->encoding).toLower();
    detect_obj_free(&obj);
    return encodeind;
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Q_GLOBAL_STATIC_WITH_ARGS(QMutex, pdfMutex, (QMutex::Recursive));

DPdfMutexLocker::DPdfMutexLocker(const QString &tmpLog): QMutexLocker(pdfMutex())
#else
Q_GLOBAL_STATIC(QRecursiveMutex, pdfMutex);

DPdfMutexLocker::DPdfMutexLocker(const QString &tmpLog): QMutexLocker<QRecursiveMutex>(pdfMutex())
#endif
{
    m_log = tmpLog;
    qInfo() << m_log + " begin ";
    m_timer.start();
}

DPdfMutexLocker::~DPdfMutexLocker()
{
    qInfo() << m_log + " end time = " << m_timer.elapsed();
}
