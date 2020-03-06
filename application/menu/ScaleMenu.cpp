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

#include "widgets/main/MainTabWidgetEx.h"

ScaleMenu::ScaleMenu(DWidget *parent)
    : CustomMenu("ScaleMenu", parent)
{
    initActions();

    shortKeyList << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_equal << KeyStr::g_ctrl_smaller;
}

int ScaleMenu::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK || msgType == MSG_TAB_SHOW_FILE_CHANGE) {
        OnFileOpenOk(msgContent);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        onShortKey(msgContent);

        if (shortKeyList.contains(msgContent)) {
            return MSG_OK;
        }
    }

    return MSG_NO_OK;
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

void ScaleMenu::slotPrevScale()
{
    int nIndex = m_nCurrentIndex - 1;
    if (nIndex < 0) {
        return ;
    }
    int iData = dataList.at(nIndex);
    __ChangeScale(iData);
}

void ScaleMenu::slotNextScale()
{
    int nIndex = m_nCurrentIndex + 1;
    int nSize = dataList.size();
    if (nIndex < nSize) {
        int iData = dataList.at(nIndex);
        __ChangeScale(iData);
    }
}

void ScaleMenu::onShortKey(const QString &keyType)
{
    if (keyType == KeyStr::g_ctrl_smaller) {
        slotPrevScale();
    } else if (keyType == KeyStr::g_ctrl_larger || keyType == KeyStr::g_ctrl_equal) {
        slotNextScale();
    }
}

void ScaleMenu::__ChangeScale(const int &iData)
{
    emit sigCurrentScale(iData);

    m_nCurrentIndex = dataList.indexOf(iData);

    QJsonObject obj;
    obj.insert("content", QString::number(iData));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    dApp->m_pModelService->notifyMsg(MSG_FILE_SCALE, doc.toJson(QJsonDocument::Compact));

    auto actionList = this->findChildren<QAction *>();
    foreach (auto a, actionList) {
        int nData = a->data().toInt();
        if (nData == iData) {
            a->setChecked(true);
            break;
        }
    }
}

void ScaleMenu::OnFileOpenOk(const QString &sPath)
{
    FileDataModel fdm = MainTabWidgetEx::Instance()->qGetFileData(sPath);
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
