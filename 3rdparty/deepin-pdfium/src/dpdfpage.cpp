#include "dpdfdoc.h"
#include "dpdfpage.h"
#include "dpdfannot.h"

#include "public/fpdfview.h"
#include "public/fpdf_text.h"
#include "public/fpdf_annot.h"
#include "public/fpdf_doc.h"

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdftext/cpdf_textpage.h"
#include "core/fpdfdoc/cpdf_linklist.h"

#include <QDebug>

class DPdfPagePrivate
{
    friend class DPdfPage;
public:
    DPdfPagePrivate(DPdfDocHandler *handler, int index);

    ~DPdfPagePrivate();

private:
    FPDF_DOCUMENT m_doc = nullptr;

    int m_index = -1;

    double m_width = 0;

    qreal m_height = 0;

    FPDF_PAGE m_page = nullptr;

    FPDF_TEXTPAGE m_textPage = nullptr;

    QList<DPdfAnnot *> m_dAnnots;

};

DPdfPagePrivate::DPdfPagePrivate(DPdfDocHandler *handler, int index)
{
    m_doc = reinterpret_cast<FPDF_DOCUMENT>(handler);

    m_index = index;

    m_page = FPDF_LoadPage(m_doc, m_index);

    m_width = FPDF_GetPageWidth(m_page);

    m_height = FPDF_GetPageHeight(m_page);

    m_textPage = FPDFText_LoadPage(m_page);

    //获取当前注释
    int annotCount = FPDFPage_GetAnnotCount(m_page);

    for (int i = 0; i < annotCount; ++i) {
        FPDF_ANNOTATION annot = FPDFPage_GetAnnot(m_page, i);

        FPDF_ANNOTATION_SUBTYPE subType = FPDFAnnot_GetSubtype(annot);

        DPdfAnnot::AnnotType type = DPdfAnnot::AUnknown;

        if (FPDF_ANNOT_TEXT == subType)
            type = DPdfAnnot::AText;
        else if (FPDF_ANNOT_HIGHLIGHT == subType)
            type = DPdfAnnot::AHighlight;

//        @@@@@@@@  "Border"
//        @@@@@@@@  "C"
//        @@@@@@@@  "CA"
//        @@@@@@@@  "Contents"
//        @@@@@@@@  "F"
//        @@@@@@@@  "M"
//        @@@@@@@@  "NM"
//        @@@@@@@@  "P"
//        @@@@@@@@  "Rect"
//        @@@@@@@@  "Subtype"
//        @@@@@@@@  "T"
//        @@@@@@@@  "Type"

//    |----------------------------------|
//    |--------right-------              |
//    |                                  |
//    |----left----........              |
//    |            |.     .              |
//    |            |. . . .              |
//    |           top     |bottom        |
//    |            |      |              |
//    |----------------------------------|

        if (DPdfAnnot::AText == type) {
            DPdfTextAnnot *dAnnot = new DPdfTextAnnot;

            //获取位置
            FS_RECTF rectF;
            if (FPDFAnnot_GetRect(annot, &rectF)) {//注释图标为24x24
                QRectF annorectF(rectF.left, m_height - rectF.top, 20, 20);
                dAnnot->setRectF(annorectF);

                FS_RECTF newrectf;
                newrectf.left = annorectF.left();
                newrectf.top = m_height - annorectF.top();
                newrectf.right = annorectF.right();
                newrectf.bottom = m_height - annorectF.bottom();
                FPDFAnnot_SetRect(annot, &newrectf);
            }

            ulong quadCount = FPDFAnnot_CountAttachmentPoints(annot);
            QList<QRectF> list;
            for (ulong i = 0; i < quadCount; ++i) {
                FS_QUADPOINTSF quad;
                if (!FPDFAnnot_GetAttachmentPoints(annot, i, &quad))
                    continue;

                QRectF rectF;
                rectF.setX(static_cast<double>(quad.x1));
                rectF.setY(m_height - static_cast<double>(quad.y1));
                rectF.setWidth(static_cast<double>(quad.x2 - quad.x1));
                rectF.setHeight(static_cast<double>(quad.y1 - quad.y3));
                list.append(rectF);
            }

            //获取文本
            FPDF_WCHAR buffer[1024];
            FPDFAnnot_GetStringValue(annot, "Contents", buffer, 1024);
            dAnnot->m_text = QString::fromUtf16(buffer);

            m_dAnnots.append(dAnnot);
        } else if (DPdfAnnot::AHighlight == type) {
            DPdfHightLightAnnot *dAnnot = new DPdfHightLightAnnot;
            //获取颜色
            unsigned int r = 0;
            unsigned int g = 0;
            unsigned int b = 0;
            unsigned int a = 255;
            if (FPDFAnnot_GetColor(annot, FPDFANNOT_COLORTYPE_Color, &r, &g, &b, &a)) {
                dAnnot->setColor(QColor(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a)));
            }

            //获取区域
            ulong quadCount = FPDFAnnot_CountAttachmentPoints(annot);
            QList<QRectF> list;
            for (ulong i = 0; i < quadCount; ++i) {
                FS_QUADPOINTSF quad;
                if (!FPDFAnnot_GetAttachmentPoints(annot, i, &quad))
                    continue;

                QRectF rectF;
                rectF.setX(static_cast<double>(quad.x1));
                rectF.setY(m_height - static_cast<double>(quad.y1));
                rectF.setWidth(static_cast<double>(quad.x2 - quad.x1));
                rectF.setHeight(static_cast<double>(quad.y1 - quad.y3));

                list.append(rectF);
            }
            dAnnot->setBoundaries(list);

            //获取文本
            FPDF_WCHAR buffer[1024];
            FPDFAnnot_GetStringValue(annot, "Contents", buffer, 1024);
            dAnnot->m_text = QString::fromUtf16(buffer);

            m_dAnnots.append(dAnnot);
        }
        FPDFPage_CloseAnnot(annot);
    }
}

