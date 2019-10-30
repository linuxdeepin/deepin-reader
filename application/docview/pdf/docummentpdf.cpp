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
#include <QFile>

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
    for (int i = 0; i < document->numPages(); i++) {
        PagePdf *page = new PagePdf(q);
        page->setPage(document->page(i), i);
        m_pages.append((PageBase *)page);
    }
    if (m_pages.size() > 0) {
        m_imagewidth = m_pages.at(0)->getOriginalImageWidth();
        m_imageheight = m_pages.at(0)->getOriginalImageHeight();
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
        int major, minor;
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
    //    Q_D(DocummentPDF);
    //    delete d->document;
    //    d->document = nullptr;
}

bool DocummentPDF::loadDocumment(QString filepath)
{
    Q_D(DocummentPDF);
    //    d->loadDocumment(filepath);
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
                    rectbound.setBottomLeft(listquad.at(0).points[2]);
                    rectbound.setBottomRight( listquad.at(0).points[3]);
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
    QPoint pt = startpos;
    int page = pointInWhichPage(pt);
    qDebug() << "removeAnnotation start";
    if (page < 0) return "";
    qDebug() << "removeAnnotation end";
    return static_cast<PagePdf *>(d->m_pages.at(page))->removeAnnotation(pt);
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
        //qDebug()<<"DocummentPDF::addAnnotation";
        //rect=QRect(50,30,100,50);
        uuid = static_cast<PagePdf *>(d->m_pages.at(startpage))->addAnnotation(color, rect);
    } else {
        if (startpage < 0) return "";
        uuid = static_cast<PagePdf *>(d->m_pages.at(startpage))->addAnnotation(color);
    }
    return uuid;
}

