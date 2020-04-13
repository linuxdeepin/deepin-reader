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
#include <QUuid>

#include "FileDataModel.h"
#include "DocSheet.h"
#include "application.h"

DocTabBar::DocTabBar(QWidget *parent)
    : DTabBar(parent)
{
    this->setTabsClosable(true);

    this->setMovable(true);

    this->expanding();

    this->setElideMode(Qt::ElideMiddle);

    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(SlotTabCloseRequested(int)));

    connect(this, SIGNAL(tabAddRequested()), SLOT(SlotTabAddRequested()));

    connect(this, SIGNAL(currentChanged(int)), SLOT(onTabChanged(int)));

    connect(this, &DTabBar::tabReleaseRequested, this, &DocTabBar::handleTabReleased);

    connect(this, &DTabBar::tabDroped, this, &DocTabBar::handleTabDroped);

    connect(this, &DTabBar::dragActionChanged, this, &DocTabBar::handleDragActionChanged);

    setDragable(true);

}

DocTabBar::~DocTabBar()
{

}

int DocTabBar::indexOfFilePath(const QString &filePath)
{
    //修改成根据文件的绝对路径查重
    DocSheet *docSheet{nullptr};
    for (int i = 0; i < count(); ++i) {
//        if (this->tabData(i).toString() == filePath)
//            return i;
        docSheet = DocSheet::getSheet(this->tabData(i).toString());
        if (docSheet && (docSheet->filePath() == filePath)) {
            return i;
        }
    }
    return -1;
}

void DocTabBar::insertSheet(DocSheet *sheet, int index)
{
    QString fileName = getFileName(sheet->filePath());

    if (-1 == index)
        index = addTab(fileName);
    else
        index = insertTab(index, fileName);

    this->setTabData(index, DocSheet::getUuid(sheet));

    this->setTabMinimumSize(index, QSize(140, 36));

    this->setCurrentIndex(index);
}

void DocTabBar::removeSheet(DocSheet *sheet)
{
    for (int i = 0; i < count(); ++i) {
        if (DocSheet::getSheet(this->tabData(i).toString()) == sheet) {
            removeTab(i);
            return;
        }
    }
}

void DocTabBar::showSheet(DocSheet *sheet)
{
    for (int i = 0; i < count(); ++i) {
        if (DocSheet::getSheet(this->tabData(i).toString()) == sheet) {
            setCurrentIndex(i);
            return;
        }
    }
}

QMimeData *DocTabBar::createMimeDataFromTab(int index, const QStyleOptionTab &option) const
{
    const QString tabName = tabText(index);

    QMimeData *mimeData = new QMimeData;

    mimeData->setData("deepin_reader/tabbar", tabName.toUtf8());

    mimeData->setData("deepin_reader/uuid", this->tabData(index).toByteArray());

    return mimeData;
}

void DocTabBar::insertFromMimeDataOnDragEnter(int index, const QMimeData *source)
{
    const QString tabName = QString::fromUtf8(source->data("deepin_reader/tabbar"));

    insertTab(index, tabName);

    setTabMinimumSize(index, QSize(140, 36));
}

void DocTabBar::insertFromMimeData(int index, const QMimeData *source)
{
    QString id = source->data("deepin_reader/uuid");

    DocSheet *sheet = DocSheet::getSheet(id);

    if (nullptr != sheet) {
        sigTabMoveIn(sheet);
        insertSheet(sheet, index);
    }
}

bool DocTabBar::canInsertFromMimeData(int index, const QMimeData *source) const
{
    return source->hasFormat("deepin_reader/tabbar");
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

QString DocTabBar::getFileName(const QString &strFilePath)
{
    int nLastPos = strFilePath.lastIndexOf('/');
    nLastPos++;
    return strFilePath.mid(nLastPos);
}

void DocTabBar::onTabChanged(int index)
{
    QString id = tabData(index).toString();

    sigTabChanged(DocSheet::getSheet(id));

}

void DocTabBar::handleTabReleased(int index)
{
    if (count() <= 1)
        return;

    DocSheet *sheet = DocSheet::getSheet(this->tabData(index).toString());

    if (nullptr == sheet)
        return;

    removeTab(index);

    emit sigTabNewWindow(sheet);
}

void DocTabBar::handleTabDroped(int index, Qt::DropAction da, QObject *target)
{
    Q_UNUSED(da)    //同程序da可以根据目标传回，跨程序全是copyAction

    DocSheet *sheet = DocSheet::getSheet(this->tabData(index).toString());

    if (nullptr == sheet)
        return;

    if (nullptr == target) {
        //如果是空则为新建窗口
        if (count() <= 0) {//如果是最后一个，不允许
            return;
        }
        removeTab(index);
        emit sigTabNewWindow(sheet);
    } else if (Qt::MoveAction == da) {
        //如果是移动
        removeTab(index);
        emit sigTabMoveOut(sheet);
    }
}

void DocTabBar::onDroped()
{
    if (count() <= 0) {
        emit sigLastTabMoved();
    }
}

//  新增
void DocTabBar::SlotTabAddRequested()
{
    emit sigNeedOpenFilesExec();
}

//  关闭
void DocTabBar::SlotTabCloseRequested(int index)
{
    DocSheet *sheet = DocSheet::getSheet(this->tabData(index).toString());

    if (nullptr == sheet)
        return;

    emit sigTabClosed(sheet);
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
    }
}

//  打开成功了， 将标志位 置  111
void DocTabBar::SlotOpenFileResult(const QString &filePath, const bool &res)
{
    if (!res) {
        SlotRemoveFileTab(filePath);
    }
}
