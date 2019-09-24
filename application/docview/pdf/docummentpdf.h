#ifndef DOCUMMENTPDF_H
#define DOCUMMENTPDF_H

#include "../docummentbase.h"
#include <poppler-qt5.h>

class DocummentPDF: public DocummentBase
{
public:
    DocummentPDF(DWidget *parent = nullptr);
    ~DocummentPDF();
    bool openFile(QString filepath) override;
    bool bDocummentExist() override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
//    bool loadPages() override;
//    bool loadWords() override;
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
    bool annotationClicked(const QPoint &pos, QString &strtext);
    void title(QString &title) override;
    void findNext()override;
    void findPrev()override;
    void setAnnotationText(int ipage, const QString &struuid, const QString &strtext) override;
    void getAnnotationText(const QString &struuid, QString &strtext, int ipage = -1)override;


private:
//    void loadWordCache(int indexpage, PageBase *page);
    //--------------------------------------------//
//    bool abstractTextPage(const QList<Poppler::TextBox *> &text, PageBase *page);
    bool pdfsave(const QString &filePath, bool withChanges)const;
    void searchHightlight(Poppler::Page *page, const QString &strtext, stSearchRes &stres, const QColor &color);
    void refreshOnePage(int ipage);
    void setBasicInfo(const QString &filepath);
private:
    Poppler::Document *document;
    //--------------------------------------------//
    QList<Poppler::Annotation *> m_listsearch;
    stFileInfo m_fileinfo;
    unsigned int m_cursearch;
    int m_findcurpage;
    QMap<int, int> m_pagecountsearch; //搜索结果页对应当前页个数
};

#endif // DOCUMMENTPDF_H
