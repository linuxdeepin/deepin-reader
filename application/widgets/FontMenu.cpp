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
#include "FontMenu.h"
#include "DocSheet.h"
#include "ModuleHeader.h"
#include "MsgHeader.h"
#include "docview/docummentproxy.h"

FontMenu::FontMenu(DWidget *parent):
    CustomMenu(parent)
{
    initActions();

    initConnection();
}

bool FontMenu::handleShortcut(QString shortcut)
{
    if (shortcut == KeyStr::g_ctrl_2) {
        //自适应高
        slotFiteH();
        return true;
    } else if (shortcut == KeyStr::g_ctrl_3) {
        //自适应宽
        slotFiteW();
        return true;
    } else if (shortcut == KeyStr::g_ctrl_r) {
        //左旋转
        slotRotateL();
        return true;
    } else if (shortcut == KeyStr::g_ctrl_shift_r) {
        //右旋转
        slotRotateR();
        return true;
    }

    return false;
}

/**
 * @brief FontMenu::resetAdaptive
 * 手动改变(ctrl + 1)缩放比例时，复位自适应宽高
 */
void FontMenu::resetAdaptive()
{
    //CancelFitState();

    setAppSetFiteHAndW();
}

void FontMenu::readCurDocParam(DocSheet *sheet)
{
    m_sheet = sheet;

    if (m_sheet.isNull())
        return;

    //单双页
    int value = m_sheet->getOper(DoubleShow).toInt();

    m_bDoubPage = (value == 1) ? true : false;
    if (m_pTwoPageAction) {
        m_pTwoPageAction->setChecked(m_bDoubPage);
    }

    //自适应宽/高
    int adaptat = m_sheet->getOper(Fit).toInt();

    if (adaptat == ADAPTE_WIDGET_State) {
        m_bFiteW = true;
        m_bFiteH = false;
    } else if (adaptat == ADAPTE_HEIGHT_State) {
        m_bFiteH = true;
        m_bFiteW = false;
    } else {
        m_bFiteW = false;
        m_bFiteH = false;
    }
    m_pFiteWAction->setChecked(m_bFiteW);
    m_pFiteHAction->setChecked(m_bFiteH);
}


/**
 * @brief FontMenu::slotTwoPage
 * 双页显示
 */
void FontMenu::slotTwoPage()
{
    if (m_sheet.isNull())
        return;

    m_bDoubPage = !m_bDoubPage;

    m_sheet->setData(DoubleShow, (int)m_bDoubPage);
    m_sheet->setDoubleShow(m_bDoubPage);

    if (DoubleShow) {
        if (m_sheet->getOper(Thumbnail).toInt() == 1 || ADAPTE_WIDGET_State == m_sheet->getOper(Fit).toInt() || QString::number(m_sheet->getOper(Scale).toDouble(), 'f', 2) == "100.00")
            m_sheet->setFit(ADAPTE_WIDGET_State);
    } else
        m_sheet->setFit(ADAPTE_WIDGET_State);
}

/**
 * @brief FontMenu::slotFiteH
 *自适应高
 */
void FontMenu::slotFiteH()
{
    m_pFiteWAction->setChecked(false);
    m_bFiteW = false;
    m_bFiteH = !m_bFiteH;
    m_pFiteHAction->setChecked(m_bFiteH);

    setAppSetFiteHAndW();
}

/**
 * @brief FontMenu::slotFiteW
 * 自适应宽
 */
void FontMenu::slotFiteW()
{
    m_pFiteHAction->setChecked(false);
    m_bFiteH = false;
    m_bFiteW = !m_bFiteW;
    m_pFiteWAction->setChecked(m_bFiteW);

    setAppSetFiteHAndW();
}

/**
 * @brief FontMenu::slotRotateL
 * 左旋转
 */
void FontMenu::slotRotateL()
{
    if (m_sheet.isNull())
        return;

    m_sheet->setRotateLeft();
}

/**
 * @brief FontMenu::slotRotateR
 * 右旋转
 */
void FontMenu::slotRotateR()
{
    if (m_sheet.isNull())
        return;

    m_sheet->setRotateRight();
}

/**
 * @brief FontMenu::initMenu
 * 初始化Menu
 */
void FontMenu::initActions()
{
    m_pTwoPageAction = createAction(tr("Two-Page View"), SLOT(slotTwoPage()), true);
    m_pFiteHAction = createAction(tr("Fit Height"), SLOT(slotFiteH()), true);
    m_pFiteWAction = createAction(tr("Fit Width"), SLOT(slotFiteW()), true);
    m_pRotateLAction = createAction(tr("Rotate Left"), SLOT(slotRotateL()));
    m_pRotateRAction = createAction(tr("Rotate Right"), SLOT(slotRotateR()));
}

/**
 * @brief FontMenu::initConnection
 * 初始化信号槽
 */
void FontMenu::initConnection()
{
}

/**
 * @brief FontMenu::createAction   创建菜单action
 * @param objName                  action名称
 * @param member                   action所关联的响应函数
 * @param checked                  action是否有选中状态
 * @return                         当前action指针
 */
QAction *FontMenu::createAction(const QString &objName, const char *member, bool checked)
{
    auto action = new QAction(objName, this);
    if (action) {
        action->setObjectName(objName);
        action->setCheckable(checked);

        connect(action, SIGNAL(triggered()), this, member);

        this->addAction(action);
        this->addSeparator();

        return action;
    }
    return nullptr;
}

/**
 * @brief FontMenu::setAppSetFiteHAndW
 * 记录文档属性,下次加载时使用
 * 0:都不自适应  1:自适应宽  10:自适应高
 */
void FontMenu::setAppSetFiteHAndW()
{
    if (m_sheet.isNull())
        return;

    int iValue = Default_State;

    if (m_bFiteW) {
        iValue = ADAPTE_WIDGET_State;
    } else if (m_bFiteH) {
        iValue = ADAPTE_HEIGHT_State;
    }

    m_sheet->setFit(iValue);
}
