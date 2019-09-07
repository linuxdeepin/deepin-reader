#ifndef PAGEPDF_H
#define PAGEPDF_H
#include "../pagebase.h"
#include <QImage>
#include <poppler-qt5.h>

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
    void setPage(Poppler::Page *page);
    void showImage(double scale = 1);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QList<QRect> paintrects;
    QList<stWord> m_words;
    double m_imagewidth;
    double m_imageheight;
    Poppler::Page *m_page;
};

#endif // PAGEPDF_H
