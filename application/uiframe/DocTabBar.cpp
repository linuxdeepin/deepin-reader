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
#include "DocTabBar.h"

#include <QDebug>
#include <DPlatformWindowHandle>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QProcess>
#include <QDir>
#include <QTimer>

#include "CentralDocPage.h"
#include "FileDataModel.h"
#include "MainWindow.h"
#include "gof/bridge/IHelper.h"
#include "TitleMenu.h"
#include "TitleWidget.h"
#include "app/ProcessController.h"
#include "business/SaveDialog.h"

DocTabBar::DocTabBar(DWidget *parent)
    : DTabBar(parent)
{
    m_strObserverName = "DocTabBar";

    this->setTabsClosable(true);

    this->setMovable(true);

    this->expanding();

    this->setElideMode(Qt::ElideMiddle);

    m_pMsgList = {MSG_TAB_ADD, MSG_MENU_NEW_TAB};

    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(SlotTabCloseRequested(int)));

    connect(this, SIGNAL(tabAddRequested()), SLOT(SlotTabAddRequested()));

    connect(this, SIGNAL(currentChanged(int)), SLOT(SlotCurrentChanged(int)));

    dApp->m_pModelService->addObserver(this);

    connect(this, &DTabBar::tabReleaseRequested, this, &DocTabBar::handleTabReleased);

    connect(this, &DTabBar::tabDroped, this, &DocTabBar::handleTabDroped);

    connect(this, &DTabBar::dragActionChanged, this, &DocTabBar::handleDragActionChanged);

    setDragable(true);

}

DocTabBar::~DocTabBar()
{
    dApp->m_pModelService->removeObserver(this);
}

int DocTabBar::indexOfFilePath(const QString &filePath)
{
    for (int i = 0; i < count(); ++i) {
        if (this->tabData(i).toString() == filePath)
            return i;
    }
    return -1;
}

QMimeData *DocTabBar::createMimeDataFromTab(int index, const QStyleOptionTab &option) const
{
    const QString tabName = tabText(index);

    QMimeData *mimeData = new QMimeData;

    mimeData->setData("reader/tabbar", tabName.toUtf8());

    QStringList sDataList = this->tabData(index).toString().split(Constant::sQStringSep, QString::SkipEmptyParts);

    QString sPath = sDataList.value(0);

    mimeData->setData("reader/filePath", QByteArray().append(sPath));

    return mimeData;
}

void DocTabBar::insertFromMimeDataOnDragEnter(int index, const QMimeData *source)
{
    const QString tabName = QString::fromUtf8(source->data("reader/tabbar"));

    insertTab(index, tabName);

    setTabMinimumSize(index, QSize(140, 36));
}

void DocTabBar::insertFromMimeData(int index, const QMimeData *source)
{
    QString filePath = source->data("reader/filePath");

    AddFileTab(filePath + Constant::sQStringSep, index);
}

bool DocTabBar::canInsertFromMimeData(int index, const QMimeData *source) const
{
    //可以改为根据进程号当前进程不能给自己传
    return source->hasFormat("reader/tabbar");
}

void DocTabBar::handleDragActionChanged(Qt::DropAction action)
{
    if (count() <= 1) {
        QGuiApplication::changeOverrideCursor(Qt::ForbiddenCursor);
    } else if (action == Qt::TargetMoveAction) {
        QGuiApplication::changeOverrideCursor(Qt::DragMoveCursor);
    } else if (action == Qt::IgnoreAction) {
        QGuiApplication::changeOverrideCursor(Qt::DragCopyCursor);
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), true);
    } else if (dragIconWindow()) {
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
        if (QGuiApplication::overrideCursor())
            QGuiApplication::changeOverrideCursor(QGuiApplication::overrideCursor()->shape());
    }
}

int DocTabBar::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_TAB_ADD == msgType) {
        AddFileTab(msgContent);
    } else if (MSG_MENU_NEW_TAB == msgType) {
        SlotTabAddRequested();
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}

void DocTabBar::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

void DocTabBar::SlotCurrentChanged(int index)
{
    QString filePath = this->tabData(index).toString();
    emit sigTabBarIndexChange(filePath);
}

