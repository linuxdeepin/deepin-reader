#include "dpdfdoc.h"
#include "dpdfpage.h"
#include "dpdfannot.h"

#include "public/fpdfview.h"
#include "public/fpdf_text.h"
#include "public/fpdf_annot.h"
#include "public/fpdf_doc.h"
#include "public/fpdf_edit.h"

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdftext/cpdf_textpage.h"
#include "core/fpdfdoc/cpdf_linklist.h"
#include "fpdfsdk/cpdfsdk_helpers.h"

#include <QDebug>

class DPdfPagePrivate
{
    friend class DPdfPage;
public:
    DPdfPagePrivate(DPdfDocHandler *handler, int index);

    ~DPdfPagePrivate();

public:
    void loadPage();

    void loadTextPage();

    /**
     * @brief 文档自身旋转
     * @return
     */
    int oriRotation();

private:
    /**
     * @brief 加载注释,无需初始化，注释的坐标取值不受页自身旋转影响
     * @return 加载失败说明该页存在问题
     */
    bool loadAnnots();

    /**
     * @brief 视图坐标转化为文档坐标
     * @param rotation 文档自身旋转
     * @param rect
     * @return
     */
    FS_RECTF transRect(const int &rotation, const QRectF &rect);

    /**
     * @brief 文档坐标转化视图坐标
     * @param rotation 文档自身旋转
     * @param rect
     * @return
     */
    QRectF transRect(const int &rotation, const FS_RECTF &rect);

private:
    FPDF_DOCUMENT m_doc = nullptr;

    int m_index = -1;

    qreal m_width = 0;

    qreal m_height = 0;

    FPDF_PAGE m_page = nullptr;

    FPDF_TEXTPAGE m_textPage = nullptr;

    QList<DPdfAnnot *> m_dAnnots;

    bool m_isValid = false;
};

DPdfPagePrivate::DPdfPagePrivate(DPdfDocHandler *handler, int index)
{
    m_doc = reinterpret_cast<FPDF_DOCUMENT>(handler);

    m_index = index;

    //宽高会受自身旋转值影响
    FPDF_GetPageSizeByIndex(m_doc, index, &m_width, &m_height);

    m_isValid = loadAnnots();
}

DPdfPagePrivate::~DPdfPagePrivate()
{
    if (m_textPage)
        FPDFText_ClosePage(m_textPage);

    if (m_page)
        FPDF_ClosePage(m_page);

    qDeleteAll(m_dAnnots);
}

void DPdfPagePrivate::loadPage()
{
    if (nullptr == m_page) {
        m_page = FPDF_LoadPage(m_doc, m_index);
    }
}

void DPdfPagePrivate::loadTextPage()
{
    loadPage();

    if (nullptr == m_textPage)
        m_textPage = FPDFText_LoadPage(m_page);
}

int DPdfPagePrivate::oriRotation()
{
    if (nullptr == m_page) {
        FPDF_PAGE page = FPDF_LoadNoParsePage(m_doc, m_index);

        CPDF_Page *pPage = CPDFPageFromFPDFPage(page);

        int rotation = pPage->GetPageRotation();

        FPDF_ClosePage(page);

        return rotation;
    }

    return FPDFPage_GetRotation(m_page);

}

