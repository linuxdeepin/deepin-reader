#ifndef FONTWIDGET_H
#define FONTWIDGET_H

#include <DLabel>
#include <DSlider>
#include <DImageButton>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QtMath>

#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"

class FontWidget : public CustomWidget
{
    Q_OBJECT
public:
    FontWidget(CustomWidget *parent = nullptr);
    ~FontWidget() override;

signals:
    void sigWidgetHide();

public:
    int dealWithData(const int &, const QString &) override;

protected:
    void initWidget() override;

protected:
    void  paintEvent(QPaintEvent *e) override;
    void  hideEvent(QHideEvent *event) override;

private:
    void rotateFileView(bool isRight = true);
    void scaleAndRotate(int);

private slots:
    void slotSetChangeVal(int);
    void slotSetDoubPageViewCheckIcon();
    void slotSetSuitHCheckIcon();
    void slotSetSuitWCheckIcon();
    void slotSetRotateLeftCheckIcon();
    void slotSetRotateRightCheckIcon();

private:
    DLabel *m_pEnlargeLab = nullptr;
    DLabel *m_pMinLabALab = nullptr;
    DLabel *m_pMaxLabALab = nullptr;
    DSlider *m_pEnlargeSlider = nullptr;
    DImageButton *m_pDoubPageViewBtn = nullptr;
    DImageButton *m_pSuitHBtn = nullptr;
    DImageButton *m_pSuitWBtn = nullptr;
    DImageButton *m_pRotateLeftBtn = nullptr;
    DImageButton *m_pRotateRightBtn = nullptr;
    DLabel *m_pDoubPageViewLab = nullptr;
    DLabel *m_pSuitHLab = nullptr;
    DLabel *m_pSuitWLab = nullptr;
    DLabel *m_pRotateLeftLab = nullptr;
    DLabel *m_pRotateRightLab = nullptr;
    int m_rotate = 360;  // 旋转角度
    int m_rotateType = RotateType_Normal;
};

#endif // FONTWIDGET_H
