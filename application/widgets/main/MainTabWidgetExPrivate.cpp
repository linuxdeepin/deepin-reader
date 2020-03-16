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
#include "MainTabWidgetExPrivate.h"

#include <QDesktopServices>

#include "MainTabWidgetEx.h"
#include "MainSplitter.h"

MainTabWidgetExPrivate::MainTabWidgetExPrivate(MainTabWidgetEx *cw)
    : q_ptr(cw)
{
    m_pMsgList = {E_APP_MSG_TYPE, E_TABBAR_MSG_TYPE, MSG_FILE_IS_CHANGE};
}

//  打开当前所在文件夹
void MainTabWidgetExPrivate::OpenCurFileFolder()
{
    QString sPath = GetCurPath();
    if (sPath != "") {
        int nLastPos = sPath.lastIndexOf('/');
        sPath = sPath.mid(0, nLastPos);
        sPath = QString("file://") + sPath;
        QDesktopServices::openUrl(QUrl(sPath));
    }
}

QStringList MainTabWidgetExPrivate::GetAllPath()
{
    Q_Q(MainTabWidgetEx);
    QStringList pathList;

    auto splitterList = q->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath != "") {
            pathList.append(sSplitterPath);
        }
    }
    return pathList;
}

int MainTabWidgetExPrivate::GetFileChange(const QString &sPath)
{
    Q_Q(MainTabWidgetEx);

    auto splitterList = q->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            return  s->qGetFileChange();
        }
    }
    return -1;
}

QString MainTabWidgetExPrivate::GetCurPath()
{
    Q_Q(MainTabWidgetEx);

    auto splitterList = q->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        if (s->isVisible()) {
            return s->qGetPath();
        }
    }
    return "";
}

void MainTabWidgetExPrivate::SetFileChange(const QString &sPath, const int &iState)
{
    auto splitterList = q_ptr->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            s->qSetFileChange(iState);
            break;
        }
    }
}

