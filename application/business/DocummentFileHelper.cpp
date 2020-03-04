/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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

#include "DocummentFileHelper.h"

#include "gof/bridge/IHelper.h"

DocummentFileHelper::DocummentFileHelper(QObject *parent)
    : QObject(parent)
{
    m_strObserverName = "DocummentFileHelper";

    initConnections();

    dApp->m_pModelService->addObserver(this);
}

void DocummentFileHelper::initConnections()
{
    connect(this, SIGNAL(sigFileSlider(const int &)), SLOT(slotFileSlider(const int &)));
}

//  处理 应用推送的消息数据
int DocummentFileHelper::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_SLIDE) {
        emit sigFileSlider(1);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (KeyStr::g_esc == msgContent) {
            emit sigFileSlider(0);
        }
    }
    return MSG_NO_OK;
}

//  发送 操作消息
//void DocummentFileHelper::sendMsg(const int &, const QString &) {}

//  通知消息, 不需要撤回
void DocummentFileHelper::notifyMsg(const int &, const QString &) {}

/**
 * @brief DocummentFileHelper::slotFileSlider
 * @param nFlag 1 开启放映, 0 退出放映
 */
void DocummentFileHelper::slotFileSlider(const int &nFlag)
{
    dApp->m_pHelper->qDealWithData(MSG_OPERATION_SLIDE, QString::number(nFlag));
}