DPdfPagePrivate::~DPdfPagePrivate()
{
    if (m_textPage)
        FPDFText_ClosePage(m_textPage);

    if (m_page)
        FPDF_ClosePage(m_page);
}

DPdfPage::DPdfPage(DPdfDocHandler *handler, int pageIndex)
    : d_ptr(new DPdfPagePrivate(handler, pageIndex))
{

}

DPdfPage::~DPdfPage()
{

}

qreal DPdfPage::width() const
{
    return FPDF_GetPageWidth(d_func()->m_page);
}

qreal DPdfPage::height() const
{
    return FPDF_GetPageHeight(d_func()->m_page);
}

int DPdfPage::pageIndex() const
{
    return d_func()->m_index;
}

QImage DPdfPage::image(qreal xscale, qreal yscale, qreal x, qreal y, qreal width, qreal height)
{
    if (nullptr == d_func()->m_doc)
        return QImage();

    QImage image(width, height, QImage::Format_RGBA8888);

    if (image.isNull())
        return QImage();
    image.fill(0xFFFFFFFF);

    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(image.width(), image.height(),
                                             FPDFBitmap_BGRA,
                                             image.scanLine(0), image.bytesPerLine());

    if (bitmap == nullptr) {
        return QImage();
    }

    FPDF_RenderPageBitmap(bitmap, d_func()->m_page,
                          x, y, width, height,
                          xscale * this->width(), yscale * this->height(),
                          0, FPDF_ANNOT);
    FPDFBitmap_Destroy(bitmap);
    bitmap = nullptr;

    for (int i = 0; i < image.height(); i++) {
        uchar *pixels = image.scanLine(i);
        for (int j = 0; j < image.width(); j++) {
            qSwap(pixels[0], pixels[2]);
            pixels += 4;
        }
    }

    return image;
}

int DPdfPage::countChars() const
{
    return FPDFText_CountChars(d_func()->m_textPage);
}

QVector<QRectF> DPdfPage::getTextRect(int start, int charCount) const
{
    QVector<QRectF> result;
    std::vector<CFX_FloatRect> pdfiumRects = reinterpret_cast<CPDF_TextPage *>(d_func()->m_textPage)->GetRectArray(start, charCount);
    result.reserve(pdfiumRects.size());
    for (CFX_FloatRect &rect : pdfiumRects) {
        result.push_back({rect.left, height() - rect.top, rect.right - rect.left, rect.top - rect.bottom});
    }
    return result;
}

QString DPdfPage::text(const QRectF &rect) const
{
    qreal newBottom = height() - rect.bottom();
    qreal newTop = height() - rect.top();
    CFX_FloatRect fxRect(rect.left(), std::min(newBottom, newTop), rect.right(), std::max(newBottom, newTop));
    auto text = reinterpret_cast<CPDF_TextPage *>(d_func()->m_textPage)->GetTextByRect(fxRect);
    return QString::fromWCharArray(text.c_str(), text.GetLength());
}

