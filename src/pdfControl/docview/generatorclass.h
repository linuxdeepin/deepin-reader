#ifndef GENERATORCLASS_H
#define GENERATORCLASS_H

#include "commonstruct.h"
#include <DWidget>
#include <QWaitCondition>
#include <DGuiApplicationHelper>
#include <QThread>
#include <QPoint>
#include <QMutex>
#include <QImage>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

class PageBase;
class DocummentBase;

class MagnifierImageRender: public QThread
{
    Q_OBJECT
public:
    MagnifierImageRender();
    void getmagnigierimage(PageBase *page, double width, double scalebase, double magnifierscale, QPoint &pt);
    void stoprender() {brender = false;}
protected:
    void run()override;

signals:
    void signal_imagerenderover(QImage image);
private:
    PageBase *pagebase = nullptr;
    double renderwidth;
    double basescale;
    double scalemagnifier;
    QPoint pos;
    QPoint pold;
    bool brender = false;
    QMutex mtex;
    QWaitCondition condition;
};

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

    void setShowState(bool bshow);
    bool showState();
    void showrectimage(PageBase *page, double pagescale, RotateType_EM type, QPoint &pt);
protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void slotupadteimage(QImage image);
private:
    int m_magnifierradius;
    int m_magnifierringwidth;
    QPoint m_magnifierpoint;
    QPixmap m_magnifierpixmap;
    double m_magnifierscale;
    bool bStartShow;
    int m_magnifiermapradius;
    int m_magnifierringmapwidth;
//    int m_magnifiershadowwidth;
    MagnifierImageRender renderthread;
    RotateType_EM rotatetype;
};

class ThreadLoadData : public QThread
{
    Q_OBJECT
public:
    ThreadLoadData();
    void setDoc(DocummentBase *doc);
    void setRestart();
signals:
    void signal_dataLoaded(bool);
protected:
    virtual void run();

private:
    DocummentBase *m_doc;
    bool restart;
};



#endif // GENERATORCLASS_H
