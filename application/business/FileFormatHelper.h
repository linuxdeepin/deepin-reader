#ifndef FILEFORMATHELPER_H
#define FILEFORMATHELPER_H

#include <QString>
#include "../ModelService/ModuleHeader.h"
#include "docview/commonstruct.h"

namespace  FFH {
static QString getFileFilter(const int &nCurDocType)
{
    QString sFilter = "";
    if (nCurDocType == DocType_PDF) {
        sFilter = Constant::sPdf_Filter;
    }
    /*
    else if (nCurDocType == DocType_TIFF) {
        sFilter = Constant::sTiff_Filter;
    } else if (nCurDocType == DocType_PS) {
        sFilter = Constant::sPs_Filter;
    } else if (nCurDocType == DocType_XPS) {
        sFilter = Constant::sXps_Filter;
    } else if (nCurDocType == DocType_DJVU) {
        sFilter = Constant::sDjvu_Filter;
    }
    */
    return sFilter;
}

static QString getFilePath(const QString &inputPath, const int &nCurDocType)
{
    QString filePath = inputPath;
    if (nCurDocType == DocType_PDF) {
        if (!filePath.endsWith(".pdf")) {
            filePath += ".pdf";
        }
    }
    /*else if (nCurDocType == DocType_TIFF) {
        if (!filePath.endsWith(".tiff")) {
            filePath += ".tiff";
        }
    } else if (nCurDocType == DocType_PS) {
        if (!filePath.endsWith(".ps")) {
            filePath += ".ps";
        }
    } else if (nCurDocType == DocType_XPS) {
        if (!filePath.endsWith(".xps")) {
            filePath += ".xps";
        }
    } else if (nCurDocType == DocType_DJVU) {
        if (!filePath.endsWith(".djvu")) {
            filePath += ".djvu";
        }
    }
    */
    return filePath;
}

static DocType_EM setCurDocuType(const QString &sCompleteSuffix)
{
    DocType_EM m_nCurDocType = DocType_NULL;
    if (sCompleteSuffix == "pdf" || sCompleteSuffix.endsWith("pdf")) {
        m_nCurDocType = DocType_PDF;
    }
    /*
    else if (sCompleteSuffix == "tiff" || sCompleteSuffix.endsWith("tiff")) {
        m_nCurDocType = DocType_TIFF;
    } else if (sCompleteSuffix == "ps" || sCompleteSuffix.endsWith("ps")) {
        m_nCurDocType = DocType_PS;
    } else if (sCompleteSuffix == "xps" || sCompleteSuffix.endsWith("xps")) {
        m_nCurDocType = DocType_XPS;
    } else if (sCompleteSuffix == "djvu" || sCompleteSuffix.endsWith("djvu")) {
        m_nCurDocType = DocType_DJVU;
    }
    */
    return m_nCurDocType;
}

}

#endif // FILEFORMATHELPER_H