QString DPdfPage::text(int start, int charCount) const
{
    auto text = reinterpret_cast<CPDF_TextPage *>(d_func()->m_textPage)->GetPageText(start, charCount);
    return QString::fromWCharArray(text.c_str(), charCount);
}

QList<DPdfAnnot *> DPdfPage::annots()
{
    QList<DPdfAnnot *> annots;
    for (DPdfAnnot *annot : d_func()->m_dAnnots) {
        if (annot->type() == DPdfAnnot::AHighlight || annot->type() == DPdfAnnot::AText)
            annots.append(annot);
    }

    return annots;
}

DPdfPage::Link DPdfPage::getLinkAtPoint(qreal x, qreal y)
{
    Link link;
    const FPDF_LINK &flink = FPDFLink_GetLinkAtPoint(d_func()->m_page, x, height() - y);
    CPDF_Link cLink(reinterpret_cast<CPDF_Dictionary *>(flink));
    if (cLink.GetDict() == nullptr)
        return link;

    CPDF_Document *pDoc = reinterpret_cast<CPDF_Document *>(d_func()->m_doc);
    const CPDF_Action &cAction = cLink.GetAction();
    const CPDF_Dest &dest = cAction.GetDest(pDoc);

    bool hasx = false, hasy = false, haszoom = false;
    float offsetx = 0.0, offsety = 0.0, z = 0.0;
    dest.GetXYZ(&hasx, &hasy, &haszoom, &offsetx, &offsety, &z);
    link.left = offsetx;
    link.top = offsety;

    link.nIndex = dest.GetDestPageIndex(pDoc);
    if (cAction.GetType() == CPDF_Action::URI) {
        const ByteString &bUrl = cAction.GetURI(pDoc);
        link.urlpath = QString::fromUtf8(bUrl.c_str(), bUrl.GetLength());
        if (!link.urlpath.contains("http://") && !link.urlpath.contains("https://"))
            link.urlpath.prepend("http://");
    } else {
        const WideString &wFilepath = cAction.GetFilePath();
        link.urlpath = QString::fromWCharArray(wFilepath.c_str(), wFilepath.GetLength());
    }
    return link;
}

DPdfAnnot *DPdfPage::createTextAnnot(QPointF point, QString text)
{
    FPDF_ANNOTATION_SUBTYPE subType = FPDF_ANNOT_TEXT;

    FPDF_ANNOTATION annot = FPDFPage_CreateAnnot(d_func()->m_page, subType);

    if (!FPDFAnnot_SetStringValue(annot, "Contents", text.utf16())) {
        FPDFPage_CloseAnnot(annot);
        return nullptr;
    }

    FS_RECTF rectF;
    rectF.left = point.x() - 10;
    rectF.top = height() - point.y() + 10;
    rectF.right = point.x() + 10;
    rectF.bottom = rectF.top - 20;

    if (!FPDFAnnot_SetRect(annot, &rectF)) {
        FPDFPage_CloseAnnot(annot);
        return nullptr;
    }

    FPDFPage_CloseAnnot(annot);

    DPdfTextAnnot *dAnnot = new DPdfTextAnnot;

    dAnnot->setRectF(QRectF(point.x() - 10, point.y() - 10, 20, 20));

    dAnnot->setText(text);

    d_func()->m_dAnnots.append(dAnnot);

    emit annotAdded(dAnnot);

    return dAnnot;
}

bool DPdfPage::updateTextAnnot(DPdfAnnot *dAnnot, QString text, QPointF point)
{
    DPdfTextAnnot *textAnnot = static_cast<DPdfTextAnnot *>(dAnnot);

    if (nullptr == textAnnot)
        return false;

    int index = d_func()->m_dAnnots.indexOf(dAnnot);

    FPDF_ANNOTATION annot = FPDFPage_GetAnnot(d_func()->m_page, index);

    if (!FPDFAnnot_SetStringValue(annot, "Contents", text.utf16())) {
        FPDFPage_CloseAnnot(annot);
        return false;
    }
    textAnnot->setText(text);

    if (!point.isNull()) {
        FS_RECTF rectF;
        rectF.left = point.x() - 10;
        rectF.top = height() - point.y() + 10;
        rectF.right = point.x() + 10;
        rectF.bottom = rectF.top - 20;

        if (!FPDFAnnot_SetRect(annot, &rectF)) {
            FPDFPage_CloseAnnot(annot);
            return false;
        }

        textAnnot->setRectF(QRectF(point.x() - 10, point.y() - 10, 20, 20));
    }

    FPDFPage_CloseAnnot(annot);

    emit annotUpdated(dAnnot);

    return true;
}

