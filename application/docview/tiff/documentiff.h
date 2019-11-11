#ifndef DOCUMENTIFF_H
#define DOCUMENTIFF_H

#include "../docummentbase.h"
#include <AbstractAppender.h>
#include <tiff.h>
#include <tiffio.h>

class DocummentTiff;
class DocummentTiffPrivate;

class DocummentTiff: public DocummentBase
{
    Q_OBJECT
    friend class PageTiff;
public:
    DocummentTiff(DWidget *parent = nullptr);
    bool bDocummentExist() override;
//    bool getImage(int pagenum, QImage &image, double width, double height) override;
//    void docBasicInfo(stFileInfo &info) override;
//    bool openFile(QString filepath) override;
    bool loadDocumment(QString filepath) override;
//    bool loadPages() override;
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentTiff)
};

#endif // DOCUMENTIFF_H
