#ifndef DOCUMMENTPS_H
#define DOCUMMENTPS_H

#include "../docummentbase.h"
#include <libspectre/spectre-document.h>
#include <QSettings>

class DocummentPS: public DocummentBase
{
public:
    DocummentPS(DWidget *parent = nullptr);
    ~DocummentPS();
//    bool openFile(QString filepath) override;
    bool bDocummentExist() override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
//    bool loadPages() override;
//    bool loadWords() override;
    void docBasicInfo(stFileInfo &info) override;
    bool loadDocumment(QString filepath) override;
private:
    void setBasicInfo(const QString &filepath);
private:
    SpectreDocument *document;
    SpectreRenderContext *m_renderContext;
    stFileInfo m_fileinfo;
    QSettings *m_settings;
};

#endif // DOCUMMENTPS_H
