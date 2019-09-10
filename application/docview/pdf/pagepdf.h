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
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal);
    bool getImage(QImage &image, double width, double height, RotateType_EM rotate = RotateType_Normal);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QList<QRect> paintrects;
    QList<stWord> m_words;
    double m_imagewidth;
    double m_imageheight;
    Poppler::Page *m_page;
    QColor m_paintercolor;
    QColor m_pencolor;
    int m_penwidth;
};

#endif // PAGEPDF_H
