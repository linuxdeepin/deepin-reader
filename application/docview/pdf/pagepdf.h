#ifndef PAGEPDF_H
#define PAGEPDF_H
#include "../pagebase.h"
#include <QImage>
#include <QDebug>
#include <QtConcurrent>
#include <QThread>
#include <QPaintDevice>
#include <poppler-qt5.h>
#include <unistd.h>
#include <QObject>

class PagePdf;
class PagePdfPrivate;
class PagePdf: public PageBase
{
    Q_OBJECT
public:
    PagePdf(QWidget *parent = nullptr);
    ~PagePdf() override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
    void setPage(Poppler::Page *page, int pageno);
    //Annotation
    QString addAnnotation(const QColor &color);
    QString addAnnotation(const QColor &color,const QRect& rect);
    QString removeAnnotation(const QPoint &pos);
    void removeAnnotation(const QString &struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    Poppler::Page *GetPage();
    stSearchRes search(const QString &text, bool matchCase, bool wholeWords) override;

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    void removeAnnotation(Poppler::Annotation *annotation);
    QString addHighlightAnnotation(const QColor &color);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PagePdf)
};
class PagePdfPrivate: public PageBasePrivate, public  PageInterface
{
    Q_OBJECT
public:
    PagePdfPrivate(PagePdf *parent): PageBasePrivate(parent)
    {
        m_page = nullptr;
    }

    ~PagePdfPrivate() override
    {
        //        qDebug() << "~PagePdfPrivate";
        if (m_page) {
            delete m_page;
            m_page = nullptr;
        }
    }
    bool getImage(QImage &image, double width, double height) override
    {
//        QMutexLocker locker(&m_mutexlockgetimage);
        int xres = 72.0, yres = 72.0;
        double scalex = width / m_imagewidth;
        double scaley = height / m_imageheight;
        if (!m_page)
            return false;
        image = m_page->renderToImage(xres * scalex, yres * scaley);
        return true;
    }
    bool getSlideImage(QImage &image, double &width, double &height) override
    {
        int xres = 72.0, yres = 72.0;
        double scalex = (width - 20) / m_imagewidth;
        double scaley = (height - 20) / m_imageheight;
        double scale = 1;
        if (scalex > scaley) {
            scale = scaley;
        } else {
            scale = scalex;
        }
        width = m_imagewidth * scale;
        height = m_imageheight * scale;
        if (!m_page)
            return false;
        qDebug() << "getSlideImage width:" << width << " height:" << height;
        image = m_page->renderToImage(xres * scale * pixelratiof, yres * scale * pixelratiof);
        image.setDevicePixelRatio(pixelratiof);
        return true;
    }
    bool loadData() override
    {
        loadWords();
        loadLinks();
        return true;
    }

    void setPage(Poppler::Page *page, int pageno)
    {
        m_page = page;
        m_pageno = pageno;
        QSizeF qsize = m_page->pageSizeF();
        m_imagewidth = qsize.width();
        m_imageheight = qsize.height();
    }

    Poppler::Page *m_page;
private:
    bool loadWords()
    {
        if (!m_page) {
            return false;
        }
        QList<Poppler::TextBox *> textList = m_page->textList();
        abstractTextPage(textList);
        qDeleteAll(textList);
        return true;
    }
    bool loadLinks()
    {
        qDeleteAll(m_links);
        m_links.clear();
        if (!m_page) {
            return false;
        }
        QList<Poppler::Link *> qlinks = m_page->links();
        foreach (const Poppler::Link *link, qlinks) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                break;
            }
            const QRectF boundary = link->linkArea().normalized();
            //        qDebug() << "boundary:" << boundary;

            if (link->linkType() == Poppler::Link::Goto) {
                const Poppler::LinkGoto *linkGoto = static_cast< const Poppler::LinkGoto * >(link);

                int page = linkGoto->destination().pageNumber();
                qreal left = qQNaN();
                qreal top = qQNaN();

                page = page >= 1 ? page : 1;

                if (linkGoto->destination().isChangeLeft()) {
                    left = linkGoto->destination().left();

                    left = left >= 0.0 ? left : 0.0;
                    left = left <= 1.0 ? left : 1.0;
                }

                if (linkGoto->destination().isChangeTop()) {
                    top = linkGoto->destination().top();

                    top = top >= 0.0 ? top : 0.0;
                    top = top <= 1.0 ? top : 1.0;
                }

                if (linkGoto->isExternal()) {
                    //                qDebug() << "isExternal filename:" << linkGoto->fileName() << " page:" << page;
                    m_links.append(new Page::Link(boundary, linkGoto->fileName(), page));
                } else {
                    //                qDebug() << "unExternal left:" << left << " top:" << top << " page:" << page;
                    m_links.append(new Page::Link(boundary, page, left, top));
                }
            } else if (link->linkType() == Poppler::Link::Browse) {
                const Poppler::LinkBrowse *linkBrowse = static_cast< const Poppler::LinkBrowse * >(link);
                const QString url = linkBrowse->url();

                m_links.append(new Page::Link(boundary, url));
            } else if (link->linkType() == Poppler::Link::Execute) {
                const Poppler::LinkExecute *linkExecute = static_cast< const Poppler::LinkExecute * >(link);
                const QString url = linkExecute->fileName();

                m_links.append(new Page::Link(boundary, url, Page::LinkType_Execute));
            }
        }
        qDeleteAll(qlinks);
        return true;
    }
    bool abstractTextPage(const QList<Poppler::TextBox *> &text)
    {
        Poppler::TextBox *next;
        QString s;
        bool addChar;
        m_words.clear();
        foreach (Poppler::TextBox *word, text) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                break;
            }
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
                    m_words.append(sword);
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
                m_words.append(sword);
            }
        }
        return true;
    }
};


#endif // PAGEPDF_H
