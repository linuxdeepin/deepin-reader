
#include <QtCore>
#include <QString>

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

Q_GLOBAL_STATIC_WITH_ARGS(QMutex, pdfMutex, (QMutex::Recursive));

DPdfMutexLocker::DPdfMutexLocker(): QMutexLocker(pdfMutex())
{

}
