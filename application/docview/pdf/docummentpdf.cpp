#include "docummentpdf.h"
#include "pagepdf.h"
#include "docview/publicfunc.h"
#include <DScrollBar>
#include <QImage>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QDebug>

DocummentPDF::DocummentPDF(DWidget *parent): DocummentBase(parent),
    document(nullptr),
    m_fileinfo()
{
}

DocummentPDF::~DocummentPDF()
{
    for (int i = 0; i < m_listsearch.size(); i++) {
        delete m_listsearch.at(i);
    }
    m_listsearch.clear();
    delete document;
}

bool DocummentPDF::openFile(QString filepath)
{
    document = Poppler::Document::load(filepath);
    if (nullptr == document || document->numPages() <= 0)
        return false;
    document->setRenderHint(Poppler::Document::TextAntialiasing);
    setBasicInfo(filepath);
    donotneedreloaddoc = true;
    m_pages.clear();
    m_widgets.clear();
    qDebug() << "numPages :" << document->numPages();
    for (int i = 0; i < document->numPages(); i++) {
        DWidget *qwidget = new DWidget(this);
        QHBoxLayout *qhblayout = new QHBoxLayout(qwidget);
        qhblayout->setAlignment(qwidget, Qt::AlignCenter);
        qwidget->setLayout(qhblayout);
        m_vboxLayout.addWidget(qwidget);
        m_vboxLayout.setAlignment(&m_widget, Qt::AlignCenter);
        qwidget->setMouseTracking(true);
        m_widgets.append(qwidget);

        PagePdf *page = new PagePdf(this);
        page->setPage(document->page(i));
        m_pages.append((PageBase *)page);
    }

    for (int i = 0; i < m_pages.size(); i++) {
        m_pages.at(i)->setScaleAndRotate(m_scale, m_rotate);
    }
    setViewModeAndShow(m_viewmode);
    donotneedreloaddoc = false;
    if (m_threadloaddoc.isRunning())
        m_threadloaddoc.setRestart();
    else
        m_threadloaddoc.start();
    if (m_threadloadwords.isRunning())
        m_threadloadwords.setRestart();
    else
        m_threadloadwords.start();

    return true;
}

//bool DocummentPDF::loadPages()
//{
//    if (!document && m_pages.size() == document->numPages())
//        return false;
//    qDebug() << "loadPages";
//    //    for (int i = 0; i < m_pages.size(); i++) {
//    int pagenum  = m_currentpageno;
//    if (pagenum >= 0 && pagenum < m_pages.size())
//        m_pages.at(pagenum)->showImage(m_scale, m_rotate);
//    pagenum = m_currentpageno + 1;
//    if (pagenum >= 0 && pagenum < m_pages.size())
//        m_pages.at(pagenum)->showImage(m_scale, m_rotate);
//    pagenum = m_currentpageno - 1;
//    if (pagenum >= 0 && pagenum < m_pages.size())
//        m_pages.at(pagenum)->showImage(m_scale, m_rotate);
//    pagenum = m_currentpageno + 2;
//    if (pagenum >= 0 && pagenum < m_pages.size())
//        m_pages.at(pagenum)->showImage(m_scale, m_rotate);
//    pagenum = m_currentpageno - 2;
//    if (pagenum >= 0 && pagenum < m_pages.size())
//        m_pages.at(pagenum)->showImage(m_scale, m_rotate);
//    pagenum = m_currentpageno + 3;
//    if (pagenum >= 0 && pagenum < m_pages.size())
//        m_pages.at(pagenum)->showImage(m_scale, m_rotate);
//    pagenum = m_currentpageno - 3;
//    if (pagenum >= 0 && pagenum < m_pages.size())
//        m_pages.at(pagenum)->showImage(m_scale, m_rotate);

////    int startnum = m_currentpageno - 3;
////    if (startnum < 0) {
////        startnum = 0;
////    }
////    int endnum = startnum + 7;
////    if (endnum > m_pages.size()) {
////        endnum = m_pages.size();
////    }
////    for (int i = startnum; i < endnum; i++) {
////        if (QThread::currentThread()->isInterruptionRequested()) {
////            break;
////        }
////        m_pages.at(i)->showImage(m_scale, m_rotate);
////    }
//    return true;
//}

bool DocummentPDF::loadWords()
{
    if (!document && m_pages.size() == document->numPages())
        return false;
    qDebug() << "loadWords";
    for (int i = 0; i < m_pages.size(); i++) { //根据获取到的pdf页数循环
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        //        qDebug() << "i:" << i << " width:" << m_pages.at(i)->width() << " height:" << m_pages.at(i)->height();
        loadWordCache(i, m_pages.at(i));
    }
    return true;
}

