#include "pagetiff.h"

static void adaptSizeToResolution( TIFF *tiff, ttag_t whichres, double dpi, uint32 *size )
{
    float resvalue = 1.0;
    uint16 resunit = 0;
    if ( !TIFFGetField( tiff, whichres, &resvalue )
            || !TIFFGetFieldDefaulted( tiff, TIFFTAG_RESOLUTIONUNIT, &resunit ) )
        return;

    float newsize = *size / resvalue;
    switch ( resunit ) {
    case RESUNIT_INCH:
        *size = (uint32)( newsize * dpi );
        break;
    case RESUNIT_CENTIMETER:
        *size = (uint32)( newsize * 10.0 / 25.4 * dpi );
        break;
    case RESUNIT_NONE:
        break;
    }
}

class PageTiffPrivate: public PageBasePrivate, public  PageInterface
{
//    Q_OBJECT
public:
    PageTiffPrivate(PageTiff *parent): PageBasePrivate(parent),
        m_document(nullptr)
    {
    }

    ~PageTiffPrivate() override
    {
    }
    bool getImage(QImage &image, double width, double height) override
    {
        if (!m_document)
            return false;

        bool generated = false;
//        QImage img;

        if (TIFFSetDirectory(m_document, m_pageno)) {

            image = QImage( m_imagewidth, m_imageheight, QImage::Format_RGB32 );
            uint32 *data = (uint32 *)image.bits();

            // read data
            if ( TIFFReadRGBAImageOriented( m_document, m_imagewidth, m_imageheight, data, ORIENTATION_TOPLEFT ) != 0 ) {
                // an image read by ReadRGBAImage is ABGR, we need ARGB, so swap red and blue
                uint32 size = m_imagewidth * m_imageheight;
                for ( uint32 i = 0; i < size; ++i ) {
                    uint32 red = ( data[i] & 0x00FF0000 ) >> 16;
                    uint32 blue = ( data[i] & 0x000000FF ) << 16;
                    data[i] = ( data[i] & 0xFF00FF00 ) + red + blue;
                }
                image = image.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

                generated = true;
            }
        }

        if ( !generated ) {
            image = QImage(":/resources/image/logo/logo_big.svg");
            image = image.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
//            image = QImage( width, height, QImage::Format_RGB32 );
//            image.fill( qRgb( 255, 255, 255 ) );
        }
        return true;
    }
    bool getSlideImage(QImage &image, double &width, double &height) override
    {
        if (!m_document)
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

        bool generated = false;

        if (TIFFSetDirectory(m_document, m_pageno)) {

            image = QImage( m_imagewidth, m_imageheight, QImage::Format_RGB32 );
            uint32 *data = (uint32 *)image.bits();

            // read data
            if ( TIFFReadRGBAImageOriented( m_document, m_imagewidth, m_imageheight, data, ORIENTATION_TOPLEFT ) != 0 ) {
                // an image read by ReadRGBAImage is ABGR, we need ARGB, so swap red and blue
                uint32 size = m_imagewidth * m_imageheight;
                for ( uint32 i = 0; i < size; ++i ) {
                    uint32 red = ( data[i] & 0x00FF0000 ) >> 16;
                    uint32 blue = ( data[i] & 0x000000FF ) << 16;
                    data[i] = ( data[i] & 0xFF00FF00 ) + red + blue;
                }
                image = image.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

                generated = true;
            }
        }

        if ( !generated ) {
            image = QImage(":/resources/image/logo/logo_big.svg");
            image = image.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
//            image = QImage( width, height, QImage::Format_RGB32 );
//            image.fill( qRgb( 255, 255, 255 ) );
        }
        return true;
    }
    bool loadData() override
    {
        return true;
    }

    void setPage(int pageno, TIFF *document)
    {
        m_document = document;
        m_pageno = pageno;
        uint32 width = 0;
        uint32 height = 0;
        const QSizeF dpi(76, 76);
        if (TIFFSetDirectory(m_document, m_pageno) ) {
            if ( TIFFGetField(m_document, TIFFTAG_IMAGEWIDTH, &width ) != 1 ||
                    TIFFGetField(m_document, TIFFTAG_IMAGELENGTH, &height ) != 1 ) {
                adaptSizeToResolution(m_document, TIFFTAG_XRESOLUTION, dpi.width(), &width );
                adaptSizeToResolution(m_document, TIFFTAG_YRESOLUTION, dpi.height(), &height );
            }
        }
        m_imagewidth = width;
        m_imageheight = height;
    }

    TIFF *m_document;
//    uint32 m_width;
//    uint32 m_height;
    QImage m_image;
};


PageTiff::PageTiff(QWidget *parent):
    PageBase(new PageTiffPrivate(this), parent)
{

}

void PageTiff::setPage(int pageno, TIFF *document)
{
    Q_D(PageTiff);
    d->setPage(pageno, document);
}

bool PageTiff::getSlideImage(QImage &image, double &width, double &height)
{
    Q_D(PageTiff);
    return d->getSlideImage(image, width, height);
}

bool PageTiff::getImage(QImage &image, double width, double height)
{
    Q_D(PageTiff);
    return d->getImage(image, width, height);
}

PageInterface *PageTiff::getInterFace()
{
    Q_D(PageTiff);
    return d;
}

