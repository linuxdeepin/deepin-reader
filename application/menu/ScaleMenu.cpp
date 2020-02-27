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

    dApp->m_pModelService->addObserver(this);
}

ScaleMenu::~ScaleMenu()
{
    dApp->m_pModelService->removeObserver(this);
}

int ScaleMenu::dealWithData(const int &, const QString &)
{
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

void ScaleMenu::__ChangeScale(const int &iData)
{
    m_nCurrentIndex = dataList.indexOf(iData);

    MsgModel mm;
    mm.setMsgType(MSG_FILE_SCALE);
    mm.setValue(QString::number(iData));

    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    mm.setPath(sCurPath);

    notifyMsg(E_TITLE_MSG_TYPE, mm.toJson());
}