void DocummentPDF::removeAllAnnotation()
{
    if (!document)return;
    for (int i = 0; i < document->numPages(); ++i) {
        QList<Poppler::Annotation *> listannote = document->page(i)->annotations();
        foreach (Poppler::Annotation *atmp, listannote) {
            document->page(i)->removeAnnotation(atmp);
        }
    }
    scaleAndShow(m_scale, m_rotate);
}

QString DocummentPDF::removeAnnotation(const QPoint &startpos)
{
    //暂时只处理未旋转
    QPoint pt = startpos;
    int page = pointInWhichPage(pt);
    if (page < 0) return "";
    return static_cast<PagePdf *>(m_pages.at(page))->removeAnnotation(pt);
}

void DocummentPDF::removeAnnotation(const QString &struuid)
{
    return static_cast<PagePdf *>(m_pages.at(currentPageNo()))->removeAnnotation(struuid);
}

QString DocummentPDF::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    QPoint pt = startpos;
    int page = pointInWhichPage(pt);
    if (page < 0) return "";
    return static_cast<PagePdf *>(m_pages.at(page))->addAnnotation(pt);
}

void DocummentPDF::search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color)
{
    //先清理
    if (m_pagecountsearch.size() > 0) {
        clearSearch();
        m_pagecountsearch.clear();
    }
    for (int i = 0; i < document->numPages(); ++i) {
        stSearchRes stres;
        searchHightlight(document->page(i), strtext, stres, color);
        int icount = stres.listtext.size();
        if (icount > 0) {
            resmap.insert(i, stres);
            m_pagecountsearch.insert(i, icount);
        }
    }

    int curpage = currentPageNo();
    if (m_pagecountsearch.size() > 0) {
        m_cursearch = 1;
        m_findcurpage = m_pagecountsearch.firstKey();
        if (m_pagecountsearch.find(curpage) != m_pagecountsearch.end()) {
            static_cast<PagePdf *>(m_pages.at(curpage))->showImage(m_scale, m_rotate);
        }
        // scaleAndShow(m_scale, m_rotate); //全部刷新
    }
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
        setBasicInfo(filePath);
    }

    return true;
}

bool DocummentPDF::saveas(const QString &filePath, bool withChanges)
{
    QString strsource=m_fileinfo.strFilepath;
    bool bsuccess=false;
    if(!strsource.isEmpty())
    {
        if(!withChanges)
        {
            if(QFile::copy(strsource,filePath))
                bsuccess=true;
        }
        else {
            QString strtmp=strsource.left(strsource.lastIndexOf(QChar('/'))+1);
            strtmp.append(PublicFunc::getUuid());
            strtmp.append(".pdf");
            if(QFile::copy(strsource,strtmp))
            {
                if(save(strsource,true))
                {
                    if(QFile::copy(strsource,filePath))
                    {
                        bsuccess=true;
                        if(QFile::remove(strsource))
                        {
                            if(QFile::rename(strtmp,strsource))
                                QFile::remove(strtmp);
                        }
                    }
                    else {
                        QFile::remove(strtmp);
                    }
                }
            }
        }
    }
    return  bsuccess;
}

bool DocummentPDF::pdfsave(const QString &filePath, bool withChanges) const
{
    QScopedPointer< Poppler::PDFConverter > pdfConverter(document->pdfConverter());

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
    if (m_pagecountsearch.size() > 0) {
        foreach (int i, m_pagecountsearch.keys()) {
            Poppler::Page *page = document->page(i);
            QList<Poppler::Annotation *> listannoate = page->annotations();
            foreach (Poppler::Annotation *anote, listannoate) {
                if (anote->uniqueName().endsWith(QString("search"))) {
                    page->removeAnnotation(anote);
                }
            }
        }
        scaleAndShow(m_scale, m_rotate);
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
    if (!document)
        return ;
    m_pages.at(ipage)->showImage(m_scale, m_rotate);
}

void DocummentPDF::setBasicInfo(const QString &filepath)
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
        m_fileinfo.iWidth = document->page(0)->pageSize().width();
        m_fileinfo.iHeight = document->page(0)->pageSize().height();
        m_fileinfo.iNumpages = document->numPages();
    }
}

void DocummentPDF::loadWordCache(int indexpage, PageBase *page)
{
    if (!document) {
        return;
    }
    Poppler::Page *pp = document->page(indexpage);
    QList<Poppler::TextBox *> textList;
    pp->textList();
    if (pp) {
        textList = pp->textList();
        const QSizeF s = pp->pageSizeF();
    }
    delete pp;

    abstractTextPage(textList, page);
    qDeleteAll(textList);
    PagePdf *ppdf = (PagePdf *)page;
    ppdf->loadLinks();
}

