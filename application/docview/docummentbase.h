#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include "pagebase.h"
#include <QScrollArea>
#include <QList>
#include <QVBoxLayout>
#include <QPoint>
#include <QColor>
#include <QWidget>

enum ViewMode_EM {
    ViewMode_SinglePage = 0,
    ViewMode_FacingPage
};
#include <QtDebug>

class MagnifierWidget: public QWidget
{
    Q_OBJECT
public:
    MagnifierWidget(QWidget *parent = nullptr);
    void setPixmap(QPixmap pixmap);
    void setPoint(QPoint point);
    int getMagnifierRadius();
    double getMagnifierScale();
    int getMagnifierRingWidth();
    void setMagnifierRadius(int radius);
    void setMagnifierScale(double scale);
    void setMagnifierRingWidth(int ringWidth);
    void setMagnifierColor(QColor color);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QColor m_magnifiercolor;
    int m_magnifierradius;
    int m_magnifierringwidth;
    QPoint m_magnifierpoint;
    QPixmap m_magnifierpixmap;
    double m_magnifierscale;
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

    virtual bool showMagnifier(QPoint point)
    {
        return false;
    }

    virtual bool setMagnifierStyle(QColor magnifiercolor = Qt::white, int magnifierradius = 100, int magnifierringwidth = 10, double magnifierscale = 3)
    {
        if (!m_magnifierwidget) {
            return false;
        }
        m_magnifierwidget->setMagnifierRadius(magnifierradius);
        m_magnifierwidget->setMagnifierScale(magnifierscale);
        m_magnifierwidget->setMagnifierRingWidth(magnifierringwidth);
        m_magnifierwidget->setMagnifierColor(magnifiercolor);
    }


    virtual void magnifierClear()
    {
        if (m_magnifierwidget) {
            m_magnifierwidget->setPixmap(QPixmap());
            m_magnifierwidget->hide();
        }
    }

    virtual int getPageSNum()
    {
        return 0;
    }

    virtual bool setViewModeAndShow(ViewMode_EM viewmode)
    {
        return false;
    }

    virtual bool save(const QString &filePath, bool withChanges)const
    {
        qDebug() << "do nothing";
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
    mutable bool m_bModified;
    MagnifierWidget *m_magnifierwidget;
    int m_lastmagnifierpagenum;
};

#endif // DOCUMMENTBASE_H
