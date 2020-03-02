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

#include <DFontSizeManager>
#include <QJsonObject>
#include <QWidgetAction>

#include "MsgModel.h"

#include "business/FileDataManager.h"
#include "docview/docummentproxy.h"
#include "utils/PublicFunction.h"

FontMenu::FontMenu(DWidget *parent):
    CustomMenu("FontMenu", parent)
{
    shortKeyList << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2 << KeyStr::g_ctrl_3
                 << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r;

    initActions();

    initConnection();
}

int FontMenu::qDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        OnFileOpenOk(msgContent);
    }

    return MSG_NO_OK;
}

/**
 * @brief FontMenu::resetAdaptive
 * 手动改变(ctrl + 1)缩放比例时，复位自适应宽高
 */
void FontMenu::resetAdaptive()
{
    m_bFiteH = false;
    m_bFiteW = false;

    m_pFiteHAction->setChecked(false);
    m_pFiteWAction->setChecked(false);

    setAppSetFiteHAndW();
}


/**
 * @brief FontMenu::slotTwoPage
 * 双页显示
 */
void FontMenu::slotTwoPage()
{
    m_bDoubPage = !m_bDoubPage;

    QJsonObject obj;
    obj.insert("content", QString::number(m_bDoubPage));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_VIEWCHANGE_DOUBLE_SHOW, doc.toJson(QJsonDocument::Compact));
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
    QJsonObject obj;
    obj.insert("content", "-1");
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_VIEWCHANGE_ROTATE, doc.toJson(QJsonDocument::Compact));
}

/**
 * @brief FontMenu::slotRotateR
 * 右旋转
 */
void FontMenu::slotRotateR()
{
    QJsonObject obj;
    obj.insert("content", "1");
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_VIEWCHANGE_ROTATE, doc.toJson(QJsonDocument::Compact));
}

/**
 * @brief FontMenu::slotFileOpenOk
 * 打开文件，加载参数
 */
void FontMenu::OnFileOpenOk(const QString &sPath)
{
    FileDataModel fdm = dApp->m_pDataManager->qGetFileData(sPath);

    //单双页
    int value = fdm.qGetData(DoubleShow);
    m_bDoubPage = (value == 1) ? true : false;
    if (m_pTwoPageAction) {
        m_pTwoPageAction->setChecked(m_bDoubPage);
    }

    //自适应宽/高
    int adaptat = fdm.qGetData(Fit);
    if (adaptat == 1) {
        m_bFiteW = true;
        m_bFiteH = false;
        m_pFiteWAction->setChecked(m_bFiteW);
        m_pFiteHAction->setChecked(m_bFiteH);
    } else if (adaptat == 10) {
        m_bFiteH = true;
        m_bFiteW = false;
        m_pFiteWAction->setChecked(m_bFiteW);
        m_pFiteHAction->setChecked(m_bFiteH);
    }
}

/**
 * @brief FontMenu::slotDealWithShortKey
 * 处理键盘事件
 * @param keyType  事件内容
 */
//void FontMenu::slotDealWithShortKey(const QString &keyType)
//{
//    if (keyType == KeyStr::g_ctrl_1) {
//        //还原
//        resetAdaptive();
//    } else if (keyType == KeyStr::g_ctrl_2) {
//        //自适应高
//        slotFiteH();
//    } else if (keyType == KeyStr::g_ctrl_3) {
//        //自适应宽
//        slotFiteW();
//    } else if (keyType == KeyStr::g_ctrl_r) {
//        //左旋转
//        rotateThumbnail(false);
//    } else if (keyType == KeyStr::g_ctrl_shift_r) {
//        //右旋转
//        rotateThumbnail(true);
//    }
//}

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
 * @brief FontMenu::rotateThumbnail 旋转文档和缩略图
 * @param direct true:向右 false:向左
 */
void FontMenu::rotateThumbnail(const bool &direct)
{
    int nRes = -1;
    if (direct) {
        nRes = 1;
    }

    QJsonObject obj;
    obj.insert("content", QString::number(nRes));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    dApp->m_pModelService->notifyMsg(MSG_VIEWCHANGE_ROTATE, doc.toJson(QJsonDocument::Compact));
}

/**
 * @brief FontMenu::setAppSetFiteHAndW
 * 记录文档属性,下次加载时使用
 * 0:都不自适应  1:自适应宽  10:自适应高
 */
void FontMenu::setAppSetFiteHAndW()
{
    int iValue = 0;

    if (m_bFiteW) {
        iValue = 1;
    } else if (m_bFiteH) {
        iValue = 10;
    }

    QJsonObject obj;
    obj.insert("content", QString::number(iValue));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_VIEWCHANGE_FIT, doc.toJson(QJsonDocument::Compact));
}
