// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PDFModel.h"
#include "dpdfannot.h"
#include "dpdfpage.h"
#include "dpdfdoc.h"
#include "Application.h"
#include "ddlog.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QScreen>
#include <QThread>

#include <functional>

#define LOCK_DOCUMENT QMutexLocker docMutexLocker(m_docMutex);

// libdeepin-pdfium 导出的 PDFium C 接口（官方头文件未声明这些 C 符号，但已在
// libdeepin-pdfium.so 1.5.8 上核实导出，PDFium C API 长期保持稳定）。
// 仅用于图片对象包围盒探测：夜间模式对象蒙版需要页面上 IMAGE 对象的位置信息，
// 而 Qt 级封装在 1.5.x 上未提供该能力。
extern "C" {
typedef void *FPDF_DOCUMENT;
typedef void *FPDF_PAGE;
typedef void *FPDF_PAGEOBJECT;

FPDF_DOCUMENT FPDF_LoadDocument(const char *file_path, const char *password);
FPDF_PAGE FPDF_LoadPage(FPDF_DOCUMENT document, int page_index);
void FPDF_ClosePage(FPDF_PAGE page);
void FPDF_CloseDocument(FPDF_DOCUMENT document);
int FPDFPage_CountObjects(FPDF_PAGE page);
FPDF_PAGEOBJECT FPDFPage_GetObject(FPDF_PAGE page, int index);
int FPDFPageObj_GetType(FPDF_PAGEOBJECT page_object);   // FPDF_PAGEOBJ_IMAGE == 3
int FPDFPageObj_GetBounds(FPDF_PAGEOBJECT page_object, float *left, float *bottom, float *right, float *top);
int FPDFPage_GetRotation(FPDF_PAGE page);
float FPDF_GetPageWidthF(FPDF_PAGE page);
float FPDF_GetPageHeightF(FPDF_PAGE page);
}

namespace {

// 系统 libdeepin-pdfium (1.5.x) 未提供 /ID 接口，这里直接解析 PDF 文件中的文档
// 标识符：trailer 字典或交叉引用流字典的 /ID[值1 值2]，按规范值1 为永久 ID
// （增量保存、移动/重命名均不变）。两个值各自支持十六进制 <...> 与字面量
// (...) 两种语法。增量更新会把最新 trailer 追加到文件末尾，故扫描文件尾部
// 并取最后一次出现。解析失败（加密/损坏/非常规布局）返回空串，调用方回退
// 到内容指纹匹配。
QString extractPdfFileIdentifier(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return QString();

    file.seek(qMax<qint64>(0, file.size() - 8192));
    const QString text = QString::fromLatin1(file.read(8192));

    // /ID 数组：[值1 值2]，值可为 <hex> 或 (literal)；字面量内允许 \x 转义
    static const QRegularExpression re(
            QLatin1String("/ID\\s*\\[\\s*"
                          "(?:<([0-9a-fA-F]+)>|\\(((?:\\\\.|[^\\\\)])*)\\))"
                          "\\s*"
                          "(?:<[0-9a-fA-F]+>|\\((?:\\\\.|[^\\\\)])*\\))"
                          "\\s*\\]"));
    QString id;
    QRegularExpressionMatchIterator it = re.globalMatch(text);
    while (it.hasNext()) {                       // 取最后一次出现（最新增量更新的 trailer）
        const QRegularExpressionMatch m = it.next();
        // 值1：十六进制时归一化小写；字面量时保持原始字节（同一文件解析结果稳定）
        const QString hex = m.captured(1);
        id = hex.isNull() ? m.captured(2) : hex.toLower();
    }
    return id;
}

} // namespace