void DocTabBar::AddFileTab(const QString &sContent, int index)
{
    QStringList filePaths;

    QList<QString> sOpenFiles = CentralDocPage::Instance()->qGetAllPath();

    QStringList canOpenFileList = sContent.split(Constant::sQStringSep, QString::SkipEmptyParts);

    foreach (auto s, canOpenFileList) {
        if (!sOpenFiles.contains(s)) {
            filePaths.append(s);
        }
    }

    int nSize = filePaths.size();

    if (nSize > 0) {
        for (int i = 0; i < nSize; i++) {
            QString filePath = filePaths.at(i);
            if (filePath != "") {
                QString fileName = getFileName(filePath);

                if (index == -1)
                    index = this->addTab(fileName);
                else
                    index = this->insertTab(index, fileName);

                this->setTabData(index, filePath);
                this->setTabMinimumSize(index, QSize(140, 36));
                this->setCurrentIndex(index);
                emit sigAddTab(filePath);
            }
        }

        int nCurIndex = this->currentIndex();
        if (nCurIndex > -1) {
            SlotCurrentChanged(nCurIndex);
        }
    }

    if (canOpenFileList.count()  > 0)
        CentralDocPage::Instance()->setCurrentTabByFilePath(canOpenFileList.value(canOpenFileList.count() - 1));
}

QString DocTabBar::getFileName(const QString &strFilePath)
{
    int nLastPos = strFilePath.lastIndexOf('/');
    nLastPos++;
    return strFilePath.mid(nLastPos);
}

void DocTabBar::handleTabReleased(int index)
{
    if (count() <= 1)
        return;

    QStringList sDataList = this->tabData(index).toString().split(Constant::sQStringSep, QString::SkipEmptyParts);

    QString sPath = sDataList.value(0);

    CentralDocPage::Instance()->SaveFile(MSG_SAVE_FILE, sPath);

    removeTab(index);

    emit sigCloseTab(sPath);

    QProcess app;

    app.startDetached(QString("%1 \"%2\" newwindow").arg(qApp->applicationDirPath() + "/deepin-reader").arg(sPath));

    QTimer::singleShot(50, this, SLOT(onDroped()));
}

void DocTabBar::handleTabDroped(int index, Qt::DropAction da, QObject *target)
{
    Q_UNUSED(da)    //同程序da可以根据目标传回，跨程序全是copyAction

    if (nullptr != target) {//如果是本程序 同移出程序
        handleTabReleased(index);
        return;
    }

    QStringList sDataList = this->tabData(index).toString().split(Constant::sQStringSep, QString::SkipEmptyParts);

    QString sPath = sDataList.value(0);

    CentralDocPage::Instance()->SaveFile(MSG_SAVE_FILE, sPath);

    removeTab(index);

    emit sigCloseTab(sPath);

    QTimer::singleShot(50, this, SLOT(onDroped()));
}

void DocTabBar::onDroped()
{
    if (count() <= 0)
        MainWindow::Instance()->close();
}

//  新增
void DocTabBar::SlotTabAddRequested()
{
    notifyMsg(E_OPEN_FILE);
}

//  关闭
void DocTabBar::SlotTabCloseRequested(int index)
{
    QString filePath = this->tabData(index).toString();
    emit sigCloseTab(filePath);
}

void DocTabBar::SlotRemoveFileTab(const QString &sPath)
{
    if (sPath != "") {
        int nCount = this->count();
        for (int i = 0; i < nCount; i++) {
            if (sPath == this->tabData(i).toString()) {
                this->removeTab(i);
                break;
            }
        }

        nCount = this->count();
        if (nCount == 0) {
            //禁用所有的功能
            TitleMenu::Instance()->disableallaction();
            TitleWidget::Instance()->suitDocType(DocType_NULL);
            notifyMsg(CENTRAL_INDEX_CHANGE);

        }
    }
}

//  打开成功了， 将标志位 置  111
void DocTabBar::SlotOpenFileResult(const QString &filePath, const bool &res)
{
    if (!res) {
        SlotRemoveFileTab(filePath);
    }
}
