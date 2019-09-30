#ifndef PAGEPS_H
#define PAGEPS_H
#include "../pagebase.h"
#include <libspectre/spectre-document.h>
#include <QImage>
#include <QThread>

class DocummentPS;

class PagePS;
class PagePSPrivate;
class PagePSPrivate: public PageBasePrivate, public  PageInterface
{
    Q_OBJECT
public:
    PagePSPrivate(PagePS *parent): PageBasePrivate((PageBase *)parent),
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

class PagePS: public PageBase
{
    Q_OBJECT
public:
    PagePS(QWidget *parent = 0);
//    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
//    bool loadWords() override;
//    bool loadLinks() override;
    void setPage(SpectrePage *page, SpectreRenderContext *renderContext, int pageno);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PagePS)
};

#endif // PAGEPS_H
