#ifndef DOCUMMENTDJVU_H
#define DOCUMMENTDJVU_H

#include "../docummentbase.h"
#include <poppler-qt5.h>
#include <libdjvu/ddjvuapi.h>
#include <libdjvu/miniexp.h>
#include <QThread>

class DocummentDJVU: public DocummentBase
{
public:
    DocummentDJVU(DWidget *parent = nullptr);
    bool openFile(QString filepath) override;
    ddjvu_document_t *getDocument();
    ddjvu_context_t *getContext();
    ddjvu_format_t *getFormat();
    QHash< QString, int > getPageByName();
    QPoint global2RelativePoint(QPoint globalpoint) override;
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0) override;
    bool mouseSelectText(QPoint start, QPoint stop) override;
    void mouseSelectTextClear() override;
    bool mouseBeOverText(QPoint point) override;
    void scaleAndShow(double scale, RotateType_EM rotate) override;
    bool getImage(int pagenum, QImage &image, double width, double height) override;
    int getPageSNum() override;
    bool setViewModeAndShow(ViewMode_EM viewmode) override;
    bool showMagnifier(QPoint point) override;

    bool loadPages() override;
    bool save(const QString &filePath, bool withChanges) override;
    bool loadWords() override;
    void removeAllAnnotation();
    QString removeAnnotation(const QPoint &startpos) override;
    void removeAnnotation(const QString &struuid) override;
    QString addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color = Qt::yellow) override;
    void search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color = Qt::yellow) override;
    void clearSearch() override;
    int currentPageNo() override;
    bool pageJump(int pagenum) override;
    void docBasicInfo(stFileInfo &info) override;
    bool annotationClicked(const QPoint &pos, QString &strtext);
    void title(QString &title) override;
    Page::Link *mouseBeOverLink(QPoint point) override;
    bool getSelectTextString(QString &st) override;
    bool showSlideModel() override;
private slots:
    void slot_vScrollBarValueChanged(int value) override;
    void slot_hScrollBarValueChanged(int value) override;

private:
    int pointInWhichPage(QPoint &qpoint);
//    void loadWordCache(int indexpage, PageBase *page);
//    bool abstractTextPage(const QList<Poppler::TextBox *> &text, PageBase *page);
    void showSinglePage();
    void showFacingPage();
    bool pdfsave(const QString &filePath, bool withChanges)const;
    void searchHightlight(Poppler::Page *page, const QString &strtext, stSearchRes &stres, const QColor &color);
    void refreshOnePage(int ipage);
    void setBasicInfo(const QString &filepath);
//    Poppler::Document *document;
    ddjvu_document_t *document;
    ddjvu_context_t *m_context;
    ddjvu_format_t *m_format;
    QHash< QString, int > m_pageByName;
    QHash< int, QString > m_titleByIndex;
    double m_scale;
    RotateType_EM m_rotate;
    QList<DWidget *>m_widgets;
    DWidget *pblankwidget;
    QList<Poppler::Annotation *> m_listsearch;
    int m_currentpageno;
    bool donotneedreloaddoc;
    stFileInfo m_fileinfo;
};

#endif // DOCUMMENTDJVU_H
