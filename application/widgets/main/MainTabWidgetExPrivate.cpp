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

MainTabWidgetExPrivate::MainTabWidgetExPrivate(CustomWidget *cw, QObject *parent)
    : CustomWidgetPrivate(cw, parent)
{
    m_pMsgList = {E_APP_MSG_TYPE, E_TABBAR_MSG_TYPE, MSG_FILE_IS_CHANGE};
}

QString MainTabWidgetExPrivate::getSliderPath() const
{
    return m_strSliderPath;
}

void MainTabWidgetExPrivate::setSliderPath(const QString &strSliderPath)
{
    m_strSliderPath = strSliderPath;
}

int MainTabWidgetExPrivate::getCurrentState() const
{
    return m_nCurrentState;
}

void MainTabWidgetExPrivate::setCurrentState(const int &nCurrentState)
{
    m_nCurrentState = nCurrentState;
}

void MainTabWidgetExPrivate::RemovePath(const QString &sPath)
{
    m_strOpenFileAndProxy.remove(sPath);
}

void MainTabWidgetExPrivate::InsertPathProxy(const QString &sPath, DocummentProxy *proxy)
{
    m_strOpenFileAndProxy.insert(sPath, proxy);
}

DocummentProxy *MainTabWidgetExPrivate::getCurFileAndProxy(const QString &sPath) const
{
    QString sTempPath = sPath;
    if (sTempPath == "") {
        sTempPath = qobject_cast<MainTabWidgetEx *>(q_ptr)->qGetCurPath();
    }
    if (m_strOpenFileAndProxy.contains(sTempPath)) {
        return m_strOpenFileAndProxy[sTempPath];
    }
    return  nullptr;
}

QMap<QString, DocummentProxy *> MainTabWidgetExPrivate::getOpenFileAndProxy() const
{
    return m_strOpenFileAndProxy;
}

//  打开当前所在文件夹
void MainTabWidgetExPrivate::OpenCurFileFolder()
{
    QString sPath = qobject_cast<MainTabWidgetEx *>(q_ptr)->qGetCurPath();
    if (sPath != "") {
        int nLastPos = sPath.lastIndexOf('/');
        sPath = sPath.mid(0, nLastPos);
        sPath = QString("file://") + sPath;
        QDesktopServices::openUrl(QUrl(sPath));
    }
}

QStringList MainTabWidgetExPrivate::GetAllPath() const
{
    QStringList pathList;

    auto splitterList = q_ptr->findChildren<MainSplitter *>();
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
    auto splitterList = q_ptr->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            return  s->qGetFileChange();
        }
    }
    return -1;
}

QString MainTabWidgetExPrivate::GetCurPath() const
{
    auto splitterList = q_ptr->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        if (s->isVisible()) {
            return s->qGetPath();
        }
    }
    return "";
}

FileDataModel MainTabWidgetExPrivate::GetFileData(const QString &sPath) const
{
    auto splitterList = q_ptr->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            return s->qGetFileData();
        }
    }
    return FileDataModel();
}

QString MainTabWidgetExPrivate::getMagniferPath() const
{
    return m_strMagniferPath;
}

void MainTabWidgetExPrivate::setMagniferPath(const QString &strMagniferPath)
{
    m_strMagniferPath = strMagniferPath;
}

void MainTabWidgetExPrivate::SetFileData(const QString &sPath, const FileDataModel &fdm)
{
    auto splitterList = q_ptr->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            s->setFileData(fdm);
            break;
        }
    }
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