bool DocummentPDF::abstractTextPage(const QList<Poppler::TextBox *> &text, PageBase *page)
{
    Poppler::TextBox *next;
    PagePdf *ppdf = (PagePdf *)page;
    QString s;
    bool addChar;
    foreach (Poppler::TextBox *word, text) {
        const int qstringCharCount = word->text().length();
        next = word->nextWord();
        // if(next)
        int textBoxChar = 0;
        for (int j = 0; j < qstringCharCount; j++) {
            const QChar c = word->text().at(j);
            if (c.isHighSurrogate()) {
                s = c;
                addChar = false;
            } else if (c.isLowSurrogate()) {
                s += c;
                addChar = true;
            } else {
                s = c;
                addChar = true;
            }

            if (addChar) {
                QRectF charBBox = word->charBoundingBox(textBoxChar);
                //                qDebug() << "addChar s:" << s << " charBBox:" << charBBox;
                stWord sword;
                sword.s = s;
                sword.rect = charBBox;
                ppdf->appendWord(sword);
                textBoxChar++;
            }
        }

        if (word->hasSpaceAfter() && next) {
            QRectF wordBBox = word->boundingBox();
            QRectF nextWordBBox = next->boundingBox();
            //            qDebug() << "hasSpaceAfter wordBBox:" << wordBBox << " nextWordBBox:" << nextWordBBox;
            stWord sword;
            sword.s = QStringLiteral(" ");
            QRectF qrect;
            qrect.setLeft(wordBBox.right());
            qrect.setBottom(wordBBox.bottom());
            qrect.setRight(nextWordBBox.left());
            qrect.setTop(wordBBox.top());
            sword.rect = qrect;
            ppdf->appendWord(sword);
        }
    }
    return true;
}

bool DocummentPDF::bDocummentExist()
{
    if (!document) {
        return false;
    }
    return true;
}

bool DocummentPDF::getImage(int pagenum, QImage &image, double width, double height)
{
    return m_pages.at(pagenum)->getImage(image, width, height);
}

void DocummentPDF::docBasicInfo(stFileInfo &info)
{
    info = m_fileinfo;
}

bool DocummentPDF::annotationClicked(const QPoint &pos, QString &strtext)
{
    QPoint pt(pos);
    int ipage = pointInWhichPage(pt);
    if (ipage < 0) return  false;
    return static_cast<PagePdf>(m_pages.at(ipage)).annotationClicked(pt, strtext);
}

void DocummentPDF::title(QString &title)
{
    title = document->title();
}

void DocummentPDF::findNext()
{
    if (m_pagecountsearch.size() <= 0) return;
    if (m_findcurpage == m_pagecountsearch.lastKey() &&
            m_cursearch == m_pagecountsearch.find(m_findcurpage).value()) {
        m_findcurpage = m_pagecountsearch.firstKey();
        m_cursearch = 1;
    }
    //    if(m_pagecountsearch.find(m_findcurpage)==m_pagecountsearch.end())
    //    {
    //        QList<int> keys=m_pagecountsearch.keys();
    //        foreach(int i,keys)
    //        {
    //            if(i>=m_findcurpage)
    //            {
    //                m_findcurpage=i;
    //                m_cursearch=1;
    //                break;
    //            }
    //        }
    //    }

    if (m_pagecountsearch.find(m_findcurpage).value() >= m_cursearch) {
        Poppler::Page *page = document->page(m_findcurpage);
        QList<Poppler::Annotation *> plistannote = page->annotations();
        foreach (Poppler::Annotation *annote, plistannote) {
            if (annote->uniqueName().endsWith(QString("search")) &&
                    annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
                double curheight = m_scale * page->pageSizeF().height();
                double topspace = (height() - curheight) / 2;
                QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
                Poppler::HighlightAnnotation::Quad quadtem = listquad.at(m_cursearch - 1);
                int value = m_widgets.at(m_findcurpage)->y() + topspace + quadtem.points[0].y() * m_scale * page->pageSizeF().height();
                QScrollBar *scrollBar_Y = verticalScrollBar();
                if (scrollBar_Y)
                    scrollBar_Y->setValue(value);
                m_cursearch++;
                break;
            }
        }
    } else {

        QMap<int, int>::const_iterator it = m_pagecountsearch.find(m_findcurpage);
        if (++it != m_pagecountsearch.end()) {
            m_cursearch = 1;
            m_findcurpage = it.key();
            Poppler::Page *page = document->page(m_findcurpage);
            QList<Poppler::Annotation *> plistannote = page->annotations();
            foreach (Poppler::Annotation *annote, plistannote) {
                if (annote->uniqueName().endsWith(QString("search")) &&
                        annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
                    double curheight = m_scale * page->pageSizeF().height();
                    double topspace = (height() - curheight) / 2;
                    QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
                    Poppler::HighlightAnnotation::Quad quadtem = listquad.at(m_cursearch - 1);
                    int value = m_widgets.at(m_findcurpage)->y() + topspace + quadtem.points[0].y() * m_scale * page->pageSizeF().height();
                    QScrollBar *scrollBar_Y = verticalScrollBar();
                    if (scrollBar_Y)
                        scrollBar_Y->setValue(value);
                    m_cursearch++;
                    break;
                }
            }
        }
    }
}

