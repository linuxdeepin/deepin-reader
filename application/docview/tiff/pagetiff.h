#ifndef PAGETIFF_H
#define PAGETIFF_H
#include "../pagebase.h"
#include <tiff.h>
#include <tiffio.h>

class DocummentTiff;

class PageTiff;
class PageTiffPrivate;
class PageTiff: public PageBase
{
    Q_OBJECT
public:
    PageTiff(QWidget *parent = nullptr);
//    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
    void setPage(int pageno, TIFF *document);

private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PageTiff)
};

#endif // PAGETIFF_H
