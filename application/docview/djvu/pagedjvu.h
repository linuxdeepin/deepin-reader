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
//    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    PageInterface *getInterFace() override;
//    bool loadWords() override;
//    bool loadLinks() override;
    void setPage(int pageno);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), PageDJVU)
};

class PageDJVUPrivate: public PageBasePrivate, public  PageInterface
{
    Q_OBJECT
public:
    PageDJVUPrivate(PageDJVU *parent): PageBasePrivate(parent),
        m_parent(nullptr),
        m_resolution(0)
    {
    }

    ~PageDJVUPrivate() override
    {
    }
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    bool loadData() override
    {
        loadWords();
        loadLinks();
        return true;
    }

    void setPage(int pageno);
    DocummentDJVU *m_parent;
    int m_resolution;
private:

    bool loadWords();
    bool loadLinks();
};

#endif // PAGEDJVU_H
