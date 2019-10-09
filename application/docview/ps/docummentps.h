#ifndef DOCUMMENTPS_H
#define DOCUMMENTPS_H

#include "../docummentbase.h"
#include <libspectre/spectre-document.h>
#include <QSettings>
class DocummentPS;
class DocummentPSPrivate;

class DocummentPS: public DocummentBase
{
    Q_OBJECT
public:
    DocummentPS(DWidget *parent = nullptr);
    ~DocummentPS() override;
//    bool openFile(QString filepath) override;
    bool bDocummentExist() override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
    void docBasicInfo(stFileInfo &info) override;
    bool loadDocumment(QString filepath) override;
//signals:
//    void signal_loadDocumment(QString);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentPS)
};

class DocummentPSPrivate: public DocummentBasePrivate
{
    Q_OBJECT
public:
    DocummentPSPrivate(DocummentPS *parent): DocummentBasePrivate(parent)
    {
        document = nullptr;
        m_renderContext = nullptr;
        m_settings = new QSettings("docummentps", "deepin_reader", this);
    }

    ~DocummentPSPrivate()
    {
        if (nullptr != m_renderContext) {
            spectre_render_context_free(m_renderContext);
            m_renderContext = nullptr;
        }

        if (nullptr != document) {
            spectre_document_free(document);
            document = nullptr;
        }
    }

    SpectreDocument *document;
    SpectreRenderContext *m_renderContext;
    stFileInfo m_fileinfo;
    QSettings *m_settings;
    Q_DECLARE_PUBLIC(DocummentPS)
protected slots:
    void loadDocumment(QString filepath) override;
private:
    void setBasicInfo(const QString &filepath);
};
#endif // DOCUMMENTPS_H
