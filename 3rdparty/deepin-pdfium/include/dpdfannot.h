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

class DEEPIN_PDFIUM_EXPORT DPdfLinkAnnot : public DPdfAnnot
{
public:
    DPdfLinkAnnot();

    enum LinkType {
        Unknown = 0,
        Uri,        //URI, including web pages and other Internet resources
        RemoteGoTo, //Go to a destination within another document.
        Goto        //Go to a destination within current document.
    };

    bool pointIn(QPointF pos) override;

    QList<QRectF> boundaries() override;

    void setRectF(const QRectF &rect);

    void setUrl(QString url);

    QString url() const;

    void setFilePath(QString filePath);

    QString filePath() const;

    void setPage(int index, float left, float top);

    int pageIndex() const;

    QPointF offset() const;

    void setLinkType(int type);

    int linkType() const;

private:
    QRectF m_rect;
    QString m_url;
    QString m_filePath;

    int m_linkType = Unknown;
    int m_index = -1;
    float m_left = 0;
    float m_top = 0;
};

class DEEPIN_PDFIUM_EXPORT DPdfUnknownAnnot : public DPdfAnnot
{
public:
    DPdfUnknownAnnot();

    bool pointIn(QPointF pos) override;

    QList<QRectF> boundaries() override;

};
#endif // DPDFANNOT_H
