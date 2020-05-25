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

#include "DocSheet.h"
#include "application.h"

DocTabBar::DocTabBar(QWidget *parent)
    : DTabBar(parent)
{
#if (DTK_VERSION_MAJOR > 5 || (DTK_VERSION_MAJOR >= 5 && DTK_VERSION_MINOR >= 2 ))
    this->setEnabledEmbedStyle(true);//设置直角样式
    this->setExpanding(true);//设置平铺窗口模式
#endif

    this->setTabsClosable(true);
    this->setMovable(true);
    this->setElideMode(Qt::ElideMiddle);
    this->setVisibleAddButton(true);
    this->setDragable(true);
    this->setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(onTabCloseRequested(int)));

    connect(this, SIGNAL(tabAddRequested()), SLOT(onTabAddRequested()));

    connect(this, SIGNAL(currentChanged(int)), SLOT(onTabChanged(int)));

    connect(this, &DTabBar::tabReleaseRequested, this, &DocTabBar::onTabReleased);

    connect(this, &DTabBar::tabDroped, this, &DocTabBar::onTabDroped);

    connect(this, &DTabBar::dragActionChanged, this, &DocTabBar::onDragActionChanged);
}

DocTabBar::~DocTabBar()
{

}

int DocTabBar::indexOfFilePath(const QString &filePath)
{
    //修改成根据文件的绝对路径查重
    DocSheet *docSheet{nullptr};
    for (int i = 0; i < count(); ++i) {
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

    this->setTabMinimumSize(index, QSize(140, 36));

    this->setTabData(index, DocSheet::getUuid(sheet));

    updateTabWidth(92);

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
            this->setCurrentIndex(i);
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

    if (tabName.isEmpty())
        return;

    emit sigNeedActivateWindow();

    insertTab(index, tabName);

    setTabMinimumSize(index, QSize(140, 36));

    updateTabWidth(143);

}

void DocTabBar::insertFromMimeData(int index, const QMimeData *source)
{
    QString id = source->data("deepin_reader/uuid");

    if (id.isEmpty())
        return;

    DocSheet *sheet = DocSheet::getSheet(id);

    if (nullptr != sheet) {
        insertSheet(sheet, index);
        sigTabMoveIn(sheet);
    }
}

bool DocTabBar::canInsertFromMimeData(int index, const QMimeData *source) const
{
    return source->hasFormat("deepin_reader/tabbar");
}

void DocTabBar::onDragActionChanged(Qt::DropAction action)
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

void DocTabBar::resizeEvent(QResizeEvent *event)
{
    DTabBar::resizeEvent(event);
    updateTabWidth(187);
    this->resize(this->width(), 36);
    this->update();
}

QString DocTabBar::getFileName(const QString &strFilePath)
{
    int nLastPos = strFilePath.lastIndexOf('/');
    nLastPos++;
    return strFilePath.mid(nLastPos);
}

void DocTabBar::updateTabWidth(int line)
{
    int tabWidth = 100;
    if (count() != 0) {
        tabWidth = (this->width() - 40) / count();
        for (int i = 0; i < count(); i++) {
            if (tabWidth <= 140) {
                setUsesScrollButtons(true);
                // 此处设置最小高度为36是为了能够在resize的时候进行重绘
                setTabMinimumSize(i, QSize(140, 37));
            } else {
                setUsesScrollButtons(false);
                setTabMinimumSize(i, QSize(tabWidth, 37));
            }
        }
    }

}

void DocTabBar::onTabChanged(int index)
{
    QString id = tabData(index).toString();

    sigTabChanged(DocSheet::getSheet(id));

}

void DocTabBar::onTabReleased(int index)
{
    if (count() <= 1)
        return;

    DocSheet *sheet = DocSheet::getSheet(this->tabData(index).toString());

    if (nullptr == sheet)
        return;

    removeTab(index);

    emit sigTabNewWindow(sheet);
}

void DocTabBar::onTabDroped(int index, Qt::DropAction da, QObject *target)
{
    Q_UNUSED(da)    //同程序da可以根据目标传回，跨程序全是copyAction

    DocSheet *sheet = DocSheet::getSheet(this->tabData(index).toString());

    if (nullptr == sheet)
        return;

    if (nullptr == target) {
        //如果是空则为新建窗口
        if (count() <= 1) {//如果是最后一个，不允许
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
void DocTabBar::onTabAddRequested()
{
    emit sigNeedOpenFilesExec();
}

//  关闭
void DocTabBar::onTabCloseRequested(int index)
{
    DocSheet *sheet = DocSheet::getSheet(this->tabData(index).toString());

    if (nullptr == sheet)
        return;

    emit sigTabClosed(sheet);
}

