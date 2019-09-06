#ifndef PAGEPDF_H
#define PAGEPDF_H
#include "../pagebase.h"

struct stWord {
    QString s;
    QRectF rect;
};

class PagePdf: public PageBase
{
    Q_OBJECT
public:
    PagePdf(QWidget *parent = 0);
    bool ifMouseMoveOverText(const QPoint point);
    bool pageTextSelections(const QPoint start, const QPoint end);
    void clearPageTextSelections();
    void appendWord(stWord word);
    void setImageWidth(double width);
    void setImageHeight(double height);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QList<QRect> paintrects;
    QList<stWord> m_words;
    double m_imagewidth;
    double m_imageheight;
};

#endif // PAGEPDF_H
