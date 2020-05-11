#include "docummentpdf.h"
#include "pagepdf.h"
#include "../searchtask.h"
#include "pdfControl/docview/publicfunc.h"
//#include "../docummentbase.cpp"
#include <DScrollBar>
#include <QImage>
#include <QTemporaryFile>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QDebug>
#include <QFile>


class DocummentPDFPrivate: public DocummentBasePrivate
{
//    Q_OBJECT
public:
    DocummentPDFPrivate(DocummentPDF *parent): DocummentBasePrivate(parent)
    {
        document = nullptr;
    }

    ~DocummentPDFPrivate() override
    {
        qDeleteAll(m_pages);
        m_pages.clear();
        if (nullptr != document) {
            delete document;
            document = nullptr;
        }
    }

    Poppler::Document *document;

    Q_DECLARE_PUBLIC(DocummentPDF)

protected slots:
    void loadDocumment(QString filepath) override;

private:
    void setBasicInfo(const QString &filepath);

};

void DocummentPDFPrivate::loadDocumment(QString filepath)
{
    Q_Q(DocummentPDF);
    document = Poppler::Document::load(filepath);
    if (nullptr == document || document->numPages() <= 0) {
        emit signal_docummentLoaded(false);
        return;
    }

    document->setRenderHint(Poppler::Document::TextAntialiasing, true);
    document->setRenderHint(Poppler::Document::Antialiasing, true);
    document->setRenderHint(Poppler::Document::ThinLineSolid, true);
    document->setRenderHint(Poppler::Document::ThinLineShape, true);
    m_pages.clear();

    int countlabelnotmatch = 0;
    for (int i = 0; i < document->numPages(); i++) {
        PagePdf *page = new PagePdf(q);
        Poppler::Page *popplerpage = document->page(i);
        page->setPage(popplerpage, i);
        QString strlabel = popplerpage->label();

        if (!strlabel.isEmpty()) {
            m_label2pagenum.insert(strlabel, i);
            m_pagenum2label.insert(i, strlabel);
            if (strlabel.toInt() != i + 1) {
                countlabelnotmatch ++;
            }
        }
        m_pages.append((PageBase *)page);
    }

    if (countlabelnotmatch <= 0) {
        m_label2pagenum.clear();
        m_pagenum2label.clear();
    }
    if (m_pages.size() > 0) {
        m_imagewidth = m_pages.at(0)->getOriginalImageWidth();
        m_imageheight = m_pages.at(0)->getOriginalImageHeight();
        double sz = m_imageheight > m_imagewidth ? m_imageheight : m_imagewidth;
        m_maxzoomratio = MAXPAGEHEIGHT / (sz * qApp->devicePixelRatio());
        m_maxzoomratio = m_maxzoomratio > 5.0 ? 5.0 : m_maxzoomratio;
        m_scale = m_scale > m_maxzoomratio ? m_maxzoomratio : m_scale;
    }

    setBasicInfo(filepath);

    emit signal_docummentLoaded(true);
}


void DocummentPDFPrivate::setBasicInfo(const QString &filepath)
{
    QFileInfo info(filepath);
    m_fileinfo->size = info.size();
    m_fileinfo->CreateTime = info.birthTime();
    m_fileinfo->ChangeTime = info.lastModified();
    m_fileinfo->strAuther = info.owner();
    m_fileinfo->strFilepath = info.filePath();
    if (document) {
        int major = 0, minor = 0;
        document->getPdfVersion(&major, &minor);
        m_fileinfo->strFormat = QString("PDF v.%1.%2").arg(major).arg(minor);
        m_fileinfo->boptimization = document->isLinearized();
        m_fileinfo->strKeyword = document->keywords();
        m_fileinfo->strTheme = document->title();
        m_fileinfo->strProducter = document->producer();
        m_fileinfo->strCreater = document->creator();
        m_fileinfo->bsafe = document->isEncrypted();
        m_fileinfo->iWidth = static_cast<PagePdf *>(m_pages.at(0))->GetPage()->pageSize().width();
        m_fileinfo->iHeight = static_cast<PagePdf *>(m_pages.at(0))->GetPage()->pageSize().height();
        m_fileinfo->iNumpages = document->numPages();
    }
}

DocummentPDF::DocummentPDF(DWidget *parent):
    DocummentBase(new DocummentPDFPrivate(this), parent)
{

}

DocummentPDF::~DocummentPDF()
{
}

bool DocummentPDF::loadDocumment(QString filepath)
{
    emit signal_loadDocumment(filepath);
    return true;
}