namespace deepin_reader {
PDFAnnotation::PDFAnnotation(DPdfAnnot *dannotation) : Annotation(),
    m_dannotation(dannotation)
{
    // qCDebug(appLog) << "Creating PDF annotation with type:" << (dannotation ? dannotation->type() : -1);
}

PDFAnnotation::~PDFAnnotation()
{
    // qCDebug(appLog) << "Destroying PDF annotation";

    m_dannotation = nullptr;
}

QList<QRectF> PDFAnnotation::boundary() const
{
    qCDebug(appLog) << "PDFAnnotation::boundary() - Getting annotation boundary";
    QList<QRectF> rectFList;

    if (m_dannotation->type() == DPdfAnnot::AText || m_dannotation->type() == DPdfAnnot::AHighlight) {
        qCDebug(appLog) << "PDFAnnotation::boundary() - Processing text or highlight annotation";
        foreach (QRectF rect, m_dannotation->boundaries())
            rectFList.append(rect);
    }

    qCDebug(appLog) << "PDFAnnotation::boundary() - Returning" << rectFList.size() << "boundaries";
    return rectFList;
}

QString PDFAnnotation::contents() const
{
    qCDebug(appLog) << "PDFAnnotation::contents() - Getting annotation contents";
    if (nullptr == m_dannotation) {
        qCWarning(appLog) << "Attempt to get contents from null annotation";
        return QString();
    }

    QString text = m_dannotation->text();
    qCDebug(appLog) << "PDFAnnotation::contents() - Returning contents:" << text;
    return text;
}

int PDFAnnotation::type()
{
    qCDebug(appLog) << "PDFAnnotation::type() - Getting annotation type";
    if (nullptr == m_dannotation) {
        qCWarning(appLog) << "Attempt to get type from null annotation";
        return -1;
    }

    int type = m_dannotation->type();
    qCDebug(appLog) << "PDFAnnotation::type() - Returning type:" << type;
    return type;
}

DPdfAnnot *PDFAnnotation::ownAnnotation()
{
    // qCDebug(appLog) << "PDFAnnotation::ownAnnotation() - Getting own annotation pointer";
    return m_dannotation;
}

PDFPage::PDFPage(QMutex *mutex, DPdfPage *page) :
    m_docMutex(mutex), m_page(page)
{
    // qCInfo(appLog) << "Creating PDF page handler";
}

PDFPage::~PDFPage()
{
    // qCInfo(appLog) << "Destroying PDF page handler";
}

QSizeF PDFPage::sizeF() const
{
    // qCDebug(appLog) << "PDFPage::sizeF() - Getting page size";
    QSizeF size = m_page->sizeF();
    // qCDebug(appLog) << "PDFPage::sizeF() - Returning size:" << size;
    return size;
}

QImage PDFPage::render(int width, int height, const QRect &slice) const
{
    // qCInfo(appLog) << "Rendering PDF page with size:" << width << "x" << height << "slice:" << slice;

    LOCK_DOCUMENT

    QRect ratioRect = slice.isValid() ? QRect(slice.x(), slice.y(), slice.width(), slice.height()) : QRect();

    QImage result = m_page->image(width, height, ratioRect);
    // qCDebug(appLog) << "PDFPage::render() - Render completed";
    return result;
}

Link PDFPage::getLinkAtPoint(const QPointF &pos)
{
    // qCDebug(appLog) << "PDFPage::getLinkAtPoint() - Getting link at point:" << pos;
    Link link;
    const QList<DPdfAnnot *> &dlinkAnnots = m_page->links();
    if (dlinkAnnots.size() > 0) {
        // qCDebug(appLog) << "PDFPage::getLinkAtPoint() - Found" << dlinkAnnots.size() << "link annotations";
        for (DPdfAnnot *annot : dlinkAnnots) {
            DPdfLinkAnnot *linkAnnot = dynamic_cast<DPdfLinkAnnot *>(annot);
            if (linkAnnot && linkAnnot->pointIn(pos)) {
                // qCDebug(appLog) << "PDFPage::getLinkAtPoint() - Found matching link annotation";
                if (!linkAnnot->isValid())
                    m_page->initAnnot(annot);

                link.page = linkAnnot->pageIndex() + 1;
                if (linkAnnot->url().isEmpty()) {
                    link.urlOrFileName = linkAnnot->filePath();
                } else {
                    // 删除邮件或本地文件的前缀"http://"
                    link.urlOrFileName = (linkAnnot->url().startsWith(QLatin1String("http://mailto:"))
                                          || linkAnnot->url().startsWith(QLatin1String("http://file://")))
                                         ? linkAnnot->url().mid(7) : linkAnnot->url();
                }
                link.left = linkAnnot->offset().x();
                link.top = linkAnnot->offset().y();
                return link;
            }
        }
    }
    // qCDebug(appLog) << "PDFPage::getLinkAtPoint() - No matching link found";
    return link;
}

bool PDFPage::hasWidgetAnnots() const
{
    // qCDebug(appLog) << "Checking if page has widget annotations";
    return m_page->widgets().count() > 0;
}

QString PDFPage::text(const QRectF &rect) const
{
    // qCDebug(appLog) << "PDFPage::text() - Getting text from rect:" << rect;
    return m_page->text(rect).simplified();
}

QList<Word> PDFPage::words()
{
    qCInfo(appLog) << "Extracting text words from PDF page";

    LOCK_DOCUMENT

    if (m_wordLoaded) {
        qCDebug(appLog) << "PDFPage::words() - Words already loaded, returning cached words";
        return m_words;
    }

    int charCount = 0;

    QStringList texts;

    QVector<QRectF> rects;

    m_page->allTextLooseRects(charCount, texts, rects);

    m_wordLoaded = true;

    if (charCount <= 0) {
        qCDebug(appLog) << "PDFPage::words() - No characters found, returning empty word list";
        return m_words;
    }

    int index = 0;

    qreal maxY = rects[0].y();

    qreal maxHeight = rects[0].height();

    QList<Word> rowWords;

    while (1) {
        if (index == charCount) {
            // qCDebug(appLog) << "PDFPage::words() - Processed all characters, appending final row";
            m_words.append(rowWords);
            break;
        }

        Word word;
        word.text = texts[index];
        word.boundingBox = rects[index];

        //不进行优化
//        m_words.append(word);
//        ++index;
//        continue;

        //如果重叠的部分只在三分之一，视为换行
        if ((word.boundingBox.y() > maxY + maxHeight - (word.boundingBox.height() / 3)) ||
                (word.boundingBox.y() + word.boundingBox.height() < maxY  + (word.boundingBox.height() / 3))) {

            //最本行所有文字 补齐高和低不符合的 填满宽度
            for (int i = 0; i < rowWords.count(); ++i) {
                rowWords[i].boundingBox.setY(maxY);
                rowWords[i].boundingBox.setHeight(maxHeight);

                //如果是空格类型 且没有宽度
                if (!rowWords[i].text.isEmpty() && rowWords[i].text.trimmed().isEmpty() && rowWords[i].boundingBox.width() < 0.0001) {
                    continue;
                    //                if (i - 1 >= 0 && i + 1 < rowWords.count()) {
                    //                    if (rowWords[i + 1].boundingBox.x() - (rowWords[i - 1].boundingBox.x() + rowWords[i - 1].boundingBox.width()) < 10) {
                    //                        rowWords[i].boundingBox.setY(maxY);
                    //                        rowWords[i].boundingBox.setX(rowWords[i - 1].boundingBox.x() + rowWords[i - 1].boundingBox.width());
                    //                        rowWords[i].boundingBox.setWidth(rowWords[i + 1].boundingBox.x() - rowWords[i - 1].boundingBox.x() - rowWords[i - 1].boundingBox.width());
                    //                    }
                    //                }
                }

                //找出本行下一个非空的索引
                int nextIndex = i + 1;
                while (nextIndex < rowWords.count()) {
                    if (!rowWords[nextIndex].text.trimmed().isEmpty() || rowWords[nextIndex].boundingBox.width() > 0.0001)
                        break;
                    nextIndex++;
                }

                //将本文字宽度拉长到下一个字/如果和下一个x重合，则稍微减少点自己宽度
                if (((nextIndex) < rowWords.count()) && !rowWords[nextIndex].text.trimmed().isEmpty() &&
                        (((rowWords[i].boundingBox.x() + rowWords[i].boundingBox.width() < rowWords[nextIndex].boundingBox.x()) &&
                          (rowWords[nextIndex].boundingBox.x() - (rowWords[i].boundingBox.x() + rowWords[i].boundingBox.width()) < 10)) ||
                         ((rowWords[i].boundingBox.x() + rowWords[i].boundingBox.width() > rowWords[nextIndex].boundingBox.x())))
                   ) {
                    rowWords[i].boundingBox.setWidth(rowWords[nextIndex].boundingBox.x() - rowWords[i].boundingBox.x());
                }
            }

            m_words.append(rowWords);
            rowWords.clear();
            maxY = word.boundingBox.y();
            maxHeight = word.boundingBox.height();
        }

        //如果上一个是空格 这一个还是空 则只是追加
//        if (!rowWords.isEmpty() && rowWords[rowWords.count() - 1].text.trimmed().isEmpty() && word.text.trimmed().isEmpty()) {
//            rowWords[rowWords.count() - 1].text.append(word.text);
//            ++index;
//            continue;
//        }

        //先取出最高和最低
        if (word.boundingBox.y() < maxY)
            maxY = word.boundingBox.y();

        if (word.boundingBox.height() + (word.boundingBox.y() - maxY) > maxHeight)
            maxHeight = word.boundingBox.height() + (word.boundingBox.y() - maxY);

        rowWords.append(word);

        ++index;
    }

//    for (int i = 0; i < charCount; i++) {
//        if (texts.count() <= i || rects.count() <= i)
//            break;

//        Word word;
//        word.text = texts[i];
//        QRectF rectf = rects[i];

//        //换行
//        if (rectf.top() > lastOffset.y() || lastOffset.y() > rectf.bottom())
//            lastOffset = QPointF(rectf.x(), rectf.center().y());

//        if (rectf.width() > 0) {
//            word.boundingBox = QRectF(lastOffset.x(), rectf.y(), rectf.width() + rectf.x() - lastOffset.x(), rectf.height());

//            lastOffset = QPointF(word.boundingBox.right(), word.boundingBox.center().y());

//            int curWordsSize = m_words.size();
//            if (curWordsSize > 2
//                    && static_cast<int>(m_words.at(curWordsSize - 1).wordBoundingRect().width()) == 0
//                    && m_words.at(curWordsSize - 2).wordBoundingRect().width() > 0
//                    && word.boundingBox.center().y() <= m_words.at(curWordsSize - 2).wordBoundingRect().bottom()
//                    && word.boundingBox.center().y() >= m_words.at(curWordsSize - 2).wordBoundingRect().top()) {
//                QRectF prevBoundRectF = m_words.at(curWordsSize - 2).wordBoundingRect();
//                m_words[curWordsSize - 1].boundingBox = QRectF(prevBoundRectF.right(),
//                                                               prevBoundRectF.y(),
//                                                               word.boundingBox.x() - prevBoundRectF.right(),
//                                                               prevBoundRectF.height());
//            }
//        }

//        m_words.append(word);
//    }

    qCDebug(appLog) << "PDFPage::words() - Word extraction completed, total words:" << m_words.size();
    return m_words;
}

QVector<PageSection> PDFPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    qCInfo(appLog) << "Searching PDF page for text:" << text << "matchCase:" << matchCase << "wholeWords:" << wholeWords;

