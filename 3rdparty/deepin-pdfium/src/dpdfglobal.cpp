
#include <QtCore>
#include <QString>

#include "dpdfglobal.h"
#include "public/fpdfview.h"

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

Q_GLOBAL_STATIC_WITH_ARGS(QMutex, pdfMutex, (QMutex::Recursive));

DPdfMutexLocker::DPdfMutexLocker(): QMutexLocker(pdfMutex())
{

}
