#include "docummentpdf.h"
#include "pagepdf.h"
#include "docview/publicfunc.h"
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QScrollBar>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QMutex>

//static QMutex mutexloaddoc;
//static QMutex mutexloadwords;


ThreadLoadDoc::ThreadLoadDoc()
{
    //    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_doc = nullptr;
    restart = false;
}

void ThreadLoadDoc::setRestart()
{
    restart = true;
}

void ThreadLoadDoc::setDoc(DocummentPDF *doc)
{
    m_doc = doc;
}

void ThreadLoadDoc::run()
{
//    mutexloaddoc.lock();
    if (!m_doc)
        return;
    restart = true;
    while (restart) {
        restart = false;
        m_doc->loadPages();
    }
//    mutexloaddoc.unlock();
}


ThreadLoadWords::ThreadLoadWords()
{
    //    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_doc = nullptr;
    restart = false;
}

void ThreadLoadWords::setDoc(DocummentPDF *doc)
{
    m_doc = doc;
}

void ThreadLoadWords::setRestart()
{
    restart = true;
}

void ThreadLoadWords::run()
{
//    mutexloadwords.lock();
    if (!m_doc)
        return;
    restart = true;
    while (restart) {
        restart = false;
        m_doc->loadWords();
    }
//    mutexloadwords.unlock();
}


DocummentPDF::DocummentPDF(QWidget *parent): DocummentBase(parent),
    document(nullptr),
    m_listsearch(),
    m_fileinfo()
{
    m_threadloaddoc.setDoc(this);
    m_threadloadwords.setDoc(this);
    setWidgetResizable(true);
    setWidget(&m_widget);
    m_scale = 1;
    m_rotate = RotateType_Normal;
    m_currentpageno = 0;
    pblankwidget = new QWidget(this);
    pblankwidget->setMouseTracking(true);
    pblankwidget->hide();
    donotneedreloaddoc = false;
}

