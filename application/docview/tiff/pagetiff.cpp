#include "pagetiff.h"



static void adaptSizeToResolution( TIFF *tiff, ttag_t whichres, double dpi, uint32 *size )
{
    float resvalue = 1.0;
    uint16 resunit = 0;
    if ( !TIFFGetField( tiff, whichres, &resvalue )
         || !TIFFGetFieldDefaulted( tiff, TIFFTAG_RESOLUTIONUNIT, &resunit ) )
        return;

    float newsize = *size / resvalue;
    switch ( resunit )
    {
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

PageTiff::PageTiff(QWidget *parent): PageBase(parent)
  ,m_document()
  ,m_pageno()
{

}

bool PageTiff::showImage(double scale, RotateType_EM rotate)
{
    if (!m_document)
        return false;
    m_scale = scale;
    m_rotate = rotate;
    uint32 width;
    uint32 height;

    QPixmap map;
    QImage image( m_width, m_height, QImage::Format_RGB32 );
    if(TIFFSetDirectory(m_document,m_pageno))
    {
        uint32 * data = (uint32 *)image.bits();

        if ( TIFFReadRGBAImageOriented(m_document, m_width, m_height, data, ORIENTATION_TOPLEFT ) != 0 )
        {
            // an image read by ReadRGBAImage is ABGR, we need ARGB, so swap red and blue
            uint32 size = m_width * m_height;
            for ( uint32 i = 0; i < size; ++i )
            {
                uint32 red = ( data[i] & 0x00FF0000 ) >> 16;
                uint32 blue = ( data[i] & 0x000000FF ) << 16;
                data[i] = ( data[i] & 0xFF00FF00 ) + red + blue;
            }
            map= QPixmap::fromImage(image);
        }
    }
    else {
        QImage imageerror(":/resources/image/logo/logo_big.svg");//显示错误图片便于排查
         map= QPixmap::fromImage(imageerror);
    }
    QMatrix leftmatrix;
    switch (m_rotate) {
    case RotateType_90:
        leftmatrix.rotate(90);
        break;
    case RotateType_180:
        leftmatrix.rotate(180);
        break;
    case RotateType_270:
        leftmatrix.rotate(270);
        break;
    default:
        leftmatrix.rotate(0);
        break;
    }
    map=map.transformed(leftmatrix, Qt::SmoothTransformation);
    setPixmap(map);
    return true;
}

void PageTiff::setPage(int pageno, TIFF *document)
{
    m_document=document;
    m_pageno=pageno;
    uint32 width = 0;
    uint32 height = 0;
    const QSizeF dpi(76,76);
    if (TIFFSetDirectory(m_document,m_pageno) )
    {
        if ( TIFFGetField(m_document, TIFFTAG_IMAGEWIDTH, &width ) != 1 ||
             TIFFGetField(m_document, TIFFTAG_IMAGELENGTH, &height ) != 1 )
        {
            adaptSizeToResolution(m_document, TIFFTAG_XRESOLUTION, dpi.width(), &width );
            adaptSizeToResolution(m_document, TIFFTAG_YRESOLUTION, dpi.height(), &height );
        }
    }
    m_imagewidth=m_width=width;
    m_imageheight=m_height=height;
}

