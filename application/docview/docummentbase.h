#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include "pagebase.h"
#include <QScrollArea>
#include <QList>
#include <QVBoxLayout>
#include <QPoint>

enum ViewMode_EM {
    ViewMode_SinglePage = 0,
    ViewMode_FacingPage
};

class DocummentBase: public QScrollArea
{
    Q_OBJECT
public:
    DocummentBase(QWidget *parent = nullptr);
    virtual bool openFile(QString filepath)
    {
        return false;
    }
    virtual bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0)
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
    virtual void scaleAndShow(double scale, RotateType_EM rotate)
    {
        return;
    }
    virtual bool mouseBeOverText(QPoint point)
    {
        return false;
    }

    virtual bool getImage(int pagenum, QImage &image, double width, double height)
    {
        return false;
    }

    virtual QPoint global2RelativePoint(QPoint globalpoint)
    {
        return QPoint();
    }

    virtual int getPageSNum()
    {
        return 0;
    }

    virtual bool setViewModeAndShow(ViewMode_EM viewmode)
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
    ViewMode_EM m_viewmode;
};

#endif // DOCUMMENTBASE_H
