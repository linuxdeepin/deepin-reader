/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FONTWIDGET_H
#define FONTWIDGET_H

#include <DLabel>
#include <DSlider>

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QtMath>

#include "CustomControl/CustomWidget.h"
#include "CustomControl/CustomClickLabel.h"
#include "docview/commonstruct.h"

/**
 * @brief The FontWidget class
 * 字体调整窗体
 */
class FontWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FontWidget)

public:
    explicit FontWidget(CustomWidget *parent = nullptr);

signals:
    void sigWidgetHide();
    void sigOpenFileOk(const int &);
//    void sigSetCurScale(const QString &);
    void sigDealWithKey(const QString &);
    void sigDealWithData(const int &, const QString &);
//    void sigKeyLargerOrSmaller(int);

public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *)Q_DECL_OVERRIDE;

private slots:
    void slotDealWithKey(const QString &);
    void slotDealWithData(const int &, const QString &);
    void slotUpdateTheme();
    void slotReset(const int &);

    void slotSetChangeVal(int);
    void slotSetDoubPageViewCheckIcon();
    void slotSetSuitHCheckIcon();
    void slotSetSuitWCheckIcon();
    void slotSetRotateLeftCheckIcon();
    void slotSetRotateRightCheckIcon();
//    void slotKeyLargerOrSmaller(int itype);

private:
    void rotateFileView(bool isRight = true);
    void scaleAndRotate();
//    void setShowSuitHIcon();
//    void setShowSuitWIcon();
    void initConnection();

    DLabel *getLabelLineH(int fixheight = 2);

    void initScaleLabel();
    void initScaleSlider();
    void initDowbleShow();
    void initAdaptateHeight();
    void initAdaptateWidght();
    void setScaleRotateViewModeAndShow();
    void setFileLargerOrSmaller(const int &);
    void setAppSetAdaptateHAndW();
    void setFrameValue();

    void SetCurScale(const QString &);

private:
    QStringList     shortKeyList;

    QHBoxLayout     *m_pDoubleShowLayout = nullptr;
    QHBoxLayout     *m_pAdaptateHeightLayout = nullptr;
    QHBoxLayout     *m_pAdaptateWidghtLayout = nullptr;
    QHBoxLayout     *m_pEnlargeSliderLayout = nullptr;

    CustomClickLabel *m_pEnlargeLab = nullptr;         // 缩放比例
    DSlider *m_pEnlargeSlider = nullptr;     // 缩放比例slider

    CustomClickLabel      *m_pDoubPageViewLabelIcon = nullptr;    // 双页显示ICON标签
    CustomClickLabel      *m_pSuitHLabelIcon = nullptr;           // 自适应高度ICON标签
    CustomClickLabel      *m_pSuitWLabelIcon = nullptr;           // 自适应宽度ICON标签

    int m_rotate = 0;                        // 旋转角度
    RotateType_EM m_rotateType = RotateType_0;    // 旋转类型
    bool m_bSuitH = false;                   // 自适应高度
    bool m_bSuitW = false;                   // 自适应宽度
    bool m_isDoubPage = false;               //  双页显示标志
    bool m_bIsAdaptMove = false;
};

#endif // FONTWIDGET_H
