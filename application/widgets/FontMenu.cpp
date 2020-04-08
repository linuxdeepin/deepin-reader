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
    CustomMenu(FONT_MENU, parent)
{
    shortKeyList << KeyStr::g_ctrl_2 << KeyStr::g_ctrl_3
                 << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r;

    initActions();

    initConnection();
}

int FontMenu::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTIFY_KEY_MSG) {
        OnShortKey(msgContent);
        if (shortKeyList.contains(msgContent)) {
            return MSG_OK;
        }
    }

    return MSG_NO_OK;
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

    FileDataModel fdm = m_sheet->qGetFileData();

    //单双页
    int value = fdm.qGetData(DoubleShow);

    m_bDoubPage = (value == 1) ? true : false;
    if (m_pTwoPageAction) {
        m_pTwoPageAction->setChecked(m_bDoubPage);
    }

    //自适应宽/高
    int adaptat = fdm.qGetData(Fit);

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

    m_sheet->setData(DoubleShow, QString::number(m_bDoubPage));
    m_sheet->setDoubleShow(m_bDoubPage);

    //解决双页时文档不能自适应是视窗大小的问题
    QJsonObject obj;
    QString str{""};
    str = QString::number(1) + Constant::sQStringSep + QString::number(0);
    obj.insert("content", str);
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);
    QJsonDocument jsonDoc(obj);
    notifyMsg(MSG_VIEWCHANGE_FIT, jsonDoc.toJson(QJsonDocument::Compact));
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
    if (dApp->openFileOk()) {
        dApp->setFlush(true);
    }
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
 * @brief FontMenu::slotDealWithShortKey
 * 处理键盘事件
 * @param keyType  事件内容
 */
void FontMenu::OnShortKey(const QString &keyType)
{
    if (keyType == KeyStr::g_ctrl_2) {
        //自适应高
        slotFiteH();
    } else if (keyType == KeyStr::g_ctrl_3) {
        //自适应宽
        slotFiteW();
    } else if (keyType == KeyStr::g_ctrl_r) {
        //左旋转
        slotRotateL();
    } else if (keyType == KeyStr::g_ctrl_shift_r) {
        //右旋转
        slotRotateR();
    }
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
    int iValue = Default_State;

    if (m_bFiteW) {
        iValue = ADAPTE_WIDGET_State;
    } else if (m_bFiteH) {
        iValue = ADAPTE_HEIGHT_State;
    }

    m_sheet->setFit(iValue);
}
