#include "docummentpdf.h"
#include "pagepdf.h"
#include "../searchtask.h"
#include "docview/publicfunc.h"
#include <DScrollBar>
#include <QImage>
#include <QTemporaryFile>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QDebug>

void DocummentPDFPrivate::loadDocumment(QString filepath)
{
    Q_Q(DocummentPDF);
    document = Poppler::Document::load(filepath);
    if (nullptr == document || document->numPages() <= 0)
        return;
    document->setRenderHint(Poppler::Document::TextAntialiasing, true);
    document->setRenderHint(Poppler::Document::Antialiasing, true);
    m_pages.clear();
    for (int i = 0; i < document->numPages(); i++) {
        PagePdf *page = new PagePdf(q);
        page->setPage(document->page(i), i);
        m_pages.append((PageBase *)page);
    }
    setBasicInfo(filepath);

    emit signal_docummentLoaded();

}


void DocummentPDFPrivate::setBasicInfo(const QString &filepath)
{
    QFileInfo info(filepath);
    m_fileinfo.size = info.size();
    m_fileinfo.CreateTime = info.birthTime();
    m_fileinfo.ChangeTime = info.lastModified();
    m_fileinfo.strAuther = info.owner();
    m_fileinfo.strFilepath = info.filePath();
    if (document) {
        int major, minor;
        document->getPdfVersion(&major, &minor);
        m_fileinfo.strFormat = QString("PDF v.%1.%2").arg(major).arg(minor);
        m_fileinfo.boptimization = document->isLinearized();
        m_fileinfo.strKeyword = document->keywords();
        m_fileinfo.strTheme = document->title();
        m_fileinfo.strProducter = document->producer();
        m_fileinfo.strCreater = document->creator();
        m_fileinfo.bsafe = document->isEncrypted();
        m_fileinfo.iWidth = static_cast<PagePdf *>(m_pages.at(0))->GetPage()->pageSize().width();
        m_fileinfo.iHeight = static_cast<PagePdf *>(m_pages.at(0))->GetPage()->pageSize().height();
        m_fileinfo.iNumpages = document->numPages();
    }
}

DocummentPDF::DocummentPDF(DWidget *parent):
    DocummentBase(new DocummentPDFPrivate(this), parent)
{
    Q_D(DocummentPDF);
    connect(this, SIGNAL(signal_loadDocumment(QString)), d, SLOT(loadDocumment(QString)));
}

DocummentPDF::~DocummentPDF()
{
    Q_D(DocummentPDF);
    delete d->document;
    d->document = nullptr;
}

bool DocummentPDF::loadDocumment(QString filepath)
{
    emit signal_loadDocumment(filepath);
    return true;
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

QString DocummentPDF::removeAnnotation(const QPoint &startpos)
{
    Q_D(DocummentPDF);
    //暂时只处理未旋转
    QPoint pt = startpos;
    int page = pointInWhichPage(pt);
    if (page < 0) return "";
    return static_cast<PagePdf *>(d->m_pages.at(page))->removeAnnotation(pt);
}

void DocummentPDF::removeAnnotation(const QString &struuid)
{
    Q_D(DocummentPDF);
    return static_cast<PagePdf *>(d->m_pages.at(currentPageNo()))->removeAnnotation(struuid);
}

QString DocummentPDF::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    Q_D(DocummentPDF);
    QPoint pt = startpos;
    int page = pointInWhichPage(pt);
    if (page < 0) return "";
    return static_cast<PagePdf *>(d->m_pages.at(page))->addAnnotation(pt);
}

void DocummentPDF::search(const QString &strtext, QColor color)
{
    Q_D(DocummentPDF);
    clearSearch();
    m_searchTask->start(d->m_pages, strtext, false, false, d->m_currentpageno + 1);
}