bool DPdfPagePrivate::loadAnnots()
{
    //使用临时page，不完全加载,防止刚开始消耗时间过长
    FPDF_PAGE page = FPDF_LoadNoParsePage(m_doc, m_index);

    if (nullptr == page) {
        return false;
    }

    CPDF_Page *pPage = CPDFPageFromFPDFPage(page);

    int rotation = pPage->GetPageRotation();

    //获取当前注释
    int annotCount = FPDFPage_GetAnnotCount(page);

    for (int i = 0; i < annotCount; ++i) {
        FPDF_ANNOTATION annot = FPDFPage_GetAnnot(page, i);

        FPDF_ANNOTATION_SUBTYPE subType = FPDFAnnot_GetSubtype(annot);

        DPdfAnnot::AnnotType type = DPdfAnnot::AUnknown;

        if (FPDF_ANNOT_TEXT == subType)
            type = DPdfAnnot::AText;
        else if (FPDF_ANNOT_HIGHLIGHT == subType)
            type = DPdfAnnot::AHighlight;
        else if (FPDF_ANNOT_LINK == subType)
            type = DPdfAnnot::ALink;

        //取出的rect为基于自身旋转前，现将转成基于旋转后的 m_width/m_height 为受旋转影响后的宽高
        qreal actualHeight = (rotation % 2 == 0) ? m_height : m_width;
        if (DPdfAnnot::AText == type) {
            DPdfTextAnnot *dAnnot = new DPdfTextAnnot;

            //获取位置
            FS_RECTF rectF;
            if (FPDFAnnot_GetRect(annot, &rectF)) { //注释图标默认为20x20
                QRectF annorectF = transRect(rotation, rectF);
                dAnnot->setRectF(annorectF);

//                FS_RECTF newrectf;
//                newrectf.left = static_cast<float>(annorectF.left());
//                newrectf.top = static_cast<float>(actualHeight - annorectF.top());
//                newrectf.right = static_cast<float>(annorectF.right());
//                newrectf.bottom = static_cast<float>(actualHeight - annorectF.bottom());
//                FPDFAnnot_SetRect(annot, &newrectf);
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
                rectF.setY(actualHeight - static_cast<double>(quad.y1));
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
        } else if (DPdfAnnot::ALink == type) {
            DPdfLinkAnnot *dAnnot = new DPdfLinkAnnot;

            FPDF_LINK link = FPDFAnnot_GetLink(annot);

            FPDF_ACTION action = FPDFLink_GetAction(link);

            unsigned long type = FPDFAction_GetType(action);

            //获取位置
            FS_RECTF rectF;
            if (FPDFAnnot_GetRect(annot, &rectF)) {
                QRectF annorectF = transRect(rotation, rectF);
                dAnnot->setRectF(annorectF);
            }

            //获取类型
            if (PDFACTION_URI == type) {
                char uri[256] = {0};
                unsigned long lenth = FPDFAction_GetURIPath(m_doc, action, uri, 256);
                if (0 != lenth) {
                    dAnnot->setUrl(uri);
                }
                dAnnot->setLinkType(DPdfLinkAnnot::Uri);

            } else if (PDFACTION_REMOTEGOTO == type) {
                char filePath[256] = {0};
                unsigned long lenth = FPDFAction_GetFilePath(action, filePath, 256);
                if (0 != lenth) {
                    dAnnot->setFilePath(filePath);
                }

                dAnnot->setLinkType(DPdfLinkAnnot::RemoteGoTo);
            } else if (PDFACTION_GOTO == type || PDFACTION_UNSUPPORTED == type) { //跳转到文档某处
                FPDF_DEST dest = FPDFAction_GetDest(m_doc, action);

                int index = FPDFDest_GetDestPageIndex(m_doc, dest);

                FPDF_BOOL hasX = false;
                FPDF_BOOL hasY = false;
                FPDF_BOOL hasZ = false;
                FS_FLOAT x = 0;
                FS_FLOAT y = 0;
                FS_FLOAT z = 0;

                bool result = FPDFDest_GetLocationInPage(dest, &hasX, &hasY, &hasZ, &x, &y, &z);

                if (result)
                    dAnnot->setPage(index, hasX ? x : 0, hasY ? y : 0);

                dAnnot->setLinkType(DPdfLinkAnnot::Goto);
            }

            m_dAnnots.append(dAnnot);
        } else {
            //其他类型 用于占位 对应索引
            DPdfUnknownAnnot *dAnnot = new DPdfUnknownAnnot;

            m_dAnnots.append(dAnnot);
        }
        FPDFPage_CloseAnnot(annot);
    }

    FPDF_ClosePage(page);

    return true;
}

FS_RECTF DPdfPagePrivate::transRect(const int &rotation, const QRectF &rect)
{
    qreal actualWidth  = (rotation % 2 == 0) ? m_width : m_height;
    qreal actualHeight = (rotation % 2 == 0) ? m_height : m_width;

    FS_RECTF fs_rect;

    if (1 == rotation) {
        fs_rect.left = rect.y();
        fs_rect.top = rect.x() + rect.width();
        fs_rect.right = rect.y() + rect.height();
        fs_rect.bottom = rect.x();
    } else if (2 == rotation) {
        fs_rect.left = actualWidth - rect.x() - rect.width();
        fs_rect.top = rect.y() + rect.height();
        fs_rect.right = actualWidth - rect.x();
        fs_rect.bottom = rect.y();
    } else if (3 == rotation) {
        fs_rect.left = actualHeight - rect.y() - rect.height();
        fs_rect.top = actualWidth - rect.x();
        fs_rect.right = actualHeight - rect.y();
        fs_rect.bottom = actualWidth - rect.x() - rect.width();
    } else {
        fs_rect.left = rect.x();
        fs_rect.top = actualHeight - rect.y();
        fs_rect.right = rect.x() + rect.width();
        fs_rect.bottom = actualHeight - rect.y() - rect.height();
    }

    return fs_rect;
}

QRectF DPdfPagePrivate::transRect(const int &rotation, const FS_RECTF &fs_rect)
{
//    qreal actualwidth  = (rotation % 2 == 0) ? m_width : m_height;
//    qreal actualHeight = (rotation % 2 == 0) ? m_height : m_width;

    //    rotation:0
    //                 actualWidth
    //    |----------------------------------|
    //    |--------right-------              |
    //    |                                  |
    //    |----left----........              |
    //    |            |      .          actualHeight
    //    |            |. . . .              |
    //    |           top     |bottom        |
    //    |            |      |              |
    //   圆心---------------------------------|

    if (1 == rotation) {    //90
        return QRectF(static_cast<qreal>(fs_rect.bottom),
                      static_cast<qreal>(fs_rect.left),
                      static_cast<qreal>(fs_rect.top) - static_cast<qreal>(fs_rect.bottom),
                      static_cast<qreal>(fs_rect.right) - static_cast<qreal>(fs_rect.left));
    } else if (2 == rotation) { //180
        return QRectF(m_width - static_cast<qreal>(fs_rect.right),
                      static_cast<qreal>(fs_rect.bottom),
                      static_cast<qreal>(fs_rect.right) - static_cast<qreal>(fs_rect.left),
                      static_cast<qreal>(fs_rect.top) - static_cast<qreal>(fs_rect.bottom));

    } else if (3 == rotation) { //270
        return QRectF(m_height - static_cast<qreal>(fs_rect.top),
                      m_width - static_cast<qreal>(fs_rect.right),
                      static_cast<qreal>(fs_rect.top) - static_cast<qreal>(fs_rect.bottom),
                      static_cast<qreal>(fs_rect.right) - static_cast<qreal>(fs_rect.left));
    }

    return QRectF(static_cast<qreal>(fs_rect.left),
                  m_height - static_cast<qreal>(fs_rect.top),
                  static_cast<qreal>(fs_rect.right) - static_cast<qreal>(fs_rect.left),
                  static_cast<qreal>(fs_rect.top) - static_cast<qreal>(fs_rect.bottom));
}

DPdfPage::DPdfPage(DPdfDocHandler *handler, int pageIndex)
    : d_ptr(new DPdfPagePrivate(handler, pageIndex))
{

}

DPdfPage::~DPdfPage()
{

}

bool DPdfPage::isValid() const
{
    return d_func()->m_isValid;
}

qreal DPdfPage::width() const
{
    return d_func()->m_width;
}

qreal DPdfPage::height() const
{
    return d_func()->m_height;
}

int DPdfPage::pageIndex() const
{
    return d_func()->m_index;
}

QImage DPdfPage::image(qreal scale)
{
    if (nullptr == d_func()->m_doc)
        return QImage();

    d_func()->loadPage();

    int scaleWidth = static_cast<int>(width() * scale) ;
    int scaleHeight = static_cast<int>(height() * scale);

    QImage image(scaleWidth, scaleHeight, QImage::Format_RGBA8888);

    image.fill(0xFFFFFFFF);

    if (image.isNull())
        return QImage();

    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(image.width(), image.height(), FPDFBitmap_BGRA, image.scanLine(0), image.bytesPerLine());

    if (bitmap == nullptr) {
        return QImage();
    }

    FPDF_RenderPageBitmap(bitmap, d_func()->m_page, 0, 0, scaleWidth, scaleHeight, scaleWidth, scaleHeight, 0, FPDF_ANNOT);

    FPDFBitmap_Destroy(bitmap);

    for (int i = 0; i < image.height(); i++) {
        uchar *pixels = image.scanLine(i);
        for (int j = 0; j < image.width(); j++) {
            qSwap(pixels[0], pixels[2]);
            pixels += 4;
        }
    }

    return image;
}

QImage DPdfPage::image(qreal xscale, qreal yscale, qreal x, qreal y, qreal width, qreal height)
{
    if (nullptr == d_func()->m_doc)
        return QImage();

    d_func()->loadPage();

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

int DPdfPage::countChars()
{
    d_func()->loadTextPage();

    return FPDFText_CountChars(d_func()->m_textPage);
}

QVector<QRectF> DPdfPage::getTextRects(int start, int charCount)
{
    d_func()->loadTextPage();

    QVector<QRectF> result;
    const std::vector<CFX_FloatRect> &pdfiumRects = reinterpret_cast<CPDF_TextPage *>(d_func()->m_textPage)->GetRectArray(start, charCount);
    result.reserve(pdfiumRects.size());
    for (const CFX_FloatRect &rect : pdfiumRects) {
        result.push_back({rect.left, height() - rect.top, rect.right - rect.left, rect.top - rect.bottom});
    }
    return result;
}

bool DPdfPage::getTextRect(int start, QRectF &textrect)
{
    d_func()->loadTextPage();

    if (FPDFText_GetUnicode(d_func()->m_textPage, start) == L' ') {
        textrect = QRectF();
        return true;
    }

    FS_RECTF rect;
    if (FPDFText_GetLooseCharBox(d_func()->m_textPage, start, &rect)) {
        textrect = QRectF(rect.left, height() - rect.top, rect.right - rect.left, rect.top - rect.bottom);
        return true;
    }

    return  false;
}

QString DPdfPage::text(const QRectF &rect)
{
    d_func()->loadTextPage();

    qreal newBottom = height() - rect.bottom();
    qreal newTop = height() - rect.top();
    CFX_FloatRect fxRect(rect.left(), std::min(newBottom, newTop), rect.right(), std::max(newBottom, newTop));
    auto text = reinterpret_cast<CPDF_TextPage *>(d_func()->m_textPage)->GetTextByRect(fxRect);
    return QString::fromWCharArray(text.c_str(), text.GetLength());
}

QString DPdfPage::text(int start, int charCount)
{
    d_func()->loadTextPage();

    auto text = reinterpret_cast<CPDF_TextPage *>(d_func()->m_textPage)->GetPageText(start, charCount);
    return QString::fromWCharArray(text.c_str(), text.GetLength());
}

DPdfAnnot *DPdfPage::createTextAnnot(QPointF point, QString text)
{
    d_func()->loadPage();

    FPDF_ANNOTATION_SUBTYPE subType = FPDF_ANNOT_TEXT;

    FPDF_ANNOTATION annot = FPDFPage_CreateAnnot(d_func()->m_page, subType);

    if (!FPDFAnnot_SetStringValue(annot, "Contents", text.utf16())) {
        FPDFPage_CloseAnnot(annot);
        return nullptr;
    }

    FS_RECTF fs_rect = d_func()->transRect(d_func()->oriRotation(), QRectF(point.x() - 10, point.y() - 10, 20, 20));

    if (!FPDFAnnot_SetRect(annot, &fs_rect)) {
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
    d_func()->loadPage();

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
        FS_RECTF fs_rect = d_func()->transRect(d_func()->oriRotation(), QRectF(point.x() - 10, point.y() - 10, 20, 20));

        if (!FPDFAnnot_SetRect(annot, &fs_rect)) {
            FPDFPage_CloseAnnot(annot);
            return false;
        }

        textAnnot->setRectF(QRectF(point.x() - 10, point.y() - 10, 20, 20));
    }

    FPDFPage_CloseAnnot(annot);

    emit annotUpdated(dAnnot);

    return true;
}

DPdfAnnot *DPdfPage::createHightLightAnnot(const QList<QRectF> &list, QString text, QColor color)
{
    d_func()->loadPage();

    FPDF_ANNOTATION_SUBTYPE subType = FPDF_ANNOT_HIGHLIGHT;

    FPDF_ANNOTATION annot = FPDFPage_CreateAnnot(d_func()->m_page, subType);

    if (color.isValid() && !FPDFAnnot_SetColor(annot, FPDFANNOT_COLORTYPE_Color,
                                               static_cast<unsigned int>(color.red()),
                                               static_cast<unsigned int>(color.green()),
                                               static_cast<unsigned int>(color.blue()),
                                               static_cast<unsigned int>(color.alpha()))) {
        FPDFPage_CloseAnnot(annot);
        return nullptr;
    }

    for (const QRectF &rect : list) {
        FS_QUADPOINTSF quad;
        quad.x1 = static_cast<float>(rect.x());
        quad.y1 = static_cast<float>(d_func()->m_height - rect.y());
        quad.x2 = static_cast<float>(rect.x() + rect.width());
        quad.y2 = static_cast<float>(d_func()->m_height - rect.y());
        quad.x3 = static_cast<float>(rect.x());
        quad.y3 = static_cast<float>(d_func()->m_height - rect.y() - rect.height());
        quad.x4 = static_cast<float>(rect.x() + rect.width());
        quad.y4 = static_cast<float>(d_func()->m_height - rect.y() - rect.height());

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
    d_func()->loadPage();

    DPdfHightLightAnnot *hightLightAnnot = static_cast<DPdfHightLightAnnot *>(dAnnot);

    if (nullptr == hightLightAnnot)
        return false;

    int index = d_func()->m_dAnnots.indexOf(dAnnot);

    FPDF_ANNOTATION annot = FPDFPage_GetAnnot(d_func()->m_page, index);

    if (color.isValid()) {
        if (!FPDFAnnot_SetColor(annot, FPDFANNOT_COLORTYPE_Color,
                                static_cast<unsigned int>(color.red()),
                                static_cast<unsigned int>(color.green()),
                                static_cast<unsigned int>(color.blue()),
                                static_cast<unsigned int>(color.alpha()))) {
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
    d_func()->loadPage();

    int index = d_func()->m_dAnnots.indexOf(dAnnot);

    if (index < 0)
        return false;

    if (!FPDFPage_RemoveAnnot(d_func()->m_page, index))
        return false;

    d_func()->m_dAnnots.removeAll(dAnnot);

    emit annotRemoved(dAnnot);

    delete dAnnot;

    return true;
}

QVector<QRectF> DPdfPage::search(const QString &text, bool matchCase, bool wholeWords)
{
    d_func()->loadTextPage();

    QVector<QRectF> rectfs;
    unsigned long flags = 0x00000000;

    if (matchCase)
        flags |= FPDF_MATCHCASE;

    if (wholeWords)
        flags |= FPDF_MATCHWHOLEWORD;

    FPDF_SCHHANDLE schandle = FPDFText_FindStart(d_func()->m_textPage, text.utf16(), flags, 0);
    if (schandle) {
        while (FPDFText_FindNext(schandle)) {
            int curSchIndex = FPDFText_GetSchResultIndex(schandle);
            if (curSchIndex >= 0) {
                const QVector<QRectF> &textrectfs = getTextRects(curSchIndex, text.length());
                rectfs << textrectfs;
            }
        };
    }

    FPDFText_FindClose(schandle);
    return rectfs;
}

QList<DPdfAnnot *> DPdfPage::annots()
{
    QList<DPdfAnnot *> dannots;

    foreach (DPdfAnnot *dannot, d_func()->m_dAnnots) {
        if (dannot->type() == DPdfAnnot::AText || dannot->type() == DPdfAnnot::AHighlight) {
            dannots.append(dannot);
            continue;
        }
    }

    return dannots;
}

QList<DPdfAnnot *> DPdfPage::links()
{
    QList<DPdfAnnot *> links;

    foreach (DPdfAnnot *annot, d_func()->m_dAnnots) {
        if (annot->type() == DPdfAnnot::ALink) {
            links.append(annot);
            continue;
        }
    }

    return links;
}