bool DocummentPDF::openFile(QString filepath)
{
    document = Poppler::Document::load(filepath);
    if (nullptr == document || document->numPages() <= 0)
        return false;
    document->setRenderHint(Poppler::Document::TextAntialiasing);
    m_pages.clear();
    setBasicInfo(filepath);
    qDebug() << "numPages :" << document->numPages();
    for (int i = 0; i < document->numPages(); i++) {
        QWidget *qwidget = new QWidget(this);
        QHBoxLayout *qhblayout = new QHBoxLayout(qwidget);
        qwidget->setLayout(qhblayout);
        m_vboxLayout.addWidget(qwidget);
        //        m_vboxLayout.addWidget(m_pages.at(i));
        m_vboxLayout.setAlignment(&m_widget, Qt::AlignCenter);
        qwidget->setMouseTracking(true);
        m_widgets.append(qwidget);

        PagePdf *page = new PagePdf(this);
        page->setPage(document->page(i));
        m_pages.append((PageBase *)page);
    }

    for (int i = 0; i < m_pages.size(); i++) {
        PagePdf *page = (PagePdf *)m_pages.at(i);
        page->setScaleAndRotate(m_scale, m_rotate);
    }
    setViewModeAndShow(m_viewmode);
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

bool DocummentPDF::setViewModeAndShow(ViewMode_EM viewmode)
{
    int currpageno = m_currentpageno;
    m_viewmode = viewmode;
    donotneedreloaddoc = true;
    switch (m_viewmode) {
    case ViewMode_SinglePage:
        showSinglePage();
        break;
    case ViewMode_FacingPage:
        showFacingPage();
        break;
    default:
        return false;
        break;
    }
    pageJump(currpageno);
    donotneedreloaddoc = false;
    if (m_threadloaddoc.isRunning())
        m_threadloaddoc.setRestart();
    else
        m_threadloaddoc.start();
    return true;;
}

void DocummentPDF::showSinglePage()
{
    pblankwidget->hide();
    for (int i = 0; i < m_pages.size(); i++) {
        m_widgets.at(i)->layout()->addWidget(m_pages.at(i));
        m_widgets.at(i)->show();
    }
}
void DocummentPDF::showFacingPage()
{
    for (int i = 0; i < m_widgets.size(); i++) {
        m_widgets.at(i)->hide();
    }
    pblankwidget->hide();
    for (int i = 0; i < m_pages.size() / 2; i++) {
        m_widgets.at(i)->layout()->addWidget(m_pages.at(i * 2));
        m_widgets.at(i)->layout()->addWidget(m_pages.at(i * 2 + 1));
        m_widgets.at(i)->show();
    }
    if (m_pages.size() % 2) {
        pblankwidget->show();
        m_widgets.at(m_pages.size() / 2)->layout()->addWidget(m_pages.at(m_pages.size() - 1));
        m_widgets.at(m_pages.size() / 2)->layout()->addWidget(pblankwidget);
        m_widgets.at(m_pages.size() / 2)->show();
    }
}

bool DocummentPDF::loadPages()
{
    if (!document && m_pages.size() == document->numPages())
        return false;
    qDebug() << "loadPages";
    //    for (int i = 0; i < m_pages.size(); i++) {
    int startnum = m_currentpageno - 3;
    if (startnum < 0) {
        startnum = 0;
    }
    int endnum = startnum + 7;
    if (endnum > m_pages.size()) {
        endnum = m_pages.size();
    }
    for (int i = startnum; i < endnum; i++) {
        PagePdf *ppdf = (PagePdf *)m_pages.at(i);
        ppdf->showImage(m_scale, m_rotate);
    }
    return true;
}

bool DocummentPDF::loadWords()
{
    if (!document && m_pages.size() == document->numPages())
        return false;
    qDebug() << "loadWords";
    for (int i = 0; i < m_pages.size(); i++) { //根据获取到的pdf页数循环
        //        qDebug() << "i:" << i << " width:" << m_pages.at(i)->width() << " height:" << m_pages.at(i)->height();
        loadWordCache(i, m_pages.at(i));
    }
    return true;
}

void DocummentPDF::scaleAndShow(double scale, RotateType_EM rotate)
{
    int currpageno = m_currentpageno;
    m_scale = scale;
    m_rotate = rotate;
    donotneedreloaddoc = true;
    for (int i = 0; i < m_pages.size(); i++) {
        PagePdf *page = (PagePdf *)m_pages.at(i);
        page->setScaleAndRotate(m_scale, m_rotate);
    }
    pageJump(currpageno);
    donotneedreloaddoc = false;

    if (m_threadloaddoc.isRunning())
        m_threadloaddoc.setRestart();
    else
        m_threadloaddoc.start();
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
    scaleAndShow(m_scale,m_rotate);
}

QString DocummentPDF::removeAnnotation(const QPoint &startpos)
{
    //暂时只处理未旋转
    QPoint pt=startpos;
    int page=pointInWhichPage(pt);
    return static_cast<PagePdf*>(m_pages.at(page))->removeAnnotation(pt);
}

void DocummentPDF::removeAnnotation(const QString &struuid)
{
    return static_cast<PagePdf*>(m_pages.at(currentPageNo()))->removeAnnotation(struuid);
}

void DocummentPDF::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{    
    QPoint pt=startpos;
    int page=pointInWhichPage(pt);
    static_cast<PagePdf*>(m_pages.at(page))->addAnnotation(pt);
}

void DocummentPDF::search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color)
{
    //先清理
    if (m_listsearch.size() > 0) {
        clearSearch();
    }

    for (int i = 0; i < document->numPages(); ++i) {
        stSearchRes stres;
        searchHightlight(document->page(i), strtext, stres, color);
        if (stres.listtext.size() > 0)
            resmap.insert(i, stres);
    }
    static_cast<PagePdf*>(m_pages.at(currentPageNo()))->showImage(m_scale,m_rotate);
    scaleAndShow(m_scale, m_rotate); //全部刷新
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
    for (int i = 0; i < document->numPages(); ++i) {
        foreach (Poppler::Annotation *ptmp, m_listsearch) {
            document->page(i)->removeAnnotation(ptmp);
        }
        //refresh(i);
    }
    scaleAndShow(m_scale, m_rotate);
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

        qDebug() << "**" << rec << "**";
        quad.points[0] = recboundary.topLeft();
        quad.points[1] = recboundary.topRight();
        quad.points[2] = recboundary.bottomRight();
        quad.points[3] = recboundary.bottomLeft();
        qlistquad.append(quad);
    }
    annotation->setHighlightQuads(qlistquad);
    //annotation->setBoundary(recboundary);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    page->addAnnotation(annotation);
    m_listsearch.append(annotation);
}