bool DocummentPDF::save(const QString &filePath, bool withChanges)
{
    // Save document to temporary file...
    QTemporaryFile temporaryFile;
    temporaryFile.setFileTemplate(temporaryFile.fileTemplate() + QLatin1String(".") + QFileInfo(filePath).suffix());
    if (!temporaryFile.open()) {
        return false;
    }

    temporaryFile.close();

    if (!pdfsave(temporaryFile.fileName(), withChanges)) {
        return false;
    }

    // Copy from temporary file to actual file...
    QFile file(filePath);

    if (!temporaryFile.open()) {
        return false;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    if (!PublicFunc::copyFile(temporaryFile, file)) {
        return false;
    }

    if (withChanges) {
        // m_bModified = false;//reset modify status
//        setBasicInfo(filePath);
    }

    return true;
}

bool DocummentPDF::saveas(const QString &filePath, bool withChanges)
{
    Q_D(DocummentPDF);
    QString strsource = d->m_fileinfo.strFilepath;
    bool bsuccess = false;
    if (!strsource.isEmpty()) {
        if (!withChanges) {
            if (QFile::copy(strsource, filePath))
                bsuccess = true;
        } else {
            QString strtmp = strsource.left(strsource.lastIndexOf(QChar('/')) + 1);
            strtmp.append(PublicFunc::getUuid());
            strtmp.append(".pdf");
            if (QFile::copy(strsource, strtmp)) {
                if (save(strsource, true)) {
                    if (QFile::copy(strsource, filePath)) {
                        bsuccess = true;
                        if (QFile::remove(strsource)) {
                            if (QFile::rename(strtmp, strsource))
                                QFile::remove(strtmp);
                        }
                    } else {
                        QFile::remove(strtmp);
                    }
                }
            }
        }
    }
    return  bsuccess;
}

bool DocummentPDF::pdfsave(const QString &filePath, bool withChanges)
{
    Q_D(DocummentPDF);
    QScopedPointer< Poppler::PDFConverter > pdfConverter(d->document->pdfConverter());

    pdfConverter->setOutputFileName(filePath);

    Poppler::PDFConverter::PDFOptions options = pdfConverter->pdfOptions();

    if (withChanges) {
        options |= Poppler::PDFConverter::WithChanges;
    }

    pdfConverter->setPDFOptions(options);

    return pdfConverter->convert();

}

void DocummentPDF::clearSearch()
{
    Q_D(DocummentPDF);
    m_searchTask->cancel();
    m_searchTask->wait();
    if (m_pagecountsearch.size() > 0) {

        foreach (int key, m_pagecountsearch.keys()) {
            d->m_pages.at(key)->clearHighlightRects();
        }
    }
}

void DocummentPDF::searchHightlight(Poppler::Page *page, const QString &strtext, stSearchRes &stres, const QColor &color)
{
    if (nullptr == page) return;
    QList<QRectF> listrect = page->search(strtext);
    if (listrect.size() <= 0)return;

    if (listrect.size() <= 0)return;
    Poppler::Annotation::Style style;
    style.setColor(color);

    Poppler::Annotation::Popup popup;
    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

    Poppler::HighlightAnnotation *annotation = new Poppler::HighlightAnnotation();

    Poppler::HighlightAnnotation::Quad quad;
    QList<Poppler::HighlightAnnotation::Quad> qlistquad;
    QRectF rec, recboundary;
    foreach (rec, listrect) {
        //获取搜索结果附近文字
        QRectF rctext = rec;
        rctext.setX(rctext.x() - 40);
        rctext.setWidth(rctext.width() + 80);
        stres.listtext.push_back(page->text(rctext));
        recboundary.setTopLeft(QPointF(rec.left() / page->pageSizeF().width(),
                                       rec.top() / page->pageSizeF().height()));
        recboundary.setTopRight(QPointF(rec.right() / page->pageSizeF().width(),
                                        rec.top() / page->pageSizeF().height()));
        recboundary.setBottomLeft(QPointF(rec.left() / page->pageSizeF().width(),
                                          rec.bottom() / page->pageSizeF().height()));
        recboundary.setBottomRight(QPointF(rec.right() / page->pageSizeF().width(),
                                           rec.bottom() / page->pageSizeF().height()));

        quad.points[0] = recboundary.topLeft();
        quad.points[1] = recboundary.topRight();
        quad.points[2] = recboundary.bottomRight();
        quad.points[3] = recboundary.bottomLeft();
        qlistquad.append(quad);
    }
    QString uniquename(PublicFunc::getUuid());
    uniquename.append("search");
    annotation->setUniqueName(uniquename);
    annotation->setHighlightQuads(qlistquad);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    page->addAnnotation(annotation);
}

void DocummentPDF::refreshOnePage(int ipage)
{
    Q_D(DocummentPDF);
    if (!d->document)
        return ;
    d->m_pages.at(ipage)->showImage(d->m_scale, d->m_rotate);
}

//void DocummentPDF::setBasicInfo(const QString &filepath)
//{
//    Q_D(DocummentPDF);
//    QFileInfo info(filepath);
//    d->m_fileinfo.size = info.size();
//    d->m_fileinfo.CreateTime = info.birthTime();
//    d->m_fileinfo.ChangeTime = info.lastModified();
//    d->m_fileinfo.strAuther = info.owner();
//    d->m_fileinfo.strFilepath = info.filePath();
//    if (d->document) {
//        int major, minor;
//        d->document->getPdfVersion(&major, &minor);
//        d->m_fileinfo.strFormat = QString("PDF v.%1.%2").arg(major).arg(minor);
//        d->m_fileinfo.boptimization = d->document->isLinearized();
//        d->m_fileinfo.strKeyword = d->document->keywords();
//        d->m_fileinfo.strTheme = d->document->title();
//        d->m_fileinfo.strProducter = d->document->producer();
//        d->m_fileinfo.strCreater = d->document->creator();
//        d->m_fileinfo.bsafe = d->document->isEncrypted();
//        d->m_fileinfo.iWidth = static_cast<PagePdf *>(d->m_pages.at(0))->GetPage()->pageSize().width();
//        d->m_fileinfo.iHeight = static_cast<PagePdf *>(d->m_pages.at(0))->GetPage()->pageSize().height();
//        d->m_fileinfo.iNumpages = d->document->numPages();
//    }
//}

bool DocummentPDF::bDocummentExist()
{
    Q_D(DocummentPDF);
    if (!d->document) {
        return false;
    }
    return true;
}

bool DocummentPDF::getImage(int pagenum, QImage &image, double width, double height)
{
    Q_D(DocummentPDF);
    return d->m_pages.at(pagenum)->getInterFace()->getImage(image, width, height);
}

void DocummentPDF::docBasicInfo(stFileInfo &info)
{
    Q_D(DocummentPDF);
    info = d->m_fileinfo;
}

bool DocummentPDF::annotationClicked(const QPoint &pos, QString &strtext)
{
    Q_D(DocummentPDF);
    QPoint pt(pos);
    int ipage = pointInWhichPage(pt);
    if (ipage < 0) return  false;
    return (static_cast<PagePdf *>(d->m_pages.at(ipage)))->annotationClicked(pt, strtext);
}

void DocummentPDF::title(QString &title)
{
    Q_D(DocummentPDF);
    title = d->document->title();
}
void DocummentPDF::setAnnotationText(int ipage, const QString &struuid, const QString &strtext)
{
    Q_D(DocummentPDF);
    if (ipage > 0 && ipage < d_ptr->m_pages.size()) {
        Poppler::Page *page = static_cast<PagePdf *>(d_ptr->m_pages.at(ipage))->GetPage();
        QList<Poppler::Annotation *> plistannote = page->annotations();
        foreach (Poppler::Annotation *annote, plistannote) {
            QString uniquename = annote->uniqueName();
            if (uniquename.isEmpty() && uniquename.compare(struuid) == 0) {
                annote->setContents(strtext);
            }
        }
        qDeleteAll(plistannote);
    }
}

void DocummentPDF::getAnnotationText(const QString &struuid, QString &strtext, int ipage)
{
    Q_D(DocummentPDF);
    if (ipage < 0) {
        for (int i = 0; i < d_ptr->m_pages.size(); ++i) {
            Poppler::Page *page = static_cast<PagePdf *>(d_ptr->m_pages.at(i))->GetPage();
            QList<Poppler::Annotation *> plistannote = page->annotations();
            foreach (Poppler::Annotation *annote, plistannote) {
                QString uniquename = annote->uniqueName();
                if (uniquename.isEmpty() && uniquename.compare(struuid) == 0) {
                    strtext = annote->contents();
                    qDeleteAll(plistannote);
                    return;
                }
            }
            qDeleteAll(plistannote);
        }
    } else {
        Poppler::Page *page = static_cast<PagePdf *>(d_ptr->m_pages.at(ipage))->GetPage();
        QList<Poppler::Annotation *> plistannote = page->annotations();
        foreach (Poppler::Annotation *annote, plistannote) {
            QString uniquename = annote->uniqueName();
            if (uniquename.isEmpty() && uniquename.compare(struuid) == 0) {
                strtext = annote->contents();
                qDeleteAll(plistannote);
                return;
            }
        }
        qDeleteAll(plistannote);
    }

}

