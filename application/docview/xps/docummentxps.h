#ifndef DOCUMMENTXPS_H
#define DOCUMMENTXPS_H

#include "../docummentbase.h"
#include "xpsapi.h"
#include <QSettings>
class DocummentXPS;
class DocummentXPSPrivate;

class DocummentXPS: public DocummentBase
{
    Q_OBJECT
public:
    DocummentXPS(DWidget *parent = nullptr);
    ~DocummentXPS() override;
    bool bDocummentExist() override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
    void docBasicInfo(stFileInfo &info) override;
    bool loadDocumment(QString filepath) override;
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentXPS)
};

#endif // DOCUMMENTXPS_H
