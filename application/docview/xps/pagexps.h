#ifndef PAGEXPS_H
#define PAGEXPS_H
#include "../pagebase.h"
#include "xpsapi.h"
#include <QImage>
#include <QThread>
class PageXPS;
class PageXPSPrivate;

class PageXPSPrivate: public PageBasePrivate, public  PageInterface
{
    Q_OBJECT
public:
    PageXPSPrivate(PageXPS *parent): PageBasePrivate((PageBase *)parent)
    {
        m_page = nullptr;
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
        int xres = 72.0, yres = 72.0;
        double scalex = width / m_imagewidth;
        double scaley = height / m_imageheight;
        if (!m_page)
            return false;
        image = m_page->renderToImage(xres * scalex, yres * scaley, -1, -1, width, height);
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
        image = m_page->renderToImage(xres * scale, yres * scale, -1, -1, width, height);
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

    XpsPage *m_page;
};
class PageXPS: public PageBase
{
    Q_OBJECT
public:
    PageXPS(QWidget *parent = 0);
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    bool loadWords() override;
    bool loadLinks() override;
    void setPage(XpsPage *page, int pageno);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PageXPS)
};

#endif // PAGEXPS_H