void DocummentPDF::jumpToHighLight(const QString &uuid, int ipage)
{
    Q_D(DocummentPDF);
    if (ipage >= 0 && ipage < d->m_pages.size()) {
        Poppler::Page *page = static_cast<PagePdf *>(d->m_pages.at(ipage))->GetPage();
        QList<Poppler::Annotation *> listannote = page->annotations();
        foreach (Poppler::Annotation *annote, listannote) {
            if (annote->subType() == Poppler::Annotation::AHighlight && annote->uniqueName().indexOf(uuid) >= 0) {
                QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();

                if (listquad.size() > 0) {
                    QRectF rectbound;
                    rectbound.setTopLeft(listquad.at(0).points[0]);
                    rectbound.setTopRight(listquad.at(0).points[1]);
                    rectbound.setBottomLeft(listquad.at(0).points[3]);
                    rectbound.setBottomRight(listquad.at(0).points[2]);
                    int xvalue = 0, yvalue = 0;
                    rectbound.setX(rectbound.x()*d->m_imagewidth);
                    rectbound.setY(rectbound.y()*d->m_imageheight);
                    rectbound.setWidth(rectbound.width()*d->m_imagewidth);
                    rectbound.setHeight(rectbound.height()*d->m_imageheight);
                    cacularValueXY(xvalue, yvalue, ipage, false, rectbound);
                    QScrollBar *scrollBar_Y = verticalScrollBar();
                    if (scrollBar_Y)
                        scrollBar_Y->setValue(yvalue);
                    if (xvalue > 0) {
                        QScrollBar *scrollBar_X = horizontalScrollBar();
                        if (scrollBar_X)
                            scrollBar_X->setValue(xvalue);
                    }
                    break;
                }
            }
        }
        qDeleteAll(listannote);
    }
}

void DocummentPDF::changeAnnotationColor(int ipage, const QString uuid, const QColor &color)
{
    Q_D(DocummentPDF);
    if (!d->document || ipage < 0 || ipage > d->m_pages.size())return;
    static_cast<PagePdf *>(d->m_pages.at(ipage))->changeAnnotationColor(uuid, color);
}

void DocummentPDF::removeAllAnnotation()
{
    Q_D(DocummentPDF);
    if (!d->document)return;
    for (int i = 0; i < d->m_pages.size(); ++i) {
        QList<Poppler::Annotation *> listannote = static_cast<PagePdf *>(d->m_pages.at(i))->GetPage()->annotations();
        foreach (Poppler::Annotation *atmp, listannote) {
            static_cast<PagePdf *>(d->m_pages.at(i))->GetPage()->removeAnnotation(atmp);
        }
    }
    scaleAndShow(d->m_scale, d->m_rotate);
}

QString DocummentPDF::removeAnnotation(const QPoint &startpos, AnnoteType_Em type)
{
    Q_D(DocummentPDF);
    QPoint pt = startpos;
    int page = pointInWhichPage(pt);
    if (page < 0)
        return "";
    QString uuid = static_cast<PagePdf *>(d->m_pages.at(page))->removeAnnotation(pt);
    return uuid;
}

void DocummentPDF::removeAnnotation(const QString &struuid, int ipage)
{
    Q_D(DocummentPDF);
    if (ipage < 0 || ipage >= d->m_pages.size()) {
        for (int i = 0; i < d->m_pages.size(); ++i) {
            static_cast<PagePdf *>(d->m_pages.at(i))->removeAnnotation(struuid);
        }
    } else {
        static_cast<PagePdf *>(d->m_pages.at(ipage))->removeAnnotation(struuid);
    }
}

