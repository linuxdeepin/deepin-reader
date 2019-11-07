#ifndef DOCUMMENTDJVU_H
#define DOCUMMENTDJVU_H

#include "../docummentbase.h"
#include <poppler-qt5.h>
#include <libdjvu/ddjvuapi.h>
#include <libdjvu/miniexp.h>
class DocummentDJVU;
class DocummentDJVUPrivate;

class DocummentDJVU: public DocummentBase
{
    Q_OBJECT
public:
    DocummentDJVU(DWidget *parent = nullptr);
    ~DocummentDJVU() override;
    bool bDocummentExist() override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
    bool loadDocumment(QString filepath) override;
    void docBasicInfo(stFileInfo &info) override;
    ddjvu_document_t *getDocument();
    ddjvu_context_t *getContext();
    ddjvu_format_t *getFormat();
    QHash< QString, int > getPageByName();
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentDJVU)
};

#endif // DOCUMMENTDJVU_H
