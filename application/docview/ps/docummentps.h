#ifndef DOCUMMENTPS_H
#define DOCUMMENTPS_H

#include "../docummentbase.h"
#include <libspectre/spectre-document.h>

class DocummentPS;
class DocummentPSPrivate;

class DocummentPS: public DocummentBase
{
    Q_OBJECT
public:
    DocummentPS(DWidget *parent = nullptr);
    ~DocummentPS() override;
    bool bDocummentExist() override;
//    bool getImage(int pagenum, QImage &image, double width, double height) override;
//    void docBasicInfo(stFileInfo &info) override;
    bool loadDocumment(QString filepath) override;
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentPS)
};

#endif // DOCUMMENTPS_H