QString DocummentPDF::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    Q_D(DocummentPDF);
    QPoint startpt = startpos;
    QPoint endpt = endpos;

    int startpage = pointInWhichPage(startpt);
    int endpage = pointInWhichPage(endpt);

    QString uuid;
    if (d->m_bScanningcopy) { //是否为扫描件
        QRect rect;
        int iwidth = 0, iheight = 0;
        bool bright = true;
        if (startpt.x() > endpt.x()) {
            bright = false;
            iwidth = startpt.x() - endpt.x();
        } else {
            iwidth = endpt.x() - startpt.x();
        }
        if (startpage < endpage || startpage > endpage) {
            //不在同一页，以起始页为准
            if (startpage > endpage) {
                iheight = startpt.y();
                if (bright)
                    rect = QRect(startpt.x(), 0, iwidth, iheight);
                else
                    rect = QRect(endpt.x(), 0, iwidth, iheight);
            } else {
                iheight = d->m_pages.at(startpage)->height() - startpt.y();
                if (bright)
                    rect = QRect(startpt.x(), startpt.y(), iwidth, iheight);
                else
                    rect = QRect(endpt.x(), startpt.y(), iwidth, iheight);
            }

        } else {
            if (startpt.y() > endpt.y()) {
                iheight = startpt.y() - endpt.y();
                if (bright)
                    rect = QRect(startpt.x(), endpt.y(), iwidth, iheight);
                else
                    rect = QRect(endpt.x(), endpt.y(), iwidth, iheight);
            } else {
                iheight = endpt.y() - startpt.y();
                if (bright)
                    rect = QRect(startpt.x(), startpt.y(), iwidth, iheight);
                else
                    rect = QRect(endpt.x(), startpt.y(), iwidth, iheight);
            }
        }
        uuid = static_cast<PagePdf *>(d->m_pages.at(startpage))->addAnnotation(color, rect);
    } else {
        if (startpage < 0)
            return "";

        if (startpage > endpage) {
            startpage = startpage ^ endpage;
            endpage = startpage ^ endpage;
            startpage = startpage ^ endpage;
        }

        for (int i = startpage; i <= endpage; i++) {
            uuid = static_cast<PagePdf *>(d->m_pages.at(i))->addAnnotation(color);
        }
    }
    return uuid;
}

void DocummentPDF::getAllAnnotation(QList<stHighlightContent> &listres)
{
    Q_D(DocummentPDF);

    for (int i = 0; i < d->m_pages.size(); ++i) {
        QList<Poppler::Annotation *> listannote = static_cast<PagePdf *>(d->m_pages.at(i))->GetPage()->annotations();
        foreach (Poppler::Annotation *annote, listannote) {
            if (annote->subType() == Poppler::Annotation::AHighlight || (annote->subType() == Poppler::Annotation::AText /*&& annote->flags() == static_cast<Poppler::Annotation::Flag>(6)*/)) {
                stHighlightContent stres;
                QString struuid = annote->uniqueName();
                if (struuid.isEmpty()) {
                    struuid = PublicFunc::getUuid();
                    annote->setUniqueName(struuid);
                }
                QString strcontents = annote->contents();
                stres.ipage = i;
                stres.strcontents = strcontents;
                stres.struuid = struuid;
                listres.push_back(stres);
            }
        }
        qDeleteAll(listannote);
    }
}

void DocummentPDF::search(const QString &strtext, QColor color)
{
    Q_D(DocummentPDF);
    clearSearch();
    d->m_searchTask->start(d->m_pages, strtext, false, false, d->m_currentpageno + 1);
}

bool DocummentPDF::save(const QString &filePath)
{
    // Save document to temporary file...
    QTemporaryFile temporaryFile;
    QString strtemfile = temporaryFile.fileTemplate() + QLatin1String(".") + QFileInfo(filePath).suffix();
    temporaryFile.setFileTemplate(strtemfile);
    if (!temporaryFile.open()) {
        return false;
    }

    temporaryFile.close();
    if (!pdfsave(temporaryFile.fileName(), true)) {
        return false;
    }

    // Copy from temporary file to actual file...
    QFile file(filePath);

    if (!temporaryFile.open()) {
        return false;
    }
    if (temporaryFile.size() <= 1)
        return  false;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    if (!PublicFunc::copyFile(temporaryFile, file)) {
        return false;
    }
    return true;
}

bool DocummentPDF::saveas(const QString &filePath, bool withChanges)
{
    Q_D(DocummentPDF);

    return pdfsave(filePath, withChanges);

//    QString strsource = d->m_fileinfo->strFilepath;
//    bool bsuccess = false;
//    if (!strsource.isEmpty()) {
//        if (!withChanges) {
//            if (QFile::copy(strsource, filePath))
//                bsuccess = true;

//        } else {
//            QString strtmp = strsource.left(strsource.lastIndexOf(QChar('/')) + 1);
//            strtmp.append(PublicFunc::getUuid());
//            strtmp.append(".pdf");

//            if (QFile::copy(strsource, strtmp)) {
//                if (pdfsave(strsource, true)) {
//                    if (QFile::copy(strsource, filePath)) {
//                        bsuccess = true;

//                        if (QFile::remove(strsource)) {
//                            if (QFile::rename(strtmp, strsource))
//                                QFile::remove(strtmp);
//                        }
//                    } else {
//                        QFile::remove(strtmp);
//                    }
//                }
//            }
//        }
//    }
//    return  bsuccess;
}