    LOCK_DOCUMENT

    return m_page->search(text, matchCase, wholeWords);
}

QList< Annotation * > PDFPage::annotations() const
{
    qCDebug(appLog) << "Retrieving PDF page annotations";

    QList< Annotation * > annotations;

    const QList<DPdfAnnot *> &annos = m_page->annots();

    foreach (DPdfAnnot *dannotation, annos) {
        annotations.append(new PDFAnnotation(dannotation));
    }

    // qCDebug(appLog) << "PDFPage::annotations() - Found" << annotations.size() << "annotations";
    return annotations;
}

Annotation *PDFPage::addHighlightAnnotation(const QList<QRectF> &boundaries, const QString &text, const QColor &color)
{
    qCInfo(appLog) << "Adding highlight annotation with boundaries:" << boundaries << "text:" << text << "color:" << color;

    return new PDFAnnotation(m_page->createHightLightAnnot(boundaries, text, color));
}

bool PDFPage::removeAnnotation(Annotation *annotation)
{
    qCInfo(appLog) << "Removing PDF annotation";

    PDFAnnotation *PDFAnnotation = static_cast< class PDFAnnotation * >(annotation);

    if (PDFAnnotation == nullptr) {
        qCWarning(appLog) << "Attempt to remove null annotation";
        return false;
    }

    m_page->removeAnnot(PDFAnnotation->m_dannotation);

    PDFAnnotation->m_dannotation = nullptr;

    delete PDFAnnotation;

    qCDebug(appLog) << "PDFPage::removeAnnotation() - Successfully removed annotation";
    return true;
}

