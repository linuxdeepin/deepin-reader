#ifndef DOCUMMENTXPS_H
#define DOCUMMENTXPS_H

#include "../docummentbase.h"
#include "xpsapi.h"
#include <QSettings>

class DocummentXPS: public DocummentBase
{
public:
    DocummentXPS(DWidget *parent = nullptr);
    ~DocummentXPS();
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
    QString m_thumbnailFileName;
    QString m_corePropertiesFileName;
    QString m_signatureOrigin;
    KZip *m_xpsArchive;
    stFileInfo m_fileinfo;
    QSettings *m_settings;
//    XpsDocument *document;
    XpsFile *m_xpsFile;
};

#endif // DOCUMMENTXPS_H
