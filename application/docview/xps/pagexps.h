#ifndef PAGEXPS_H
#define PAGEXPS_H
#include "../pagebase.h"
#include "xpsapi.h"
#include <QImage>
#include <QThread>
class PageXPS;
class PageXPSPrivate;
class PageXPS: public PageBase
{
    Q_OBJECT
public:
    PageXPS(QWidget *parent = nullptr);
//    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
//    bool loadWords() override;
//    bool loadLinks() override;
    void setPage(XpsPage *page, int pageno);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PageXPS)
};
class PageXPSPrivate: public PageBasePrivate, public  PageInterface
{
    Q_OBJECT
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


#endif // PAGEXPS_H