bool PDFPage::updateAnnotation(Annotation *annotation, const QString &text, const QColor &color)
{
    qCInfo(appLog) << "Updating PDF annotation with text:" << text << "color:" << color;

    if (nullptr == annotation) {
        qCWarning(appLog) << "Attempt to update null annotation";
        return false;
    }

    if (m_page->annots().contains(annotation->ownAnnotation())) {
        qCDebug(appLog) << "PDFPage::updateAnnotation() - Found annotation, updating";
        if (annotation->type() == DPdfAnnot::AText) {
            qCDebug(appLog) << "PDFPage::updateAnnotation() - Updating text annotation";
            m_page->updateTextAnnot(annotation->ownAnnotation(), text);
        } else {
            qCDebug(appLog) << "PDFPage::updateAnnotation() - Updating highlight annotation";
            m_page->updateHightLightAnnot(annotation->ownAnnotation(), color, text);
        }
        return true;
    }

    qCDebug(appLog) << "PDFPage::updateAnnotation() - Failed, annotation not found";
    return false;
}

Annotation *PDFPage::addIconAnnotation(const QRectF &rect, const QString &text)
{
    qCInfo(appLog) << "Adding icon annotation at rect:" << rect << "with text:" << text;

    if (nullptr == m_page) {
        qCCritical(appLog) << "Attempt to add icon annotation to null page";
        return nullptr;
    }

    return new PDFAnnotation(m_page->createTextAnnot(rect.center(), text));
}

