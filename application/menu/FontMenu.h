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
#ifndef FONTMENU_H
#define FONTMENU_H

#include "CustomControl/CustomMenu.h"
#include "docview/commonstruct.h"

class FontMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(FontMenu)

public:
    explicit FontMenu(DWidget *parent = nullptr);
    ~FontMenu() Q_DECL_OVERRIDE;

signals:
    //新增
    void sigFileOpenOk(const QString &);
    void sigDealWithShortKey(const QString &);
//    void sigSetCurScale(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomMenu interface
protected:
    void initActions() Q_DECL_OVERRIDE;

private slots:
    void slotTwoPage();
    void slotFiteH();
    void slotFiteW();
    void slotRotateL();
    void slotRotateR();
//    void slotScaleValChanged(int);

    //新增
    void slotFileOpenOk(const QString &);
    void slotDealWithShortKey(const QString &);
//    void slotSetCurScale(const QString &);

private:
    void initConnection();
    QAction *createAction(const QString &objName, const char *, bool checked = false);
    void rotateThumbnail(bool);
    void scaleAndRotate();
    void calcRotateType();
    void setScaleRotateViewModeAndShow();
    void setAppSetFiteHAndW();
    void resetFiteHAndW();
    void resetAdaptive();
    void setFileViewScale(bool);
//    void setSliderMaxValue();

private:
    QStringList     shortKeyList;                 // 要处理的消息类型
//    DLabel *m_pEnlargeLab = nullptr;              // 缩放比例label
//    DSlider *m_pEnlargeSlider = nullptr;          // 缩放比例slider
    QAction *m_pTwoPageAction = nullptr;          // 双页
    QAction *m_pFiteHAction = nullptr;            // 自适应高
    QAction *m_pFiteWAction = nullptr;            // 自适应宽
    QAction *m_pRotateLAction = nullptr;          // 左旋转
    QAction *m_pRotateRAction = nullptr;          // 右旋转
//    int m_nScale = 100;                           // 缩放比例(缺省100%)
    bool m_bDoubPage = false;                     // 双页
    bool m_bFiteH = false;                        // 自适应高
    bool m_bFiteW = false;                        // 自适应宽
    int m_nRotate = 0;                            // 旋转角度(0~360度)
    RotateType_EM m_rotateType = RotateType_0;    // 旋转类型(后台所需旋转类型)
    bool m_bIsAdaptMove = false;                  // 在自适应宽高时，缩放变化不响应

};

#endif // FONTMENU_H
