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
//    bool openFile(QString filepath) override;
    bool bDocummentExist() override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
    bool loadDocumment(QString filepath) override;

//    bool loadPages() override;
//    bool loadWords() override;
    void docBasicInfo(stFileInfo &info) override;
    ddjvu_document_t *getDocument();
    ddjvu_context_t *getContext();
    ddjvu_format_t *getFormat();
    QHash< QString, int > getPageByName();
//    //--------------------------------------------//
//    bool save(const QString &filePath, bool withChanges) override;
//    void removeAllAnnotation();
//    QString removeAnnotation(const QPoint &startpos) override;
//    void removeAnnotation(const QString &struuid) override;
//    QString addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color = Qt::yellow) override;
//    void search(const QString &strtext, QColor color = Qt::yellow) override;
//    void clearSearch() override;
//    bool annotationClicked(const QPoint &pos, QString &strtext);
//    void title(QString &title) override;
//private:
//    //--------------------------------------------//
//    bool pdfsave(const QString &filePath, bool withChanges)const;
//    void searchHightlight(Poppler::Page *page, const QString &strtext, stSearchRes &stres, const QColor &color);
//    void refreshOnePage(int ipage);
//    void setBasicInfo(const QString &filepath);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentDJVU)
};

class DocummentDJVUPrivate: public DocummentBasePrivate
{
    Q_OBJECT
public:
    DocummentDJVUPrivate(DocummentDJVU *parent): DocummentBasePrivate(parent),
        document(nullptr),
        m_fileinfo(),
        m_pageByName(),
        m_titleByIndex()
    {
    }

    ~DocummentDJVUPrivate()
    {
        ddjvu_document_release(document);
        ddjvu_context_release(m_context);
        ddjvu_format_release(m_format);
    }

    ddjvu_document_t *document;
    ddjvu_context_t *m_context;
    ddjvu_format_t *m_format;
    QHash< QString, int > m_pageByName;
    QHash< int, QString > m_titleByIndex;
    //--------------------------------------------//
    stFileInfo m_fileinfo;
    Q_DECLARE_PUBLIC(DocummentDJVU)
protected slots:
    void loadDocumment(QString filepath) override;
private:
    void setBasicInfo(const QString &filepath);
};

#endif // DOCUMMENTDJVU_H
