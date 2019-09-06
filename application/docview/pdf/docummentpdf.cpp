#include "docummentpdf.h"
#include "pagepdf.h"
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QScrollBar>


ThreadLoadDoc::ThreadLoadDoc()
{
    bStop = false;
    m_doc = nullptr;
}

void ThreadLoadDoc::setDoc(DocummentPDF *doc)
{
    m_doc = doc;
}

void ThreadLoadDoc::closeThread()
{
    bStop = true;
}

void ThreadLoadDoc::run()
{
    if (!m_doc)
        return;
    //while (!bStop) {
    m_doc->loadPages();
    //}
}

DocummentPDF::DocummentPDF(QWidget *parent): DocummentBase(parent), document(nullptr)
{
    m_threadloaddoc.setDoc(this);
}

bool DocummentPDF::openFile(QString filepath)
{
    document = Poppler::Document::load(filepath);
    m_pages.clear();
    qDebug() << "numPages :" << document->numPages();
    for (int i = 0; i < document->numPages(); i++) {
        PageBase *page = new PagePdf(this);
        m_pages.append(page);
    }
    m_threadloaddoc.start();
    return true;
}

bool DocummentPDF::loadPages()
{
    if (!document && m_pages.size() == document->numPages())
        return false;
    qDebug() << "loadPages";
    for (int i = 0; i < document->numPages(); i++) {
//        PageBase *page = new PagePdf(this);
        QImage image = document->page(i)->renderToImage(216, 216);  //截取pdf文件中的相应图片
        m_pages.at(i)->setPixmap(QPixmap::fromImage(image));  //将该图片放进label中
        m_vboxLayout.addWidget(m_pages.at(i));
        m_vboxLayout.setAlignment(&m_widget, Qt::AlignCenter);
//        m_pages.append(page);
    }
    setWidget(&m_widget);
    for (int i = 0; i < m_pages.size(); i++) { //根据获取到的pdf页数循环
        qDebug() << "i:" << i;
        loadWordCache(i, m_pages.at(i));
    }
    return true;
}

void DocummentPDF::loadWordCache(int indexpage, PageBase *page)
{
    if (!document) {
        return;
    }
    double pageWidth = 0, pageHeight = 0;
    Poppler::Page *pp = document->page(indexpage);
    QList<Poppler::TextBox *> textList;
    pp->textList();
    if (pp) {
        textList = pp->textList();
        const QSizeF s = pp->pageSizeF();
        pageWidth = s.width();
        pageHeight = s.height();
    }
    delete pp;

    abstractTextPage(textList, pageHeight, pageWidth, page);
    qDeleteAll(textList);
}

bool DocummentPDF::abstractTextPage(const QList<Poppler::TextBox *> &text, double height,
                                    double width, PageBase *page)
{
    // qDebug() << "abstractTextPage";
    Poppler::TextBox *next;
    PagePdf *ppdf = (PagePdf *)page;
    ppdf->setImageWidth(width);
    ppdf->setImageHeight(height);
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

bool DocummentPDF::mouseSelectText(QPoint start, QPoint stop)
{
    if (!document) {
        return false;
    }
    QScrollBar *scrollBar_X = horizontalScrollBar();
    const int x_offset = scrollBar_X->value();
    QScrollBar *scrollBar_Y = verticalScrollBar();
    const int y_offset = scrollBar_Y->value();
    QPoint qstart = QPoint(mapFromGlobal(start).x() + x_offset,
                           mapFromGlobal(start).y() + y_offset);
    QPoint qstop = QPoint(mapFromGlobal(stop).x() + x_offset,
                          mapFromGlobal(stop).y() + y_offset);
    int startpagenum = -1, endpagenum = -1;
    for (int i = 0; i < m_pages.size(); i++) {
        if (qstop.x() > m_pages.at(i)->x() &&
                qstop.x() <
                (m_pages.at(i)->width() + m_pages.at(i)->x()) &&
                qstop.y() > m_pages.at(i)->y() &&
                qstop.y() <
                (m_pages.at(i)->height() + m_pages.at(i)->y())) {
            endpagenum = i;
            break;
        }
    }
    for (int i = 0; i < m_pages.size(); i++) {
        if (qstart.x() > m_pages.at(i)->x() &&
                qstart.x() <
                (m_pages.at(i)->width() + m_pages.at(i)->x()) &&
                qstart.y() > m_pages.at(i)->y() &&
                qstart.y() <
                (m_pages.at(i)->height() + m_pages.at(i)->y())) {
            startpagenum = i;
            break;
        }
    }

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
        if (i == startpagenum) {
            if (startpagenum == endpagenum) {
                re = ppdf->pageTextSelections(qstart, qstop);
            } else {
                re = ppdf->pageTextSelections(qstart,
                                              QPoint(m_pages.at(i)->width() + m_pages.at(i)->x(),
                                                     m_pages.at(i)->height() + m_pages.at(i)->y()));
            }
        } else if (i == endpagenum) {
            re = ppdf->pageTextSelections(
                     QPoint(m_pages.at(i)->x(), m_pages.at(i)->y()),
                     qstop);
        } else {
            re = ppdf->pageTextSelections(QPoint(m_pages.at(i)->x(), m_pages.at(i)->y()),
                                          QPoint(m_pages.at(i)->width() + m_pages.at(i)->x(),
                                                 m_pages.at(i)->height() + m_pages.at(i)->y()));
        }
    }
    return re;
}
bool DocummentPDF::mouseBeOverText(QPoint point)
{
    if (!document) {
        return false;
    }
    QScrollBar *scrollBar_X = horizontalScrollBar();
    const int x_offset = scrollBar_X->value();
    QScrollBar *scrollBar_Y = verticalScrollBar();
    const int y_offset = scrollBar_Y->value();
    QPoint qpoint = QPoint(mapFromGlobal(point).x() + x_offset,
                           mapFromGlobal(point).y() + y_offset);
    int pagenum = -1;
    for (int i = 0; i < m_pages.size(); i++) {
        if (qpoint.x() > m_pages.at(i)->x() &&
                qpoint.x() <
                (m_pages.at(i)->width() + m_pages.at(i)->x()) &&
                qpoint.y() > m_pages.at(i)->y() &&
                qpoint.y() <
                (m_pages.at(i)->height() + m_pages.at(i)->y())) {
            pagenum = i;
            break;
        }
    }
    if (-1 != pagenum) {
        PagePdf *ppdf = (PagePdf *)m_pages.at(pagenum);
        return ppdf ->ifMouseMoveOverText(qpoint);
    }
    return false;
}
