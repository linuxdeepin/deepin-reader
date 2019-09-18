#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include "pagebase.h"
#include "docview/commonstruct.h"
#include <DWidget>
#include <DScrollArea>
#include <DScrollBar>
#include <DGuiApplicationHelper>
#include <QList>
#include <QVBoxLayout>
#include <QPoint>
#include <QColor>
#include <QVideoWidget>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

enum ViewMode_EM {
    ViewMode_SinglePage = 0,
    ViewMode_FacingPage
};
#include <QtDebug>

class MagnifierWidget: public DWidget
{
    Q_OBJECT
public:
    MagnifierWidget(DWidget *parent = nullptr);
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

class DocummentBase: public DScrollArea
{
    Q_OBJECT
public:
    DocummentBase(DWidget *parent = nullptr);
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

    virtual int getPageSNum()
    {
        return 0;
    }

    virtual bool setViewModeAndShow(ViewMode_EM viewmode)
    {
        return false;
    }

    virtual bool save(const QString &filePath, bool withChanges)
    {
        qDebug() << "do nothing";
        return false;
    }

    virtual QString removeAnnotation(const QPoint &startpos) {}

    virtual void removeAnnotation(const QString &struuid) {}

    virtual QString addAnnotation(const QPoint &starpos, const QPoint &endpos, QColor color = Qt::yellow) {}


    virtual void search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color = Qt::yellow) {}
    virtual int currentPageNo()
    {
        return -1;
    }

    virtual bool pageJump(int pagenum)
    {
        return false;
    }

    virtual void clearSearch() {}


    virtual Page::Link *mouseBeOverLink(QPoint point)
    {
        return nullptr;
    }

    virtual bool getSelectTextString(QString &st)
    {
        return false;
    }

    virtual bool showSlideModel()
    {
        return false;
    }

    bool exitSlideModel()
    {
        m_slidewidget->hide();
        this->show();
        m_bslidemodel = false;
        m_slidepageno = -1;
        return true;
    }

    virtual void docBasicInfo(stFileInfo &info){}

    virtual void findNext(){}

    virtual void findPrev(){}

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

    void magnifierClear()
    {
        if (m_magnifierwidget) {
            m_magnifierwidget->setPixmap(QPixmap());
            m_magnifierwidget->hide();
        }
    }

    void pageMove(double mvx, double mvy)
    {
        DScrollBar *scrollBar_X = horizontalScrollBar();
        if (scrollBar_X)
            scrollBar_X->setValue(scrollBar_X->value() + mvx);
        DScrollBar *scrollBar_Y = verticalScrollBar();
        if (scrollBar_Y)
            scrollBar_Y->setValue(scrollBar_Y->value() + mvy);
    }
    virtual void title(QString &title) {}

signals:
    void signal_pageChange(int);
protected slots:
    virtual void slot_vScrollBarValueChanged(int value)
    {
//        qDebug() << "slot_vScrollBarValueChanged" << value;
    }
    virtual void slot_hScrollBarValueChanged(int value)
    {
//        qDebug() << "slot_hScrollBarValueChanged" << value;
    }
protected:
    QList<PageBase *> m_pages;
    DWidget m_widget;
    QVBoxLayout m_vboxLayout;
    ViewMode_EM m_viewmode;
    mutable bool m_bModified;
    MagnifierWidget *m_magnifierwidget;
    int m_lastmagnifierpagenum;
    DWidget *m_slidewidget;
    bool m_bslidemodel;
    DLabel *pslidelabel;
    int m_slidepageno;
    DLabel *pslideanimationlabel;
};

#endif // DOCUMMENTBASE_H
