#ifndef PAGEXPS_H
#define PAGEXPS_H
#include "../pagebase.h"
#include "xpsapi.h"
#include <QImage>
#include <QThread>
class PageXPS;
class PageXPSPrivate;
class PageXPS: public PageBase
{
    Q_OBJECT
public:
    PageXPS(QWidget *parent = nullptr);
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
    void setPage(XpsPage *page, int pageno);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PageXPS)
};

#endif // PAGEXPS_H
