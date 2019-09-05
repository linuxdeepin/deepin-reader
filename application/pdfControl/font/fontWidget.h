#ifndef FONTWIDGET_H
#define FONTWIDGET_H

#include <DLabel>
#include <DSlider>
#include <DImageButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPixmap>

#include "subjectObserver/CustomWidget.h"

class FontWidget : public CustomWidget
{
    Q_OBJECT
public:
    FontWidget(CustomWidget *parent = nullptr);
    ~FontWidget() override;

public:
    int dealWithData(const int &, const QString &) override;

protected:
    void initWidget() override;

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
};

#endif // FONTWIDGET_H
