#include "dpdfannot.h"

#include "public/fpdf_annot.h"

DPdfAnnot::AnnotType DPdfAnnot::type()
{
    return m_type;
}

void DPdfAnnot::setText(QString text)
{
    m_text = text;
}

QString DPdfAnnot::text()
{
    return m_text;
}

DPdfTextAnnot::DPdfTextAnnot()
{
    m_type = AText;
}

bool DPdfTextAnnot::pointIn(QPointF pos)
{
    return m_rect.contains(pos);
}

QList<QRectF> DPdfTextAnnot::boundaries()
{
    QList<QRectF> list;
    list << m_rect;

    return list;
}

void DPdfTextAnnot::setRectF(const QRectF &rectf)
{
    m_rect = rectf;
}

DPdfHightLightAnnot::DPdfHightLightAnnot()
{
    m_type = AHighlight;
}

bool DPdfHightLightAnnot::pointIn(QPointF pos)
{
    for (QRectF rect : m_rectList) {
        if (rect.contains(pos))
            return true;
    }

    return false;
}

void DPdfHightLightAnnot::setColor(QColor color)
{
    m_color = color;
}

QColor DPdfHightLightAnnot::color()
{
    return m_color;
}

void DPdfHightLightAnnot::setBoundaries(QList<QRectF> rectList)
{
    m_rectList = rectList;
}

QList<QRectF> DPdfHightLightAnnot::boundaries()
{
    return m_rectList;
}

DPdfAnnot::~DPdfAnnot()
{

}

DPdfUnknownAnnot::DPdfUnknownAnnot()
{
    m_type = AUnknown;
}

bool DPdfUnknownAnnot::pointIn(QPointF pos)
{
    Q_UNUSED(pos)
    return false;
}

QList<QRectF> DPdfUnknownAnnot::boundaries()
{
    return QList<QRectF>();
}
