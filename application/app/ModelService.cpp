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
#include "ModelService.h"
#include "IObserver.h"
#include "MsgHeader.h"

ModelService::ModelService(QObject *parent)
    : QObject(parent)
{

}

void ModelService::addObserver(IObserver *obs)
{
    m_observerList.append(obs);
}

void ModelService::removeObserver(IObserver *obs)
{
    m_observerList.removeOne(obs);
}

void ModelService::notifyMsg(const int &msgType, const QString &msgContent)
{
    QListIterator<IObserver *> iter(m_observerList);
    while (iter.hasNext()) {
        auto obs = iter.next();
        if (obs != nullptr) {
            int nRes = obs->dealWithData(msgType, msgContent);
            if (nRes == MSG_OK) {
                break;
            }
        }
    }
}
