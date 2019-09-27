#ifndef DOCUMMENTPDF_H
#define DOCUMMENTPDF_H

#include "../docummentbase.h"
#include <poppler-qt5.h>


class DocummentPDF: public DocummentBase
{
public:
    DocummentPDF(DWidget *parent = nullptr);
    ~DocummentPDF();
//    bool openFile(QString filepath) override;
    bool bDocummentExist() override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
    void docBasicInfo(stFileInfo &info) override;
    //--------------------------------------------//
    bool save(const QString &filePath, bool withChanges) override;
    bool saveas(const QString &filePath, bool withChanges) override;
    void removeAllAnnotation();
    QString removeAnnotation(const QPoint &startpos) override;
    void removeAnnotation(const QString &struuid) override;
    QString addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color = Qt::yellow) override;
    void search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color = Qt::yellow) override;
    void clearSearch() override;
    bool annotationClicked(const QPoint &pos, QString &strtext) override;
    void title(QString &title) override;
    void findNext()override;
    void findPrev()override;
    void setAnnotationText(int ipage, const QString &struuid, const QString &strtext) override;
    void getAnnotationText(const QString &struuid, QString &strtext, int ipage = -1)override;
    bool loadDocumment(QString filepath) override;
    void stopLoadPageThread() override;


private:
    bool pdfsave(const QString &filePath, bool withChanges)const;
    void searchHightlight(Poppler::Page *page, const QString &strtext, stSearchRes &stres, const QColor &color);
    void refreshOnePage(int ipage);
    void setBasicInfo(const QString &filepath);
private:
    Poppler::Document *document;  
    stFileInfo m_fileinfo;
};

#endif // DOCUMMENTPDF_H
