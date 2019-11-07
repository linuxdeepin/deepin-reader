#include "pagexps.h"
#include "docview/publicfunc.h"
#include <QQueue>
#include <QDebug>
class PageXPSPrivate: public PageBasePrivate, public  PageInterface
{
//    Q_OBJECT
public:
    PageXPSPrivate(PageXPS *parent): PageBasePrivate(parent),
        m_page(nullptr)
    {
    }

    ~PageXPSPrivate() override
    {
        if (m_page) {
            delete m_page;
            m_page = nullptr;
        }
    }
    bool getImage(QImage &image, double width, double height) override
    {
        if (!m_page)
            return false;
        image = QImage(QSize(width, height), QImage::Format_ARGB32);
        m_page->renderToImage(&image);
        return true;
    }
    bool getSlideImage(QImage &image, double &width, double &height) override
    {
        if (!m_page)
            return false;

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
        image = QImage(QSize(width, height), QImage::Format_ARGB32);
        m_page->renderToImage(&image);
        return true;
    }
    bool loadData() override
    {
        loadWords();
        return true;
    }

    void setPage(XpsPage *page, int pageno)
    {
        m_page = page;
        m_pageno = pageno;
        m_imagewidth = page->size().width();
        m_imageheight = page->size().height();
    }

    XpsPage *m_page;
private:

    bool loadWords()
    {
        if (!m_page)
            return false;
        m_page->loadPageWords(&m_words);
        return true;
    }
};

PageXPS::PageXPS(QWidget *parent)
    : PageBase(new PageXPSPrivate(this), parent)
{
}

void PageXPS::setPage(XpsPage *page, int pageno)
{
    Q_D(PageXPS);
    d->setPage(page, pageno);
}

bool PageXPS::getSlideImage(QImage &image, double &width, double &height)
{

    Q_D(PageXPS);
    return d->getSlideImage(image, width, height);
}

bool PageXPS::getImage(QImage &image, double width, double height)
{
    Q_D(PageXPS);
    return d->getImage(image, width, height);
}

PageInterface *PageXPS::getInterFace()
{
    Q_D(PageXPS);
    return d;
}

