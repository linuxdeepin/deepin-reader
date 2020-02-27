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

#include "controller/AppSetting.h"
#include "controller/FileDataManager.h"
#include "docview/docummentproxy.h"

#include "utils/PublicFunction.h"

FontMenu::FontMenu(DWidget *parent):
    CustomMenu("FontMenu", parent)
{
    shortKeyList << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2 << KeyStr::g_ctrl_3
                 << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r
                 << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_equal << KeyStr::g_ctrl_smaller;

    initActions();

    initConnection();

    dApp->m_pModelService->addObserver(this);
}

FontMenu::~FontMenu()
{
    dApp->m_pModelService->removeObserver(this);
}

/**
 * @brief FontMenu::dealWithData
 * 处理全局消息
 * @param msgType     消息类型
 * @param msgContent  消息内容
 * @return            处理消息状态
 */
int FontMenu::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == E_DOCPROXY_MSG_TYPE) {
        emit sigDocProxyMsg(msgContent);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (shortKeyList.contains(msgContent)) {
            emit sigDealWithShortKey(msgContent);
            return ConstantMsg::g_effective_res;
        }
    } /*else if (msgType == MSG_SELF_ADAPTE_SCALE) {
        emit sigSetCurScale(msgContent);
    }*/
    return 0;
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

void FontMenu::setFileViewScale(bool)
{

}

void FontMenu::SlotDocProxyMsg(const QString &sContent)
{
    MsgModel mm;
    mm.fromJson(sContent);

    int nMsg = mm.getMsgType();
    if (nMsg == MSG_OPERATION_OPEN_FILE_OK) {
        QString sPath = mm.getPath();
        OnFileOpenOk(sPath);
    }
}

/**
 * @brief FontMenu::slotTwoPage
 * 双页显示
 */
void FontMenu::slotTwoPage()
{
    m_bDoubPage = !m_bDoubPage;

    MsgModel mm;
    mm.setMsgType(MSG_VIEWCHANGE_DOUBLE_SHOW);
    mm.setValue(QString::number(m_bDoubPage));

    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    mm.setPath(sCurPath);

    notifyMsg(E_TITLE_MSG_TYPE, mm.toJson());
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
    MsgModel mm;
    mm.setMsgType(MSG_VIEWCHANGE_ROTATE);
    mm.setValue("-1");

    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    mm.setPath(sCurPath);

    notifyMsg(E_TITLE_MSG_TYPE, mm.toJson());
}

/**
 * @brief FontMenu::slotRotateR
 * 右旋转
 */
void FontMenu::slotRotateR()
{
    MsgModel mm;
    mm.setMsgType(MSG_VIEWCHANGE_ROTATE);
    mm.setValue("1");

    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    mm.setPath(sCurPath);

    notifyMsg(E_TITLE_MSG_TYPE, mm.toJson());
}

/**
 * @brief FontMenu::slotFileOpenOk
 * 打开文件，加载参数
 */
void FontMenu::OnFileOpenOk(const QString &sPath)
{
    FileDataModel fdm = dApp->m_pDataManager->qGetFileData(sPath);

    int value = 0;

    //单双页
    value = fdm.qGetData(DoubleShow);
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
void FontMenu::slotDealWithShortKey(const QString &keyType)
{
    if (keyType == KeyStr::g_ctrl_1) {
        //还原
        resetAdaptive();
    } else if (keyType == KeyStr::g_ctrl_2) {
        //自适应高
        slotFiteH();
    } else if (keyType == KeyStr::g_ctrl_3) {
        //自适应宽
        slotFiteW();
    } else if (keyType == KeyStr::g_ctrl_r) {
        //左旋转
        rotateThumbnail(false);
    } else if (keyType == KeyStr::g_ctrl_shift_r) {
        //右旋转
        rotateThumbnail(true);
    } else if (keyType == KeyStr::g_ctrl_smaller) {
        //缩放
        setFileViewScale(false);
    } else if (keyType == KeyStr::g_ctrl_larger || keyType == KeyStr::g_ctrl_equal) {
        //放大
        setFileViewScale(true);
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
    connect(this, SIGNAL(sigDocProxyMsg(const QString &)), SLOT(SlotDocProxyMsg(const QString &)));
    connect(this, SIGNAL(sigDealWithShortKey(const QString &)), SLOT(slotDealWithShortKey(const QString &)));
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

    MsgModel mm;
    mm.setMsgType(MSG_VIEWCHANGE_ROTATE);
    mm.setValue(QString::number(nRes));

    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    mm.setPath(sCurPath);

    notifyMsg(E_TITLE_MSG_TYPE, mm.toJson());
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

    MsgModel mm;
    mm.setMsgType(MSG_VIEWCHANGE_FIT);
    mm.setValue(QString::number(iValue));

    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    mm.setPath(sCurPath);

    notifyMsg(E_TITLE_MSG_TYPE, mm.toJson());
}