DPdfAnnot *DPdfPage::createHightLightAnnot(QList<QRectF> list, QString text, QColor color)
{
    FPDF_ANNOTATION_SUBTYPE subType = FPDF_ANNOT_HIGHLIGHT;

    FPDF_ANNOTATION annot = FPDFPage_CreateAnnot(d_func()->m_page, subType);

    if (color.isValid() && !FPDFAnnot_SetColor(annot, FPDFANNOT_COLORTYPE_Color, color.red(), color.green(), color.blue(), color.alpha())) {
        FPDFPage_CloseAnnot(annot);
        return nullptr;
    }

    for (QRectF rect : list) {
        FS_QUADPOINTSF quad;
        quad.x1 = rect.x();
        quad.y1 = d_func()->m_height - rect.y();
        quad.x2 = rect.x() + rect.width();
        quad.y2 = d_func()->m_height - rect.y();
        quad.x3 = rect.x();
        quad.y3 = d_func()->m_height - rect.y() - rect.height();
        quad.x4 = rect.x() + rect.width();
        quad.y4 = d_func()->m_height - rect.y() - rect.height();

        if (!FPDFAnnot_AppendAttachmentPoints(annot, &quad))
            continue;
    }

    if (!FPDFAnnot_SetStringValue(annot, "Contents", text.utf16())) {
        FPDFPage_CloseAnnot(annot);
        return nullptr;
    }

    FPDFPage_CloseAnnot(annot);

    DPdfHightLightAnnot *dAnnot = new DPdfHightLightAnnot;

    dAnnot->setBoundaries(list);

    dAnnot->setColor(color);

    dAnnot->setText(text);

    d_func()->m_dAnnots.append(dAnnot);

    emit annotAdded(dAnnot);

    return dAnnot;
}

bool DPdfPage::updateHightLightAnnot(DPdfAnnot *dAnnot, QColor color, QString text)
{
    DPdfHightLightAnnot *hightLightAnnot = static_cast<DPdfHightLightAnnot *>(dAnnot);

    if (nullptr == hightLightAnnot)
        return false;

    int index = d_func()->m_dAnnots.indexOf(dAnnot);

    FPDF_ANNOTATION annot = FPDFPage_GetAnnot(d_func()->m_page, index);

    if (color.isValid()) {
        if (!FPDFAnnot_SetColor(annot, FPDFANNOT_COLORTYPE_Color, color.red(), color.green(), color.blue(), color.alpha())) {
            FPDFPage_CloseAnnot(annot);
            return false;
        }
        hightLightAnnot->setColor(color);
    }

    if (!FPDFAnnot_SetStringValue(annot, "Contents", text.utf16())) {
        FPDFPage_CloseAnnot(annot);
        return false;
    }
    hightLightAnnot->setText(text);

    FPDFPage_CloseAnnot(annot);

    emit annotUpdated(dAnnot);

    return true;
}

bool DPdfPage::removeAnnot(DPdfAnnot *dAnnot)
{
    int index = d_func()->m_dAnnots.indexOf(dAnnot);

    if (index < 0)
        return false;

    if (!FPDFPage_RemoveAnnot(d_func()->m_page, index))
        return false;

    d_func()->m_dAnnots.removeOne(dAnnot);

    emit annotRemoved(dAnnot);

    delete dAnnot;

    return true;
}

QVector<QRectF> DPdfPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    QVector<QRectF> rectfs;
    int flags = 0x00000000;

    if (matchCase)
        flags |= FPDF_MATCHCASE;

    if (wholeWords)
        flags |= FPDF_MATCHWHOLEWORD;

    FPDF_SCHHANDLE schandle = FPDFText_FindStart(d_func()->m_textPage, text.utf16(), flags, 0);
    if (schandle) {
        while (FPDFText_FindNext(schandle)) {
            int curSchIndex = FPDFText_GetSchResultIndex(schandle);
            if (curSchIndex >= 0) {
                const QVector<QRectF> &textrectfs = getTextRect(curSchIndex, text.length());
                rectfs << textrectfs;
            }
        };
    }

    FPDFText_FindClose(schandle);
    return rectfs;
}