void DocummentPDF::getAllAnnotation(QList<stHighlightContent> &listres)
{
    Q_D(DocummentPDF);
    //    QTime t;
    //    t.start();//将此时间设置为当前时间
    for (int i = 0; i < d->m_pages.size(); ++i) {
        QList<Poppler::Annotation *> listannote = static_cast<PagePdf *>(d->m_pages.at(i))->GetPage()->annotations();
        //  qDebug()<<"getAllAnnotation"<<i<<listannote.size();
        foreach (Poppler::Annotation *annote, listannote) {
            if (annote->subType() == Poppler::Annotation::AHighlight) {
                stHighlightContent stres;
                //QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
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
    //elapsed(): 返回自上次调用start()或restart()以来经过的毫秒数
    // qDebug()<<"----getAllAnnotation----"<<t.elapsed()<<"ms"<<__func__;
}

void DocummentPDF::search(const QString &strtext, QColor color)
{
    Q_D(DocummentPDF);
    clearSearch();
    d->m_searchTask->start(d->m_pages, strtext, false, false, d->m_currentpageno + 1);
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
    return true;
}

bool DocummentPDF::saveas(const QString &filePath, bool withChanges)
{
    Q_D(DocummentPDF);
    QString strsource = d->m_fileinfo->strFilepath;
    bool bsuccess = false;
    if (!strsource.isEmpty()) {
        qDebug() << "saveas1 " << strsource;
        if (!withChanges) {
            if (QFile::copy(strsource, filePath))
                bsuccess = true;
            else {
                qDebug() << "saveas5 " << strsource;
            }
        } else {
            QString strtmp = strsource.left(strsource.lastIndexOf(QChar('/')) + 1);
            strtmp.append(PublicFunc::getUuid());
            strtmp.append(".pdf");
            qDebug() << "saveas2" << strtmp;
            if (QFile::copy(strsource, strtmp)) {
                if (save(strsource, true)) {
                    if (QFile::copy(strsource, filePath)) {
                        bsuccess = true;
                        qDebug() << "saveas3 " << strsource;
                        if (QFile::remove(strsource)) {
                            if (QFile::rename(strtmp, strsource))
                                QFile::remove(strtmp);
                        }
                    } else {
                        qDebug() << "saveas6 " << strtmp;
                        QFile::remove(strtmp);
                    }
                }
            } else {
                QFile file(strtmp);
                if (file.exists())
                    qDebug() << "saveas7 file.exists()";
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
    d->m_searchTask->cancel();
    d->m_searchTask->wait();
    if (d->m_pagecountsearch.size() > 0) {

        foreach (int key, d->m_pagecountsearch.keys()) {
            d->m_pages.at(key)->clearHighlightRects();
        }
    }
    if (d->m_currentpageno - 1 >= 0)
        d->m_pages.at(d->m_currentpageno - 1)->update();
    if (d->m_currentpageno + 1 < d->m_pages.size())
        d->m_pages.at(d->m_currentpageno + 1)->update();
    d->m_pages.at(d->m_currentpageno)->update();//刷新当前页

}

void DocummentPDF::refreshOnePage(int ipage)
{
    Q_D(DocummentPDF);
    if (!d->document)
        return ;
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

bool DocummentPDF::getImage(int pagenum, QImage &image, double width, double height)
{
    Q_D(DocummentPDF);
    if (pagenum < 0 || pagenum >= d->m_pages.size()) {
        return false;
    }
    qreal pixelratiof = d->m_pages.at(pagenum)->devicePixelRatioF();
    if (!d->m_pages.at(pagenum)->getImage(image, width * pixelratiof, height * pixelratiof)) {
        return false;
    }
    image.setDevicePixelRatio(d->m_pages.at(pagenum)->devicePixelRatioF());
    return true;
}

void DocummentPDF::docBasicInfo(stFileInfo &info)
{
    Q_D(DocummentPDF);
    info = *(d->m_fileinfo);
}

bool DocummentPDF::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    Q_D(DocummentPDF);
    QPoint pt(pos);
    int ipage = pointInWhichPage(pt);
    if (ipage < 0) return  false;
    return (static_cast<PagePdf *>(d->m_pages.at(ipage)))->annotationClicked(pt, strtext, struuid);
}

void DocummentPDF::title(QString &title)
{
    Q_D(DocummentPDF);
    title = d->document->title();
}
void DocummentPDF::setAnnotationText(int ipage, const QString &struuid, const QString &strtext)
{
    qDebug() << "setAnnotationText********************";
    Q_D(DocummentPDF);
    if (ipage >= 0 && ipage < d_ptr->m_pages.size()) {
        Poppler::Page *page = static_cast<PagePdf *>(d_ptr->m_pages.at(ipage))->GetPage();
        QList<Poppler::Annotation *> plistannote = page->annotations();
        foreach (Poppler::Annotation *annote, plistannote) {
            QString uniquename = annote->uniqueName();
            qDebug() << "setAnnotationText--" << uniquename << struuid;
            if (!uniquename.isEmpty() && uniquename.indexOf(struuid) >= 0) {
                annote->setContents(strtext);
                qDebug() << "setAnnotationText++" << annote->contents();
            }
        }
        qDeleteAll(plistannote);
    }
}

void DocummentPDF::getAnnotationText(const QString &struuid, QString &strtext, int ipage)
{   
    Q_D(DocummentPDF);
    if(ipage>=0&&ipage<d->m_pages.size())
    {
        Poppler::Page *page = static_cast<PagePdf *>(d_ptr->m_pages.at(ipage))->GetPage();
        QList<Poppler::Annotation *> plistannote = page->annotations();
        foreach (Poppler::Annotation *annote, plistannote) {
            QString uniquename = annote->uniqueName();
            if (!uniquename.isEmpty() && uniquename.indexOf(struuid) >= 0&&!struuid.isEmpty()) {
                strtext = annote->contents();
                qDebug() << QString("getAnnotationText=%1=%2=%3=%4").arg(uniquename).arg(struuid).arg(ipage).arg(strtext);
                qDeleteAll(plistannote);
                return;
            }
        }
        qDeleteAll(plistannote);
    }
}

