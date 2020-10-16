
#include <QtCore>
#include <QString>
#include "dpdfglobal.h"
#include "public/fpdfview.h"

bool initialized = false;

const static DPDFGlobal instance = DPDFGlobal();

void DPDFGlobal::initPdfium()
{
    if (!initialized) {
        FPDF_InitLibrary();
        initialized = true;
    }
}

void DPDFGlobal::shutdownPdfium()
{
    if (initialized) {
        FPDF_DestroyLibrary();
        initialized = false;
    }
}


DPDFGlobal::DPDFGlobal()
{
    initPdfium();
}

DPDFGlobal::~DPDFGlobal()
{
    shutdownPdfium();
}