Annotation *PDFPage::moveIconAnnotation(Annotation *annot, const QRectF &rect)
{
    qCInfo(appLog) << "Moving icon annotation to rect:" << rect;

    if (nullptr == m_page || nullptr == annot) {
        qCCritical(appLog) << "Attempt to move icon annotation with null page or annotation";
        return nullptr;
    }

    if (annot->ownAnnotation()) {
        qCDebug(appLog) << "PDFPage::moveIconAnnotation() - Moving annotation";
        m_page->updateTextAnnot(annot->ownAnnotation(), annot->ownAnnotation()->text(), rect.center());

        return annot;
    }
    qCDebug(appLog) << "PDFPage::moveIconAnnotation() - Failed, no annotation data";
    return nullptr;
}

PDFDocument::PDFDocument(DPdfDoc *document, const QString &password) :
    m_document(document)
    , m_password(password)
{
    qCInfo(appLog) << "Creating PDF document handler with page count:" << (document ? document->pageCount() : 0);

    m_docMutex = new QMutex;

    QScreen *srn = QApplication::screens().value(0);
    if (nullptr != srn) {
        qCDebug(appLog) << "PDFDocument::PDFDocument() - Screen DPI set, x:" << m_xRes << "y:" << m_yRes;
        m_xRes = srn->logicalDotsPerInchX(); // 获取屏幕的横纵向逻辑dpi
        m_yRes = srn->logicalDotsPerInchY();
    }
    qCDebug(appLog) << "PDFDocument::PDFDocument() - Constructor completed";
}

PDFDocument::~PDFDocument()
{
    qCInfo(appLog) << "Destroying PDF document handler";

    //需要确保pages先被析构完成
    qCDebug(appLog) << "Releasing PDF document resources...";

    m_docMutex->lock();

    delete m_document;

    m_document = nullptr;

    if (m_probeDoc) {
        FPDF_CloseDocument(m_probeDoc);
        m_probeDoc = nullptr;
    }

    m_docMutex->unlock();

    delete m_docMutex;
    qCDebug(appLog) << "PDF document resources released";
}

