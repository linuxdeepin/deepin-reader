/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#include "ScaleMenu.h"

#include <QSignalMapper>

#include "MsgModel.h"

#include "controller/FileDataManager.h"

ScaleMenu::ScaleMenu(DWidget *parent)
    : CustomMenu("ScaleMenu", parent)
{
    initActions();
    InitConnection();

    shortKeyList << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_equal << KeyStr::g_ctrl_smaller;

    dApp->m_pModelService->addObserver(this);
}

ScaleMenu::~ScaleMenu()
{
    dApp->m_pModelService->removeObserver(this);
}

int ScaleMenu::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == E_DOCPROXY_MSG_TYPE) {
        emit sigDocProxyMsg(msgContent);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (shortKeyList.contains(msgContent)) {
            emit sigDealWithShortKey(msgContent);
            return ConstantMsg::g_effective_res;
        }
    }
    return 0;
}

void ScaleMenu::initActions()
{
    dataList = {10, 25, 50, 75, 100, 125, 150, 175, 200, 300, 400, 500};

    auto actionGroup = new QActionGroup(this);
    connect(actionGroup, SIGNAL(triggered(QAction *)), SLOT(slotActionTrigger(QAction *)));

    foreach (int iData, dataList) {
        QAction *action = new QAction(QString("%1%").arg(iData), this);
        action->setData(iData);
        action->setCheckable(true);
        addAction(action);

        actionGroup->addAction(action);
    }
}

void ScaleMenu::slotActionTrigger(QAction *action)
{
    int iData = action->data().toInt();
    __ChangeScale(iData);
}

void ScaleMenu::sloPrevScale()
{
    int nIndex = m_nCurrentIndex - 1;
    if (nIndex < 0) {
        return ;
    }
    int iData = dataList.at(nIndex);
    __ChangeScale(iData);
}

void ScaleMenu::sloNextScale()
{
    int nIndex = m_nCurrentIndex + 1;
    int nSize = dataList.size();
    if (nIndex < nSize) {
        int iData = dataList.at(nIndex);
        __ChangeScale(iData);
    }
}

void ScaleMenu::slotDocProxyMsg(const QString &sContent)
{
    MsgModel mm;
    mm.fromJson(sContent);

    int nMsg = mm.getMsgType();
    if (nMsg == MSG_OPERATION_OPEN_FILE_OK) {
        QString sPath = mm.getPath();
        OnFileOpenOk(sPath);
    }
}

void ScaleMenu::slotDealWithShortKey(const QString &keyType)
{
    if (keyType == KeyStr::g_ctrl_smaller) {
        //缩放
        sloPrevScale();
    } else if (keyType == KeyStr::g_ctrl_larger || keyType == KeyStr::g_ctrl_equal) {
        //放大
        sloNextScale();
    }
}

void ScaleMenu::__ChangeScale(const int &iData)
{
    emit sigCurrentScale(iData);

    m_nCurrentIndex = dataList.indexOf(iData);

    MsgModel mm;
    mm.setMsgType(MSG_FILE_SCALE);
    mm.setValue(QString::number(iData));

    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    mm.setPath(sCurPath);

    notifyMsg(E_TITLE_MSG_TYPE, mm.toJson());

    auto actionList = this->findChildren<QAction *>();
    foreach (auto a, actionList) {
        int nData = a->data().toInt();
        if (nData == iData) {
            a->setChecked(true);
            break;
        }
    }
}

void ScaleMenu::InitConnection()
{
    connect(this, SIGNAL(sigDocProxyMsg(const QString &)), SLOT(slotDocProxyMsg(const QString &)));
    connect(this, SIGNAL(sigDealWithShortKey(const QString &)), SLOT(slotDealWithShortKey(const QString &)));
}

void ScaleMenu::OnFileOpenOk(const QString &sPath)
{
    FileDataModel fdm = dApp->m_pDataManager->qGetFileData(sPath);
    int nScale = fdm.qGetData(Scale);
    if (nScale == 0) {
        nScale = 100;
    }

    m_nCurrentIndex = dataList.indexOf(nScale);
    emit sigCurrentScale(nScale);

    auto actionList = this->findChildren<QAction *>();
    foreach (auto a, actionList) {
        int nData = a->data().toInt();
        if (nData == nScale) {
            a->setChecked(true);
            break;
        }
    }

}
