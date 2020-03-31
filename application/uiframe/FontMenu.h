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

class FontMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(FontMenu)

public:
    explicit FontMenu(DWidget *parent = nullptr);

    // CustomMenu interface
public:
    int dealWithData(const int &, const QString &) override;
    void CancelFitState();

    // CustomMenu interface
protected:
    void initActions() override;

private slots:
    void slotTwoPage();
    void slotFiteH();
    void slotFiteW();
    void slotRotateL();
    void slotRotateR();

private:
    void initConnection();
    QAction *createAction(const QString &objName, const char *, bool checked = false);
    void rotateThumbnail(const bool &);

    void setAppSetFiteHAndW();
    void resetAdaptive();
    void OnFileOpenOk(const QString &);
    void OnShortKey(const QString &keyType);

private:
    QAction         *m_pTwoPageAction = nullptr;          // 双页
    QAction         *m_pFiteHAction = nullptr;            // 自适应高
    QAction         *m_pFiteWAction = nullptr;            // 自适应宽
    QAction         *m_pRotateLAction = nullptr;          // 左旋转
    QAction         *m_pRotateRAction = nullptr;          // 右旋转
    bool            m_bDoubPage = false;                     // 双页
    bool            m_bFiteH = false;                        // 自适应高
    bool            m_bFiteW = false;                        // 自适应宽
};

#endif // FONTMENU_H
