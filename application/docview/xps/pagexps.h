#ifndef PAGEXPS_H
#define PAGEXPS_H
#include "../pagebase.h"
#include "xpsapi.h"
#include <QImage>
#include <QThread>

class PageXPS: public PageBase
{
    Q_OBJECT
public:
    PageXPS(QWidget *parent = 0);
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    bool loadWords() override;
    bool loadLinks() override;
    void setPage(XpsPage *page, int pageno);
private:
    XpsPage *m_page;
};

#endif // PAGEXPS_H
