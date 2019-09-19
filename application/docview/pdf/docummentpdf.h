#ifndef DOCUMMENTPDF_H
#define DOCUMMENTPDF_H

#include "../docummentbase.h"
#include <poppler-qt5.h>

class DocummentPDF: public DocummentBase
{
public:
    DocummentPDF(DWidget *parent = nullptr);
    bool openFile(QString filepath) override;
    bool bDocummentExist() override;
//    QPoint global2RelativePoint(QPoint globalpoint) override;
//    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0) override;
//    bool mouseSelectText(QPoint start, QPoint stop) override;
//    void mouseSelectTextClear() override;
//    bool mouseBeOverText(QPoint point) override;
//    void scaleAndShow(double scale, RotateType_EM rotate) override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
//    int getPageSNum() override;
//    bool setViewModeAndShow(ViewMode_EM viewmode) override;
//    bool showMagnifier(QPoint point) override;

    bool loadPages() override;
    bool save(const QString &filePath, bool withChanges) override;
    bool loadWords() override;
    void removeAllAnnotation();
    QString removeAnnotation(const QPoint &startpos) override;
    void removeAnnotation(const QString &struuid) override;
    QString addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color = Qt::yellow) override;
    void search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color = Qt::yellow) override;
    void clearSearch() override;
//    int currentPageNo() override;
//    bool pageJump(int pagenum) override;
    void docBasicInfo(stFileInfo &info) override;
    bool annotationClicked(const QPoint &pos, QString &strtext);
    void title(QString &title) override;
//    Page::Link *mouseBeOverLink(QPoint point) override;
//    bool getSelectTextString(QString &st) override;
//    bool showSlideModel() override;
    void findNext()override;
    void findPrev()override;
//private slots:
//    void slot_vScrollBarValueChanged(int value) override;
//    void slot_hScrollBarValueChanged(int value) override;

private:
//    int pointInWhichPage(QPoint &qpoint);
    void loadWordCache(int indexpage, PageBase *page);
    bool abstractTextPage(const QList<Poppler::TextBox *> &text, PageBase *page);
//    void showSinglePage();
//    void showFacingPage();
    bool pdfsave(const QString &filePath, bool withChanges)const;
    void searchHightlight(Poppler::Page *page, const QString &strtext, stSearchRes &stres, const QColor &color);
    void refreshOnePage(int ipage);
    void setBasicInfo(const QString &filepath);
    Poppler::Document *document;
//    double m_scale;
//    RotateType_EM m_rotate;
//    QList<DWidget *>m_widgets;
//    DWidget *pblankwidget;
    QList<Poppler::Annotation *> m_listsearch;
//    int m_currentpageno;
//    bool donotneedreloaddoc;
    stFileInfo m_fileinfo;
    unsigned int m_cursearch;
    int m_findcurpage;
    QMap<int, int> m_pagecountsearch; //搜索结果页对应当前页个数
};

#endif // DOCUMMENTPDF_H
