#include "pageps.h"
#include "docview/publicfunc.h"
#include "docummentps.h"
#include <QQueue>
#include <QDebug>
class PagePSPrivate: public PageBasePrivate, public  PageInterface
{
//    Q_OBJECT
public:
    PagePSPrivate(PagePS *parent): PageBasePrivate(parent),
        m_page(nullptr),
        m_renderContext(nullptr)
    {
    }

    ~PagePSPrivate() override
    {
        if (m_page) {
            spectre_page_free(m_page);
            m_page = nullptr;
        }
    }
    bool getImage(QImage &image, double width, double height) override
    {
        if (!m_page)
            return false;


        double scalex = width / m_imagewidth;
        double scaley = height / m_imageheight;
        spectre_render_context_set_scale(m_renderContext, scalex, scaley);
        unsigned char *pageData = nullptr;
        int rowLength = 0;

        spectre_page_render(m_page, m_renderContext, &pageData, &rowLength);

        if (spectre_page_status(m_page) != SPECTRE_STATUS_SUCCESS) {
            free(pageData);
            pageData = nullptr;

            return false;
        }

        QImage auxiliaryImage(pageData, rowLength / 4, height, QImage::Format_RGB32);
        image = QImage(auxiliaryImage.copy(0, 0, width, height));

        free(pageData);
        pageData = nullptr;
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
        spectre_render_context_set_scale(m_renderContext, scale, scale);
        unsigned char *pageData = nullptr;
        int rowLength = 0;

        spectre_page_render(m_page, m_renderContext, &pageData, &rowLength);

        if (spectre_page_status(m_page) != SPECTRE_STATUS_SUCCESS) {
            free(pageData);
            pageData = nullptr;

            return false;
        }

        QImage auxiliaryImage(pageData, rowLength / 4, height, QImage::Format_RGB32);
        image = QImage(auxiliaryImage.copy(0, 0, width, height));

        free(pageData);
        pageData = nullptr;
        return true;
    }
    bool loadData() override
    {
        return true;
    }

    void setPage(SpectrePage *page, SpectreRenderContext *renderContext, int pageno)
    {
        m_page = page;
        m_pageno = pageno;
        m_renderContext = renderContext;
        int w, h;
        spectre_page_get_size(m_page, &w, &h);
        m_imagewidth = w;
        m_imageheight = h;
    }

    SpectrePage *m_page;
    SpectreRenderContext *m_renderContext;
};

PagePS::PagePS(QWidget *parent)
    : PageBase(new PagePSPrivate(this), parent)
{
}

void PagePS::setPage(SpectrePage *page, SpectreRenderContext *renderContext, int pageno)
{
    Q_D(PagePS);
    d->setPage(page, renderContext, pageno);
}

bool PagePS::getSlideImage(QImage &image, double &width, double &height)
{
    Q_D(PagePS);
    return d->getSlideImage(image, width, height);
}

bool PagePS::getImage(QImage &image, double width, double height)
{
    Q_D(PagePS);
    return d->getImage(image, width, height);
}

PageInterface *PagePS::getInterFace()
{
    Q_D(PagePS);
    return d;
}