bool DocummentPDF::pdfsave(const QString &filePath, bool withChanges)
{
    Q_D(DocummentPDF);

    QScopedPointer< Poppler::PDFConverter > pdfConverter(d->document->pdfConverter());

    pdfConverter->setOutputFileName(filePath);

    Poppler::PDFConverter::PDFOptions options = pdfConverter->pdfOptions();

    if (withChanges & Poppler::PDFConverter::WithChanges) {
        options |= Poppler::PDFConverter::WithChanges;
    }

    pdfConverter->setPDFOptions(options);

    bool bres = pdfConverter->convert();

    if (!bres) {
        qDebug() << __FUNCTION__ << pdfConverter->lastError();

    }

    return bres;
}

void DocummentPDF::clearSearch()
{
    Q_D(DocummentPDF);
    d->m_searchTask->cancel();
    d->m_searchTask->wait();
    if (d->m_pagecountsearch.size() > 0) {
        foreach (int key, d->m_pagecountsearch.keys()) {
            d->m_pages.at(key)->clearHighlightRects();
            d->m_pages.at(key)->setCurSearchShow(false);
        }
    }

    if (d->m_currentpageno - 1 >= 0)
        d->m_pages.at(d->m_currentpageno - 1)->update();

    if (d->m_currentpageno + 1 < d->m_pages.size())
        d->m_pages.at(d->m_currentpageno + 1)->update();

    d->m_pages.at(d->m_currentpageno)->update();//刷新当前页
    d->m_pagecountsearch.clear();
}

void DocummentPDF::refreshOnePage(int ipage)
{
    Q_D(DocummentPDF);
    if (!d->document)
        return ;
    if (d->m_pages.count() > ipage)
        d->m_pages.at(ipage)->showImage(d->m_scale, d->m_rotate);
}

bool DocummentPDF::bDocummentExist()
{
    Q_D(DocummentPDF);
    if (!d->document) {
        return false;
    }
    return true;
}

bool DocummentPDF::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    Q_D(DocummentPDF);
    QPoint pt(pos);
    int ipage = pointInWhichPage(pt);
    if (ipage < 0)
        return  false;
    return (static_cast<PagePdf *>(d->m_pages.at(ipage)))->annotationClicked(pt, strtext, struuid);
}

void DocummentPDF::title(QString &title)
{
    Q_D(DocummentPDF);
    title = d->document->title();
}
void DocummentPDF::setAnnotationText(int ipage, const QString &struuid, const QString &strtext)
{
    Q_D(DocummentPDF);
    if (ipage >= 0 && ipage < d_ptr->m_pages.size()) {
        Poppler::Page *page = static_cast<PagePdf *>(d_ptr->m_pages.at(ipage))->GetPage();
        QList<Poppler::Annotation *> plistannote = page->annotations();
        foreach (Poppler::Annotation *annote, plistannote) {
            QString uniquename = annote->uniqueName();
            if (!uniquename.isEmpty() && uniquename.indexOf(struuid) >= 0) {
                annote->setContents(strtext);
            }
        }
        qDeleteAll(plistannote);
    }
}

void DocummentPDF::getAnnotationText(const QString &struuid, QString &strtext, int ipage)
{
    Q_D(DocummentPDF);
    if (ipage >= 0 && ipage < d->m_pages.size()) {
        Poppler::Page *page = static_cast<PagePdf *>(d_ptr->m_pages.at(ipage))->GetPage();
        QList<Poppler::Annotation *> plistannote = page->annotations();
        foreach (Poppler::Annotation *annote, plistannote) {
            QString uniquename = annote->uniqueName();
            if (!uniquename.isEmpty() && uniquename.indexOf(struuid) >= 0 && !struuid.isEmpty()) {
                strtext = annote->contents();
                qDeleteAll(plistannote);
                return;
            }
        }
        qDeleteAll(plistannote);
    }
}

bool DocummentPDF::freshFile(QString file)
{
    Q_D(DocummentPDF);
    for (int i = 0; i < d->m_pages.size(); i++) {
        ((PagePdf *)d->m_pages.at(i))->deletePage();
    }
    if (nullptr != d->document) {
        delete d->document;
        d->document = nullptr;
    }
    d->document = Poppler::Document::load(file);

    if (nullptr == d->document || d->document->numPages() <= 0) {
        return false;
    }
    d->document->setRenderHint(Poppler::Document::TextAntialiasing, true);
    d->document->setRenderHint(Poppler::Document::Antialiasing, true);
    d->document->setRenderHint(Poppler::Document::ThinLineSolid, true);
    d->document->setRenderHint(Poppler::Document::ThinLineShape, true);
    for (int i = 0; i < d->m_pages.size(); i++) {
        ((PagePdf *)d->m_pages.at(i))->freshPage(d->document->page(i));
    }
    return true;
}

