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
    void closeThread();
    void setDoc(DocummentPDF *doc);

protected:
    virtual void run();

private:
    volatile bool bStop;       //bStop是易失性变量，需要用volatile进行申明
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

    bool loadPages();
private:
    void loadWordCache(int indexpage, PageBase *page);
    bool abstractTextPage(const QList<Poppler::TextBox *> &text, double height, double width,
                          PageBase *page);
    Poppler::Document *document;
    ThreadLoadDoc m_threadloaddoc;
};

#endif // DOCUMMENTPDF_H
