#ifndef DPDFGLOBAL_H
#define DPDFGLOBAL_H

#include <QtCore/qglobal.h>
#include <QMutexLocker>

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

private:
    void init();

    void destory();
};

//pdfium即使不同文档之间loadpage和renderpage也不是线程安全，需要加锁
class DPdfMutexLocker : public QMutexLocker
{
public:
    DPdfMutexLocker();
};

#endif // DPDFGLOBAL_H
