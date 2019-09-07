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
    bool mouseSelectText(QPoint start, QPoint stop) override;
    void mouseSelectTextClear() override;
    bool mouseBeOverText(QPoint point) override;
    void scaleAndShow(double scale) override;

    bool loadPages();
    bool loadWords();
private:
    void loadWordCache(int indexpage, PageBase *page);
    bool abstractTextPage(const QList<Poppler::TextBox *> &text,
                          PageBase *page);
    Poppler::Document *document;
    ThreadLoadDoc m_threadloaddoc;
    ThreadLoadWords m_threadloadwords;
    double m_scale;
};

#endif // DOCUMMENTPDF_H