void DocummentPDF::refreshOnePage(int ipage)
{
    if (!document)
        return ;
    PagePdf *ppdf = (PagePdf *)m_pages.at(ipage);
    ppdf->showImage(m_scale, m_rotate);
}

void DocummentPDF::setBasicInfo(const QString &filepath)
{
    QFileInfo info(filepath);
    m_fileinfo.size=info.size();
    m_fileinfo.CreateTime=info.birthTime();
    m_fileinfo.ChangeTime=info.lastModified();
    m_fileinfo.strAuther=info.owner();
    m_fileinfo.strFilepath=info.filePath();
    if(document)
    {
        m_fileinfo.iWidth=document->page(0)->pageSize().width();
        m_fileinfo.iHeight=document->page(0)->pageSize().height();
        m_fileinfo.iNumpages=document->numPages();
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

void DocummentPDF::mouseSelectTextClear()
{
    for (int i = 0; i < m_pages.size(); i++) {
        PagePdf *ppdf = (PagePdf *)m_pages.at(i);
        ppdf->clearPageTextSelections();
    }
}

QPoint DocummentPDF::global2RelativePoint(QPoint globalpoint)
{
    int x_offset = 0;
    int y_offset = 0;
    QScrollBar *scrollBar_X = horizontalScrollBar();
    if (scrollBar_X)
        x_offset = scrollBar_X->value();
    QScrollBar *scrollBar_Y = verticalScrollBar();
    if (scrollBar_Y)
        y_offset = scrollBar_Y->value();
    QPoint qpoint = QPoint(mapFromGlobal(globalpoint).x() + x_offset,
                           mapFromGlobal(globalpoint).y() + y_offset);
    //    qDebug() << "globalpoint:" << globalpoint << " relativepoint:" << qpoint;
    return qpoint;
}

bool DocummentPDF::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    if (!document) {
        return false;
    }
    for (int i = 0; i < m_pages.size(); i++) {
        PagePdf *ppdf = (PagePdf *)m_pages.at(i);
        ppdf->setSelectTextStyle(paintercolor, pencolor, penwidth);
    }
}

bool DocummentPDF::mouseSelectText(QPoint start, QPoint stop)
{
    if (!document) {
        return false;
    }
    QPoint qstart = start;
    QPoint qstop = stop;
    qDebug() << "startpoint:" << start << " stoppoint:" << stop;
    int startpagenum = -1, endpagenum = -1;

    for (int i = 0; i < m_widgets.size(); i++) {
        if (qstop.x() > m_widgets.at(i)->x() &&
                qstop.x() <
                (m_widgets.at(i)->width() + m_widgets.at(i)->x()) &&
                qstop.y() > m_widgets.at(i)->y() &&
                qstop.y() <
                (m_widgets.at(i)->height() + m_widgets.at(i)->y())) {
            qstop = QPoint(qstop.x() - m_widgets.at(i)->x(), qstop.y() - m_widgets.at(i)->y());
            switch (m_viewmode) {
            case ViewMode_SinglePage:
                endpagenum = i;
                break;
            case ViewMode_FacingPage:
                if (qstop.x() > m_pages.at(2 * i)->x() &&
                        qstop.x() <
                        (m_pages.at(2 * i)->width() + m_pages.at(2 * i)->x()) &&
                        qstop.y() > m_pages.at(2 * i)->y() &&
                        qstop.y() <
                        (m_pages.at(2 * i)->height() + m_pages.at(2 * i)->y())) {
                    endpagenum = 2 * i;
                } else {
                    endpagenum = 2 * i + 1;
                    if (endpagenum >= m_pages.size()) {
                        endpagenum = 2 * i;
                        qstop = QPoint(m_pages.at(endpagenum)->width() + m_pages.at(endpagenum)->x(),
                                       m_pages.at(endpagenum)->height() + m_pages.at(endpagenum)->y());
                    }
                }
                break;
            default:
                break;
            }
            break;
        }
    }


    for (int i = 0; i < m_widgets.size(); i++) {
        if (qstart.x() > m_widgets.at(i)->x() &&
                qstart.x() <
                (m_widgets.at(i)->width() + m_widgets.at(i)->x()) &&
                qstart.y() > m_widgets.at(i)->y() &&
                qstart.y() <
                (m_widgets.at(i)->height() + m_widgets.at(i)->y())) {
            qstart = QPoint(qstart.x() - m_widgets.at(i)->x(), qstart.y() - m_widgets.at(i)->y());
            switch (m_viewmode) {
            case ViewMode_SinglePage:
                startpagenum = i;
                break;
            case ViewMode_FacingPage:
                if (qstart.x() > m_pages.at(2 * i)->x() &&
                        qstart.x() <
                        (m_pages.at(2 * i)->width() + m_pages.at(2 * i)->x()) &&
                        qstart.y() > m_pages.at(2 * i)->y() &&
                        qstart.y() <
                        (m_pages.at(2 * i)->height() + m_pages.at(2 * i)->y())) {
                    startpagenum = 2 * i;
                } else {
                    startpagenum = 2 * i + 1;
                    if (startpagenum >= m_pages.size()) {
                        startpagenum = 2 * i;
                        qstart = QPoint(m_pages.at(startpagenum)->width() + m_pages.at(startpagenum)->x(),
                                        m_pages.at(startpagenum)->height() + m_pages.at(startpagenum)->y());
                    }
                }
                break;
            default:
                break;
            }
            break;
        }
    }

    //    qDebug() << "startpagenum:" << startpagenum << " endpagenum:" << endpagenum;
    if (-1 == startpagenum || -1 == endpagenum)
        return false;
    if (startpagenum > endpagenum) {
        int mi = startpagenum;
        startpagenum = endpagenum;
        endpagenum = mi;
        QPoint mp = qstart;
        qstart = qstop;
        qstop = mp;
    }
    //    qDebug() << "startpagenum:" << startpagenum << " endpagenum:" << endpagenum;
    bool re = false;
    for (int i = startpagenum; i < endpagenum + 1; i++) {
        PagePdf *ppdf = (PagePdf *)m_pages.at(i);
        QPoint pfirst = QPoint(m_pages.at(i)->x(), m_pages.at(i)->y());
        QPoint plast = QPoint(m_pages.at(i)->width() + m_pages.at(i)->x(),
                              m_pages.at(i)->height() + m_pages.at(i)->y());
        switch (m_rotate) {
        case RotateType_90:
            pfirst = QPoint(m_pages.at(i)->x() + m_pages.at(i)->width(), m_pages.at(i)->y());
            plast = QPoint(m_pages.at(i)->x(),
                           m_pages.at(i)->height() + m_pages.at(i)->y());
            break;
        case RotateType_180:
            pfirst = QPoint(m_pages.at(i)->x() + m_pages.at(i)->width(), m_pages.at(i)->y());
            plast = QPoint(m_pages.at(i)->x(), m_pages.at(i)->y());
            break;
        case RotateType_270:
            pfirst = QPoint(m_pages.at(i)->x(), m_pages.at(i)->height() + m_pages.at(i)->y());
            plast = QPoint(m_pages.at(i)->x() + m_pages.at(i)->width(),
                           m_pages.at(i)->y());
            break;
        default:
            break;
        }
        if (i == startpagenum) {
            if (startpagenum == endpagenum) {
                re = ppdf->pageTextSelections(qstart, qstop);
            } else {
                re = ppdf->pageTextSelections(qstart,
                                              plast);
            }
        } else if (i == endpagenum) {
            re = ppdf->pageTextSelections(
                        pfirst,
                        qstop);
        } else {
            re = ppdf->pageTextSelections(pfirst,
                                          plast);
        }
    }
    return re;
}

int DocummentPDF::pointInWhichPage(QPoint &qpoint)
{
    int pagenum = -1;
    for (int i = 0; i < m_widgets.size(); i++) {
        if (qpoint.x() > m_widgets.at(i)->x() &&
                qpoint.x() <
                (m_widgets.at(i)->width() + m_widgets.at(i)->x()) &&
                qpoint.y() > m_widgets.at(i)->y() &&
                qpoint.y() <
                (m_widgets.at(i)->height() + m_widgets.at(i)->y())) {
            qpoint = QPoint(qpoint.x() - m_widgets.at(i)->x(), qpoint.y() - m_widgets.at(i)->y());
            switch (m_viewmode) {
            case ViewMode_SinglePage:
                pagenum = i;
                //                qpoint = QPoint(qpoint.x() - m_widgets.at(i)->x(), qpoint.y() - m_widgets.at(i)->y());
                break;
            case ViewMode_FacingPage:
                //                qpoint = QPoint(qpoint.x() - m_widgets.at(i)->x(), qpoint.y() - m_widgets.at(i)->y());
                if (qpoint.x() > m_pages.at(2 * i)->x() &&
                        qpoint.x() <
                        (m_pages.at(2 * i)->width() + m_pages.at(2 * i)->x()) &&
                        qpoint.y() > m_pages.at(2 * i)->y() &&
                        qpoint.y() <
                        (m_pages.at(2 * i)->height() + m_pages.at(2 * i)->y())) {
                    pagenum = 2 * i;
                } else {
                    pagenum = 2 * i + 1;
                    if (pagenum >= m_pages.size())
                        return -1;
                }
                break;
            default:
                break;
            }
            break;
        }
    }
    return pagenum;
}

bool DocummentPDF::mouseBeOverText(QPoint point)
{
    if (!document) {
        return false;
    }
    QPoint qpoint = point;
    int pagenum = -1;
    pagenum = pointInWhichPage(qpoint);
    qDebug() << "mouseBeOverText pagenum:" << pagenum;
    if (-1 != pagenum) {
        PagePdf *ppdf = (PagePdf *)m_pages.at(pagenum);
        return ppdf ->ifMouseMoveOverText(qpoint);
    }
    return false;
}

bool DocummentPDF::getImage(int pagenum, QImage &image, double width, double height)
{
    PagePdf *ppdf = (PagePdf *)m_pages.at(pagenum);
    return ppdf->getImage(image, width, height);
}
int DocummentPDF::getPageSNum()
{
    return m_pages.size();
}

bool DocummentPDF::showMagnifier(QPoint point)
{
    if (!document || !m_magnifierwidget) {
        return false;
    }
    QPoint qpoint = point;
    int pagenum = -1;
    int x_offset = 0;
    int y_offset = 0;
    QScrollBar *scrollBar_X = horizontalScrollBar();
    if (scrollBar_X)
        x_offset = scrollBar_X->value();
    QScrollBar *scrollBar_Y = verticalScrollBar();
    if (scrollBar_Y)
        y_offset = scrollBar_Y->value();
    QPoint gpoint = m_magnifierwidget->mapFromGlobal(mapToGlobal(QPoint(point.x() - x_offset, point.y() - y_offset)));
    pagenum = pointInWhichPage(qpoint);
    qDebug() << "showMagnifier pagenum:" << pagenum;
    if (-1 != pagenum) {
        if (pagenum != m_lastmagnifierpagenum && -1 != m_lastmagnifierpagenum) {
            if (pagenum > m_lastmagnifierpagenum && m_lastmagnifierpagenum - 3 > 0) {
                PagePdf *ppdf = (PagePdf *)m_pages.at(m_lastmagnifierpagenum - 3);
                ppdf->clearMagnifierPixmap();
                if (pagenum - m_lastmagnifierpagenum > 1) {
                    ppdf = (PagePdf *)m_pages.at(m_lastmagnifierpagenum - 2);
                    ppdf->clearMagnifierPixmap();
                }
            } else if (pagenum < m_lastmagnifierpagenum && m_lastmagnifierpagenum + 3 < m_pages.size()) {
                PagePdf *ppdf = (PagePdf *)m_pages.at(m_lastmagnifierpagenum + 3);
                ppdf->clearMagnifierPixmap();
                if ( m_lastmagnifierpagenum - pagenum > 1) {
                    ppdf = (PagePdf *)m_pages.at(m_lastmagnifierpagenum + 2);
                    ppdf->clearMagnifierPixmap();
                }
            }
            for (int i = pagenum - 3; i < pagenum + 4; i++) {
                if (i > 0 && i < m_pages.size()) {
                    PagePdf *ppdf = (PagePdf *)m_pages.at(i);
                    ppdf->loadMagnifierCacheThreadStart(ppdf->width() *m_magnifierwidget->getMagnifierScale(), ppdf->height() *m_magnifierwidget->getMagnifierScale());
                }
            }
        }
        m_lastmagnifierpagenum = pagenum;
        PagePdf *ppdf = (PagePdf *)m_pages.at(pagenum);
        QPixmap pixmap;
        if (ppdf ->getMagnifierPixmap(pixmap, qpoint, m_magnifierwidget->getMagnifierRadius(), ppdf->width() *m_magnifierwidget->getMagnifierScale(), ppdf->height() *m_magnifierwidget->getMagnifierScale())) {
            m_magnifierwidget->setPixmap(pixmap);

            m_magnifierwidget->setPoint(gpoint);
            m_magnifierwidget->show();
            m_magnifierwidget->update();
        }
    } else {
        QPixmap pix(m_magnifierwidget->getMagnifierRadius() * 2, m_magnifierwidget->getMagnifierRadius() * 2);
        pix.fill(Qt::transparent);
        m_magnifierwidget->setPixmap(pix);
        m_magnifierwidget->setPoint(gpoint);
        m_magnifierwidget->show();
        m_magnifierwidget->update();
    }
    int radius = m_magnifierwidget->getMagnifierRadius() - m_magnifierwidget->getMagnifierRingWidth();
    int bigcirclex = gpoint.x() - radius;
    int bigcircley = gpoint.y() - radius;
    if (bigcircley < 0) {

        if (scrollBar_Y)
            scrollBar_Y->setValue(scrollBar_Y->value() + bigcircley);
    } else if (bigcircley > m_magnifierwidget->height() - radius * 2) {
        if (scrollBar_Y)
            scrollBar_Y->setValue(scrollBar_Y->value() + bigcircley - (m_magnifierwidget->height() - radius * 2));
    }
    if (bigcirclex < 0) {
        if (scrollBar_X)
            scrollBar_X->setValue(scrollBar_X->value() + bigcirclex);
    } else if (bigcirclex > m_magnifierwidget->width() - radius * 2) {
        if (scrollBar_X)
            scrollBar_X->setValue(scrollBar_X->value() + bigcirclex - (m_magnifierwidget->width() - radius * 2));
    }
    return false;
}

int DocummentPDF::currentPageNo()
{
    int pagenum = -1;
    int x_offset = 0;
    int y_offset = 0;
    QScrollBar *scrollBar_X = horizontalScrollBar();
    if (scrollBar_X)
        x_offset = scrollBar_X->value();
    QScrollBar *scrollBar_Y = verticalScrollBar();
    if (scrollBar_Y)
        y_offset = scrollBar_Y->value();
    switch (m_viewmode) {
    case ViewMode_SinglePage:
        for (int i = 0; i < m_pages.size(); i++) {
            if (y_offset < m_widgets.at(i)->y() + m_widgets.at(i)->height()) {
                pagenum = i;
                break;
            }
        }
        break;
    case ViewMode_FacingPage:
        for (int i = 0; i < m_pages.size() / 2; i++) {
            if (y_offset < m_widgets.at(i)->y() + m_widgets.at(i)->height()) {
                if (x_offset < m_widgets.at(i)->x() + m_pages.at(i * 2)->x() + m_pages.at(i * 2)->width()) {
                    pagenum = i * 2;
                } else {
                    pagenum = i * 2 + 1;
                }
                break;
            }
        }
        if (-1 == pagenum && m_pages.size() % 2) {
            if (y_offset < m_widgets.at(m_pages.size() / 2)->y() + m_widgets.at(m_pages.size() / 2)->height()) {
                if (x_offset < m_widgets.at(m_pages.size() / 2)->x() + m_pages.at(m_pages.size() - 1)->x() + m_pages.at(m_pages.size() - 1)->width()) {
                    pagenum = m_pages.size() - 1;
                } else {
                    pagenum = m_pages.size();
                }
                break;
            }
        }
        break;
    default:
        break;
    }
    return pagenum;
}

bool DocummentPDF::pageJump(int pagenum)
{
    if (pagenum < 0 || pagenum > m_pages.size())
        return false;
    QScrollBar *scrollBar_X = horizontalScrollBar();
    QScrollBar *scrollBar_Y = verticalScrollBar();
    switch (m_viewmode) {
    case ViewMode_SinglePage:
        if (scrollBar_X)
            scrollBar_X->setValue(m_widgets.at(pagenum)->x());
        if (scrollBar_Y)
            scrollBar_Y->setValue(m_widgets.at(pagenum)->y());
        break;
    case ViewMode_FacingPage:
        if (scrollBar_X)
            scrollBar_X->setValue(m_widgets.at(pagenum / 2)->x() + m_pages.at(pagenum)->x());
        if (scrollBar_Y)
            scrollBar_Y->setValue(m_widgets.at(pagenum / 2)->y());
        break;
    default:
        break;
    }
    return true;
}

void DocummentPDF::docBasicInfo(stFileInfo &info)
{
    info=m_fileinfo;
}

bool DocummentPDF::annotationClicked(const QPoint &pos, QString &strtext)
{
    QPoint pt(pos);
    return static_cast<PagePdf>(m_pages.at(pointInWhichPage(pt))).annotationClicked(pt,strtext);
}

void DocummentPDF::title(QString &title)
{
    title=document->title();
}

void DocummentPDF::slot_vScrollBarValueChanged(int value)
{
    qDebug() << "slot_vScrollBarValueChanged" << value;
    int pageno = currentPageNo();
    if (m_currentpageno != pageno) {
        m_currentpageno = pageno;
        emit signal_pageChange(m_currentpageno);
    }
    if (!donotneedreloaddoc) {
        if (m_threadloaddoc.isRunning())
            m_threadloaddoc.setRestart();
        else
            m_threadloaddoc.start();
    }
}

void DocummentPDF::slot_hScrollBarValueChanged(int value)
{
    qDebug() << "slot_hScrollBarValueChanged" << value;
    int pageno = currentPageNo();
    if (m_currentpageno != pageno) {
        m_currentpageno = pageno;
    }
    if (!donotneedreloaddoc) {
        if (m_threadloaddoc.isRunning())
            m_threadloaddoc.setRestart();
        else
            m_threadloaddoc.start();
    }
}

Page::Link *DocummentPDF::mouseBeOverLink(QPoint point)
{
    if (!document) {
        return nullptr;
    }
    QPoint qpoint = point;
    int pagenum = -1;
    pagenum = pointInWhichPage(qpoint);
    qDebug() << "mouseBeOverLink pagenum:" << pagenum;
    if (-1 != pagenum) {
        PagePdf *ppdf = (PagePdf *)m_pages.at(pagenum);
        return ppdf ->ifMouseMoveOverLink(qpoint);
    }
    return nullptr;
}
