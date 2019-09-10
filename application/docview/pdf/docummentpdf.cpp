#include "docummentpdf.h"
#include "pagepdf.h"
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QScrollBar>


ThreadLoadDoc::ThreadLoadDoc()
{
    m_doc = nullptr;
    brun = false;
}

void ThreadLoadDoc::setDoc(DocummentPDF *doc)
{
    m_doc = doc;
}

void ThreadLoadDoc::run()
{
    if (brun) {
        this->terminate();
        this->wait();
        brun = false;
    }
    if (!m_doc)
        return;
    brun = true;
    m_doc->loadPages();
    brun = false;
}


ThreadLoadWords::ThreadLoadWords()
{
    m_doc = nullptr;
    brun = false;
}

void ThreadLoadWords::setDoc(DocummentPDF *doc)
{
    m_doc = doc;
}

void ThreadLoadWords::run()
{
    if (brun) {
        this->terminate();
        this->wait();
        brun = false;
    }
    if (!m_doc)
        return;
    brun = true;
    m_doc->loadWords();
    brun = false;
}


DocummentPDF::DocummentPDF(QWidget *parent): DocummentBase(parent), document(nullptr)
{
    m_threadloaddoc.setDoc(this);
    m_threadloadwords.setDoc(this);
    setWidgetResizable(true);
    setWidget(&m_widget);
    m_scale = 1;
    m_rotate = RotateType_Normal;
    pblankwidget = new QWidget(this);
    pblankwidget->setMouseTracking(true);
    pblankwidget->hide();
}

bool DocummentPDF::openFile(QString filepath)
{
    document = Poppler::Document::load(filepath);
    m_pages.clear();
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
    setViewModeAndShow(m_viewmode);
    m_threadloaddoc.start();
    m_threadloadwords.start();
    return true;
}

bool DocummentPDF::setViewModeAndShow(ViewMode_EM viewmode)
{
    m_viewmode = viewmode;
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
    for (int i = 0; i < m_pages.size(); i++) {
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
        qDebug() << "i:" << i << " width:" << m_pages.at(i)->width() << " height:" << m_pages.at(i)->height();
        loadWordCache(i, m_pages.at(i));
    }
    return true;
}

void DocummentPDF::scaleAndShow(double scale, RotateType_EM rotate)
{
    m_scale = scale;
    m_rotate = rotate;
    m_threadloaddoc.start();
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
    QScrollBar *scrollBar_X = horizontalScrollBar();
    const int x_offset = scrollBar_X->value();
    QScrollBar *scrollBar_Y = verticalScrollBar();
    const int y_offset = scrollBar_Y->value();
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

    qDebug() << "startpagenum:" << startpagenum << " endpagenum:" << endpagenum;
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
    qDebug() << "startpagenum:" << startpagenum << " endpagenum:" << endpagenum;
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
bool DocummentPDF::mouseBeOverText(QPoint point)
{
    if (!document) {
        return false;
    }
    QPoint qpoint = point;
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
                        return false;
                }
                break;
            default:
                break;
            }
            break;
        }
    }
    qDebug() << "pagenum:" << pagenum;
    if (-1 != pagenum) {
        PagePdf *ppdf = (PagePdf *)m_pages.at(pagenum);
        return ppdf ->ifMouseMoveOverText(qpoint);
    }
    return false;
}

bool DocummentPDF::getImage(int pagenum, QImage &image, double width, double height, RotateType_EM rotate)
{
    PagePdf *ppdf = (PagePdf *)m_pages.at(pagenum);
    return ppdf->getImage(image, width, height, rotate);
}
int DocummentPDF::getPageSNum()
{
    return m_pages.size();
}
