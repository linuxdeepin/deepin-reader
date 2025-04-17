// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DPDFGLOBAL_H
#define DPDFGLOBAL_H

#include <QtCore/qglobal.h>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QElapsedTimer>
#include <QRectF>
#include <QRecursiveMutex>

#ifndef BUILD_DEEPDF_STATIC
#    if defined(BUILD_DEEPDF_LIB)
#      define DEEPDF_EXPORT Q_DECL_EXPORT
#    else
#      define DEEPDF_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define DEEPDF_EXPORT
#endif

class DPdfGlobal
{
public:
    DPdfGlobal();

    ~DPdfGlobal();

    static QString textCodeType(const char *text);

    typedef struct {
        QString text;
        QRectF  rect;
    } PageLine;
    /**
     * @brief PageSection 一个选区的集合
     * page包含多个section
     * section包含多个line
     * line包含一个text和一个rect
     */
    typedef QList<PageLine> PageSection;

private:
    void init();

    void destory();
};

//pdfium即使不同文档之间loadpage和renderpage也不是线程安全，需要加锁
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
class DPdfMutexLocker : public QMutexLocker
#else
class DPdfMutexLocker : public QMutexLocker<QRecursiveMutex>
#endif
{
public:
    explicit DPdfMutexLocker(const QString &tmpLog);
    ~DPdfMutexLocker();

    QString m_log;
    QElapsedTimer m_timer;
};

#endif // DPDFGLOBAL_H
