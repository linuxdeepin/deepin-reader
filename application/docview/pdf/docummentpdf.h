#ifndef DOCUMMENTPDF_H
#define DOCUMMENTPDF_H

#include "../docummentbase.h"
#include <poppler-qt5.h>
#include <QThread>
class DocummentPDF;
class ThreadLoadDoc : public QThread
{
public:
    ThreadLoadDoc();
    void setDoc(DocummentPDF *doc);

protected:
    virtual void run();

private:
    volatile bool brun;       //bStop是易失性变量，需要用volatile进行申明
    DocummentPDF *m_doc;
};

class ThreadLoadWords : public QThread
{
public:
    ThreadLoadWords();
    void setDoc(DocummentPDF *doc);

protected:
    virtual void run();

private:
    volatile bool brun;       //bStop是易失性变量，需要用volatile进行申明
    DocummentPDF *m_doc;
};

class DocummentPDF: public DocummentBase
{
public:
    DocummentPDF(QWidget *parent = nullptr);
    bool openFile(QString filepath) override;
    QPoint global2RelativePoint(QPoint globalpoint) override;
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0) override;
    bool mouseSelectText(QPoint start, QPoint stop) override;
    void mouseSelectTextClear() override;
    bool mouseBeOverText(QPoint point) override;
    void scaleAndShow(double scale, RotateType_EM rotate) override;
    bool getImage(int pagenum, QImage &image, double width, double height, RotateType_EM rotate = RotateType_Normal) override;
    int getPageSNum() override;
    bool setViewModeAndShow(ViewMode_EM viewmode) override;

    bool loadPages();
    bool save(const QString& filePath, bool withChanges)const override;
    bool loadWords();
    void removeAllAnnotation();
    void removeAnnotation(QPoint start);
    void addAnnotation(const QPoint& startpos,const QPoint& endpos,const QColor& color);
    void search(const QString& strtext,const QColor& color);
private:
    void loadWordCache(int indexpage, PageBase *page);
    bool abstractTextPage(const QList<Poppler::TextBox *> &text,
                          PageBase *page);
    void showSinglePage();
    void showFacingPage();
    bool pdfsave(const QString& filePath, bool withChanges)const;
    void clearSearch();
    Poppler::Document *document;
    ThreadLoadDoc m_threadloaddoc;
    ThreadLoadWords m_threadloadwords;
    double m_scale;
    RotateType_EM m_rotate;
    QList<QWidget *>m_widgets;
    QWidget *pblankwidget;
    QList<Poppler::Annotation*> m_listsearch;
};

#endif // DOCUMMENTPDF_H