QVector<QRectF> PDFDocument::imageObjectRects(int index, int width, int height) const
{
    QVector<QRectF> rects;

    if (!m_document || width <= 0 || height <= 0)
        return rects;

    QMutexLocker locker(m_docMutex);

    // 探测句柄懒加载：独立于 m_document 的第二个 FPDF 文档实例（库未暴露内部
    // 句柄），整个文档生命周期内复用；加载失败记录标记避免反复尝试
    if (!m_probeDoc && !m_probeFailed) {
        const QByteArray pwdBytes = m_password.toUtf8();
        m_probeDoc = FPDF_LoadDocument(m_document->filePath().toUtf8().constData(),
                                       pwdBytes.isEmpty() ? nullptr : pwdBytes.constData());
        if (nullptr == m_probeDoc) {
            m_probeFailed = true;
            qCWarning(appLog) << "Probe document load failed, image object detection disabled";
        }
    }

    if (nullptr == m_probeDoc)
        return rects;

    // 与老的内嵌实现一致：每次 FPDF_LoadPage 解析内容，取完立即关闭，
    // 避免与渲染线程的页面句柄生命周期冲突
    FPDF_PAGE probePage = FPDF_LoadPage(m_probeDoc, index);

    if (nullptr == probePage)
        return rects;

    const int rotation = FPDFPage_GetRotation(probePage);
    const float rotatedW = FPDF_GetPageWidthF(probePage);
    const float rotatedH = FPDF_GetPageHeightF(probePage);
    // FPDF_GetPageWidthF/HeightF 为应用旋转后的尺寸，transRect 需要
    // 未旋转的页面点尺寸
    const qreal width_pt = (rotation % 2 == 1) ? rotatedH : rotatedW;
    const qreal height_pt = (rotation % 2 == 1) ? rotatedW : rotatedH;

    // 目标像素尺寸对应的缩放（与 image(index,width,height) 整页渲染对齐）：
    // 视图坐标为页面点尺寸（transRect 输出），整页线性映射到目标像素，
    // 即 rect × width/width_pt。注意不可再叠加 dpi 因子——老实现中
    // transPointToPixel(×dpi/72) 与 sizeF(×dpi/72) 上下抵消，净效果
    // 就是点尺寸直接线性缩放；若分母单独保留 dpi 会导致矩形缩小偏移
    const qreal sx = qreal(width) / width_pt;
    const qreal sy = qreal(height) / height_pt;

    const int count = FPDFPage_CountObjects(probePage);

    for (int i = 0; i < count; ++i) {
        FPDF_PAGEOBJECT obj = FPDFPage_GetObject(probePage, i);

        if (nullptr == obj || 3 != FPDFPageObj_GetType(obj))   // 3 = FPDF_PAGEOBJ_IMAGE
            continue;

        float left = 0, bottom = 0, right = 0, top = 0;

        if (!FPDFPageObj_GetBounds(obj, &left, &bottom, &right, &top))
            continue;

        // 坐标链:PDF 用户空间(y 向上)→ 旋转后视图坐标(y 向下)→ 目标像素，
        // 映射逻辑与老的内嵌实现逐行对齐（transRect(rotation, FS_RECTF)）
        QRectF viewPt;
        switch (rotation) {
        case 1:
            viewPt = QRectF(qreal(bottom), qreal(left),
                            qreal(top) - qreal(bottom), qreal(right) - qreal(left));
            break;
        case 2:
            viewPt = QRectF(width_pt - qreal(right), qreal(bottom),
                            qreal(right) - qreal(left), qreal(top) - qreal(bottom));
            break;
        case 3:
            viewPt = QRectF(height_pt - qreal(top), width_pt - qreal(right),
                            qreal(top) - qreal(bottom), qreal(right) - qreal(left));
            break;
        default:
            viewPt = QRectF(qreal(left), height_pt - qreal(top),
                            qreal(right) - qreal(left), qreal(top) - qreal(bottom));
            break;
        }

        rects.append(QRectF(viewPt.x() * sx, viewPt.y() * sy,
                            viewPt.width() * sx, viewPt.height() * sy).normalized());
    }

    FPDF_ClosePage(probePage);

    return rects;
}

int PDFDocument::pageCount() const
{
    qCDebug(appLog) << "Retrieving PDF document page count";

    return m_document->pageCount();
}

Page *PDFDocument::page(int index) const
{
    qCInfo(appLog) << "Retrieving PDF page at index:" << index;

    if (DPdfPage *page = m_document->page(index, m_xRes, m_yRes)) {
        qCDebug(appLog) << "PDFDocument::page() - Page object created";
        if (page->isValid()) {
            qCDebug(appLog) << "PDF page at index" << index << "is valid";
            return new PDFPage(m_docMutex, page);
        }
        qCWarning(appLog) << "PDF page at index" << index << "is invalid";
    }

    qCDebug(appLog) << "PDFDocument::page() - Returning null page";
    return nullptr;
}

