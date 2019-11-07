#ifndef PAGEDJVU_H
#define PAGEDJVU_H
#include "../pagebase.h"
#include <poppler-qt5.h>
#include <libdjvu/ddjvuapi.h>
#include <libdjvu/miniexp.h>
#include <QImage>
#include <QThread>

class PageDJVU;
class PageDJVUPrivate;
class DocummentDJVU;
class PageDJVU: public PageBase
{
    Q_OBJECT
public:
    PageDJVU(QWidget *parent = 0);
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
    void setPage(int pageno);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PageDJVU)
};

#endif // PAGEDJVU_H