void DocummentPDF::findPrev()
{
    if (m_pagecountsearch.size() <= 0) return;
    if (m_findcurpage == m_pagecountsearch.firstKey() &&
            m_cursearch < 1) {
        m_findcurpage = m_pagecountsearch.lastKey();
        m_cursearch = m_pagecountsearch.find(m_findcurpage).value();
    }

    if (m_cursearch >= 1) {
        Poppler::Page *page = document->page(m_findcurpage);
        QList<Poppler::Annotation *> plistannote = page->annotations();
        foreach (Poppler::Annotation *annote, plistannote) {
            if ( annote->subType() == Poppler::Annotation::AHighlight &&
                 annote->uniqueName().endsWith(QString("search"))) { //必须判断
                double curheight = m_scale * page->pageSizeF().height();
                double topspace = (height() - curheight) / 2;
                QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
                Poppler::HighlightAnnotation::Quad quadtem = listquad.at(m_cursearch - 1);
                int value = m_widgets.at(m_findcurpage)->y() + topspace + quadtem.points[0].y() * m_scale * page->pageSizeF().height();
                QScrollBar *scrollBar_Y = verticalScrollBar();
                if (scrollBar_Y)
                    scrollBar_Y->setValue(value);
                m_cursearch--;
                break;
            }
        }
    } else {
        QMap<int, int>::const_iterator it = m_pagecountsearch.find(m_findcurpage);
        if (--it != m_pagecountsearch.end()) {
            m_cursearch = it.value();
            m_findcurpage = it.key();
            qDebug() << m_cursearch << m_findcurpage;
            Poppler::Page *page = document->page(m_findcurpage);
            QList<Poppler::Annotation *> plistannote = page->annotations();
            foreach (Poppler::Annotation *annote, plistannote) {
                if (annote->uniqueName().endsWith(QString("search")) &&
                        annote->subType() == Poppler::Annotation::AHighlight) { //必须判断
                    double curheight = m_scale * page->pageSizeF().height();
                    double topspace = (height() - curheight) / 2;
                    QList<Poppler::HighlightAnnotation::Quad> listquad = static_cast<Poppler::HighlightAnnotation *>(annote)->highlightQuads();
                    Poppler::HighlightAnnotation::Quad quadtem = listquad.at(m_cursearch - 1);
                    int value = m_widgets.at(m_findcurpage)->y() + topspace + quadtem.points[0].y() * m_scale * page->pageSizeF().height();
                    QScrollBar *scrollBar_Y = verticalScrollBar();
                    if (scrollBar_Y)
                        scrollBar_Y->setValue(value);
                    m_cursearch--;
                    break;
                }
            }
        }
    }

}

void DocummentPDF::setAnnotationText(int ipage, const QString &struuid, const QString &strtext)
{
    if (ipage > 0 && ipage < document->numPages()) {
        Poppler::Page *page = document->page(ipage);
        foreach (Poppler::Annotation *annote, page->annotations()) {
            QString uniquename = annote->uniqueName();
            if (uniquename.isEmpty() && uniquename.compare(struuid) == 0) {
                annote->setContents(strtext);
            }
        }
    }
}

void DocummentPDF::getAnnotationText(const QString &struuid, QString &strtext, int ipage)
{
    if(ipage<0)
    {
        for (int i = 0; i < document->numPages(); ++i) {
            Poppler::Page *page = document->page(i);
            foreach (Poppler::Annotation *annote, page->annotations()) {
                QString uniquename = annote->uniqueName();
                if (uniquename.isEmpty() && uniquename.compare(struuid) == 0) {
                    strtext = annote->contents();
                    return;
                }
            }
        }
    }
    else {
        Poppler::Page *page = document->page(ipage);
        foreach (Poppler::Annotation *annote, page->annotations()) {
            QString uniquename = annote->uniqueName();
            if (uniquename.isEmpty() && uniquename.compare(struuid) == 0) {
                strtext = annote->contents();
                return;
            }
        }
    }

}


