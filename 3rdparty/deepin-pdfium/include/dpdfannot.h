#ifndef DPDFANNOT_H
#define DPDFANNOT_H

#include "dpdfglobal.h"

#include <QRectF>
#include <QString>
#include <QList>
#include <QColor>

class DEEPIN_PDFIUM_EXPORT DPdfAnnot
{
public:
    enum AnnotType {
        AUnknown = 0,         ///< 前期支持以外的
        AText = 1,            ///< TextAnnotation
        AHighlight = 2,       ///< HighlightAnnotation
        ALink = 3
    };

    virtual ~DPdfAnnot();
    /**
     * @brief 是否在该点上
     * @param pos
     * @return
     */
    virtual bool pointIn(QPointF pos) = 0;

    virtual QList<QRectF> boundaries() = 0;

    AnnotType type();

    void setText(QString text);

    QString text();

protected:
    AnnotType m_type;
    QString m_text;
};

class DEEPIN_PDFIUM_EXPORT DPdfTextAnnot : public DPdfAnnot
{
    friend class DPdfPage;
    friend class DPdfPagePrivate;
public:
    DPdfTextAnnot();

    bool pointIn(QPointF pos) override;

    QList<QRectF> boundaries() override;

    void setRectF(const QRectF &rectf);

private:
    QRectF m_rect;
};

class DEEPIN_PDFIUM_EXPORT DPdfHightLightAnnot : public DPdfAnnot
{
    friend class DPdfPage;
    friend class DPdfPagePrivate;
public:
    DPdfHightLightAnnot();

    bool pointIn(QPointF pos) override;

    void setColor(QColor color);

    QColor color();

    void setBoundaries(QList<QRectF> rectList);

    QList<QRectF> boundaries() override;

private:
    QList<QRectF> m_rectList;
    QColor m_color;
};

class DEEPIN_PDFIUM_EXPORT DPdfUnknownAnnot : public DPdfAnnot
{
public:
    DPdfUnknownAnnot();

    bool pointIn(QPointF pos) override;

    QList<QRectF> boundaries() override;

};
#endif // DPDFANNOT_H
