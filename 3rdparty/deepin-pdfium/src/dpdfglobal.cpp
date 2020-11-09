
#include <QtCore>
#include <QString>

#include "dpdfglobal.h"
#include "public/fpdfview.h"

static bool initialized = false;

const static DPDFGlobal instance = DPDFGlobal();

void DPDFGlobal::init()
{
    if (!initialized) {
        FPDF_InitLibrary();
        initialized = true;
    }
}

void DPDFGlobal::destory()
{
    if (initialized) {
        FPDF_DestroyLibrary();
        initialized = false;
    }
}

DPDFGlobal::DPDFGlobal()
{
    init();
}

DPDFGlobal::~DPDFGlobal()
{
    destory();
}