Outline DocummentPDF::outline()
{
    Q_D(DocummentPDF);

    Outline outline;

    QScopedPointer< QDomDocument > toc(d->document->toc());

    if (toc) {
        outline = loadOutline(*toc, d->document);
    }

    return outline;
}

Outline DocummentPDF::loadOutline(const QDomNode &parent, Poppler::Document *document)
{
    Outline outline;

    const QDomNodeList nodes = parent.childNodes();

    outline.reserve(nodes.size());

    for (int index = 0, count = nodes.size(); index < count; ++index) {
        const QDomNode node = nodes.at(index);
        const QDomElement element = node.toElement();

        outline.push_back(Section());
        Section &section = outline.back();
        section.title = element.tagName();

        QScopedPointer< Poppler::LinkDestination > destination;

        if (element.hasAttribute("Destination")) {
            destination.reset(new Poppler::LinkDestination(element.attribute("Destination")));
        } else if (element.hasAttribute("DestinationName")) {
            destination.reset(document->linkDestination(element.attribute("DestinationName")));
        }

        if (destination) {
            int page = destination->pageNumber();
            qreal left = qQNaN();
            qreal top = qQNaN();

            page = page >= 1 ? page : 1;
            page = page <= document->numPages() ? page : document->numPages();

            //if (destination->isChangeLeft())
            {
                left = destination->left();

                left = left >= 0.0 ? left : 0.0;
                left = left <= 1.0 ? left : 1.0;
            }
            // qDebug() << "======" << destination->top() << destination->left() << destination->bottom() << destination->right() << element.attribute("");
            //if (destination->isChangeTop())
            {
                top = destination->top();

                top = top >= 0.0 ? top : 0.0;
                top = top <= 1.0 ? top : 1.0;
            }

            Page::Link &link = section.link;
            link.page = page;
            link.left = left;
            link.top = top;

            const QString fileName = element.attribute("ExternalFileName");
            // qDebug() << fileName << node.nodeName() << element.tagName();
            if (!fileName.isEmpty()) {
                link.urlOrFileName = fileName;
            }
        }

        if (node.hasChildNodes()) {
            section.children = loadOutline(node, document);
        }
    }

    return outline;
}

QString DocummentPDF::addTextAnnotation(const QPoint &pos, const QColor &color, TextAnnoteType_Em type)
{
    Q_D(DocummentPDF);
    QPoint pt = pos;
    int ipage = pointInWhichPage(pt);
    QString uuid;

    if (ipage < d->m_pages.size() && ipage >= 0) {
        double curwidth, curheight, leftspace, topspace;
        curwidth = curheight = leftspace = topspace = 0.0;
        switch (d->m_rotate) {
        case RotateType_0:
        case RotateType_180: {
            curwidth = d->m_imagewidth * d->m_scale;
            curheight = d->m_imageheight * d->m_scale;
        }
        break;
        case RotateType_90:
        case RotateType_270: {
            curwidth = d->m_imageheight * d->m_scale;
            curheight = d->m_imagewidth * d->m_scale;
        }
        break;
        }

        pt.setX(pt.x() - d->m_pages.at(ipage)->x() - (d->m_pages.at(ipage)->width() - curwidth));
        pt.setY(pt.y() - d->m_pages.at(ipage)->y() - (d->m_pages.at(ipage)->height() - curheight));

        int x, y;
        int space = ICONANNOTE_WIDTH * d->m_scale;
        if (pt.x() > ICONANNOTE_WIDTH / 2) {
            x = (pt.x() + space / 2) > curwidth ? curwidth - space / 2 : pt.x();
        } else {
            x = (pt.x() < space / 2) ? space / 2 : pt.x();
        }
        pt.setX(x);
        if (pt.y() > space / 2) {
            y = (pt.y() + space / 2) > curheight ? curheight - space / 2 : pt.y();
        } else {
            y = (pt.y() < space / 2) ? space / 2 : pt.y();
        }
        pt.setY(y);
        pt = transformPoint(pt, d->m_rotate, d->m_scale);
        uuid = static_cast<PagePdf *>(d->m_pages.at(ipage))->addTextAnnotation(pt, color, type);
    }
    return  uuid;
}

bool DocummentPDF::iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    Q_D(DocummentPDF);
    QPoint pt(pos);
    bool bclicked = false;
    int ipage = pointInWhichPage(pt);
    if (ipage >= 0 && ipage < d->m_pages.size())
        bclicked = (static_cast<PagePdf *>(d->m_pages.at(ipage)))->iconAnnotationClicked(pt, strtext, struuid);
    return bclicked;
}



