#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include "pagebase.h"
#include <QScrollArea>
#include <QList>
#include <QVBoxLayout>

class DocummentBase: public QScrollArea
{
    Q_OBJECT
public:
    DocummentBase(QWidget *parent = nullptr);
    virtual bool openFile(QString filepath)
    {
        return false;
    }
    virtual bool mouseSelectText(QPoint start, QPoint stop)
    {
        return false;
    }
    virtual void mouseSelectTextClear()
    {
        return;
    }
    virtual bool mouseBeOverText(QPoint point)
    {
        return false;
    }
    QList<PageBase *> *getPages()
    {
        return &m_pages;
    }
    PageBase *getPage(int index)
    {
        if (m_pages.size() > index)
            return (PageBase *)m_pages.at(index);
        return nullptr;
    }
protected:
    QList<PageBase *> m_pages;
    QWidget m_widget;
    QVBoxLayout m_vboxLayout;
};

#endif // DOCUMMENTBASE_H