QString PDFDocument::label(int index) const
{
    qCDebug(appLog) << "Retrieving label for PDF page at index:" << index;

    return m_document->label(index);
}

QStringList PDFDocument::saveFilter() const
{
    qCDebug(appLog) << "Retrieving PDF save filters";

    return QStringList() << "Pdf Files (*.pdf)";
}

bool PDFDocument::save() const
{
    qCInfo(appLog) << "Saving PDF document";

    return m_document->save();
}

QString PDFDocument::fileIdentifier() const
{
    if (!m_document)
        return QString();
    // 系统 libdeepin-pdfium (1.5.x) 未提供 /ID 接口，改为在 reader 侧直接解析
    // PDF 文件中的文档标识符（见 extractPdfFileIdentifier）
    return extractPdfFileIdentifier(m_document->filePath());
}

bool PDFDocument::saveAs(const QString &filePath) const
{
    qCInfo(appLog) << "Saving PDF document as:" << filePath;

    return m_document->saveAs(filePath);
}

void collectOuleLine(const DPdfDoc::Outline &cOutline, Outline &outline)
{
    qCDebug(appLog) << "collectOuleLine() - Processing outline with" << cOutline.size() << "sections";
    for (const DPdfDoc::Section &cSection : cOutline) {
        Section setction;
        setction.nIndex = cSection.nIndex;
        setction.title = cSection.title;
        setction.offsetPointF = cSection.offsetPointF;
        if (cSection.children.size() > 0) {
            // qCDebug(appLog) << "collectOuleLine() - Processing" << cSection.children.size() << "child sections";
            collectOuleLine(cSection.children, setction.children);
        }
        outline << setction;
    }
    qCDebug(appLog) << "collectOuleLine() - Completed processing outline";
}

Outline PDFDocument::outline() const
{
    qCInfo(appLog) << "Retrieving PDF document outline";

    if (m_outline.size() > 0) {
        qCDebug(appLog) << "Returning cached PDF outline";
        return m_outline;
    }

    const DPdfDoc::Outline &cOutline = m_document->outline(m_xRes, m_yRes);

    collectOuleLine(cOutline, m_outline);

    qCDebug(appLog) << "PDFDocument::outline() - Outline built with" << m_outline.size() << "sections";
    return m_outline;
}

Properties PDFDocument::properties() const
{
    qCInfo(appLog) << "Retrieving PDF document properties";

    if (m_fileProperties.size() > 0) {
        qCDebug(appLog) << "Returning cached PDF properties";
        return m_fileProperties;
    }

    m_fileProperties = m_document->proeries();

    qCDebug(appLog) << "PDFDocument::properties() - Properties loaded, count:" << m_fileProperties.size();
    return m_fileProperties;
}

PDFDocument *PDFDocument::loadDocument(const QString &filePath, const QString &password, deepin_reader::Document::Error &error)
{
    qCInfo(appLog) << "Loading PDF document from:" << filePath << "with password:" << (!password.isEmpty() ? "[provided]" : "[not provided]");

    DPdfDoc *document = new DPdfDoc(filePath, password);

    if (document->status() == DPdfDoc::SUCCESS) {
        qCInfo(appLog) << "PDF document loaded successfully";
        error = Document::NoError;
        return new PDFDocument(document, password);
    } else if (document->status() == DPdfDoc::PASSWORD_ERROR) {
        qCDebug(appLog) << "PDFDocument::loadDocument() - Document status: PASSWORD_ERROR";
        if (password.isEmpty()) {
            qCWarning(appLog) << "PDF document requires password but none provided";
            error = Document::NeedPassword;
        } else {
            qCWarning(appLog) << "Incorrect password provided for PDF document";
            error = Document::WrongPassword;
        }
    } else {
        qCCritical(appLog) << "Failed to load PDF document, error code:" << document->status();
        error = Document::FileError;
    }

    delete document;
    qCDebug(appLog) << "PDFDocument::loadDocument() - Returning null document";
    return nullptr;
}
}

