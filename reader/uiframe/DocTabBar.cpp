// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DocTabBar.h"
#include "DocSheet.h"
#include "Application.h"
#include "Global.h"
#include "ddlog.h"

#include <DPlatformWindowHandle>

#include <QMimeData>
#include <QDir>
#include <QTimer>
#include <QUuid>
#include <QWindow>
#include <QLayout>

DocTabBar::DocTabBar(QWidget *parent)
    : DTabBar(parent)
{
    qCDebug(appLog) << "Initializing DocTabBar...";
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

    connect(this, SIGNAL(tabAddRequested()), SIGNAL(sigNeedOpenFilesExec()));

    connect(this, SIGNAL(currentChanged(int)), SLOT(onTabChanged(int)));

    connect(this, &DTabBar::tabReleaseRequested, this, &DocTabBar::onTabReleased);

    connect(this, &DTabBar::tabDroped, this, &DocTabBar::onTabDroped);

    connect(this, &DTabBar::dragActionChanged, this, &DocTabBar::onDragActionChanged);

    m_intervalTimer = new QTimer(this);

    m_intervalTimer->setSingleShot(true);
    qCDebug(appLog) << "DocTabBar initialized";
}

int DocTabBar::indexOfFilePath(const QString &filePath)
{
    qCDebug(appLog) << "DocTabBar::indexOfFilePath start - searching for:" << filePath;
    //修改成根据文件的绝对路径查重
    for (int i = 0; i < count(); ++i) {
        DocSheet *docSheet = DocSheet::getSheet(this->tabData(i).toString());
        if (docSheet && (docSheet->filePath() == filePath)) {
            qCDebug(appLog) << "Found file at index:" << i;
            return i;
        }
    }
    qCDebug(appLog) << "File not found, returning -1";
    return -1;
}

void DocTabBar::insertSheet(DocSheet *sheet, int index)
{
    if (sheet == nullptr) {
        qCWarning(appLog) << "Cannot insert null sheet";
        return;
    }
    qCDebug(appLog) << "DocTabBar::insertSheet start - inserting sheet:" << sheet->filePath();

    QString fileName = QFileInfo(sheet->filePath()).fileName();

    if (-1 == index)
        index = addTab(fileName);
    else
        index = insertTab(index, fileName);

    this->setTabToolTip(index, fileName);

    this->setTabData(index, DocSheet::getUuid(sheet));

    updateTabWidth();

    m_delayIndex = index;

    QTimer::singleShot(1, this, SLOT(onSetCurrentIndex()));
    qCDebug(appLog) << "DocTabBar::insertSheet end";
}

void DocTabBar::removeSheet(DocSheet *sheet)
{
    qCDebug(appLog) << "Removing sheet:" << (sheet ? sheet->filePath() : "null");
    for (int i = 0; i < count(); ++i) {
        if (DocSheet::getSheet(this->tabData(i).toString()) == sheet) {
            removeTab(i);
            updateTabWidth();
            return;
        }
    }
    qCWarning(appLog) << "Sheet not found in tab bar";
}

void DocTabBar::showSheet(DocSheet *sheet)
{
    qCDebug(appLog) << "DocTabBar::showSheet start - showing sheet:" << sheet->filePath();
    for (int i = 0; i < count(); ++i) {
        if (DocSheet::getSheet(this->tabData(i).toString()) == sheet) {
            this->setCurrentIndex(i);
            return;
        }
    }
    qCDebug(appLog) << "DocTabBar::showSheet end";
}

QList<DocSheet *> DocTabBar::getSheets()
{
    qCDebug(appLog) << "DocTabBar::getSheets start";
    QList<DocSheet *> sheets;

    for (int i = 0; i < count(); ++i) {
        DocSheet *sheet = DocSheet::getSheet(this->tabData(i).toString());
        if (nullptr != sheet)
            sheets.append(sheet);
    }

    if (sheets.isEmpty())
        return sheets;

    //需要按倒序排列
    QList<DocSheet *> orderSheets;
    QList<DocSheet *> allSheets = DocSheet::getSheets();
    for (int i = allSheets.count() - 1; i >= 0; --i) {
        if (sheets.contains(allSheets[i]))
            orderSheets.append(allSheets[i]);
    }

    qCDebug(appLog) << "DocTabBar::getSheets end";
    return orderSheets;
}

void DocTabBar::updateTabWidth()
{
    qCDebug(appLog) << "DocTabBar::updateTabWidth start";
    int tabCount = count();
    if (tabCount != 0) {
        qCDebug(appLog) << "DocTabBar::updateTabWidth - tabCount:" << tabCount;
        int tabWidth = (this->width() - 40) / tabCount - (tabCount - 1) * 10;
        for (int i = 0; i < count(); i++) {
            if (tabWidth <= 140) {
                setUsesScrollButtons(true);
                setTabMinimumSize(i, QSize(140, 37));
                setTabMaximumSize(i, QSize(140, 37));
            } else {
                setUsesScrollButtons(false);
                setTabMinimumSize(i, QSize(tabWidth, 37));
                setTabMaximumSize(i, QSize(tabWidth, 37));
            }
        }
    }
    qCDebug(appLog) << "DocTabBar::updateTabWidth end";
}

QMimeData *DocTabBar::createMimeDataFromTab(int index, const QStyleOptionTab &) const
{
    qCDebug(appLog) << "DocTabBar::createMimeDataFromTab start - index:" << index;
    const QString tabName = tabText(index);

    QMimeData *mimeData = new QMimeData;

    mimeData->setData("deepin_reader/tabbar", tabName.toUtf8());

    mimeData->setData("deepin_reader/uuid", this->tabData(index).toByteArray());

    qCDebug(appLog) << "DocTabBar::createMimeDataFromTab end";
    return mimeData;
}

void DocTabBar::insertFromMimeDataOnDragEnter(int index, const QMimeData *source)
{
    qCDebug(appLog) << "DocTabBar::insertFromMimeDataOnDragEnter start - index:" << index;
    const QString tabName = QString::fromUtf8(source->data("deepin_reader/tabbar"));

    if (tabName.isEmpty())
        return;

    emit sigNeedActivateWindow();

    insertTab(index, tabName);

    this->setTabToolTip(index, tabName);

    updateTabWidth();
    qCDebug(appLog) << "DocTabBar::insertFromMimeDataOnDragEnter end";
}

void DocTabBar::insertFromMimeData(int index, const QMimeData *source)
{
    qCDebug(appLog) << "DocTabBar::insertFromMimeData start - index:" << index;
    QString id = source->data("deepin_reader/uuid");

    if (id.isEmpty())
        return;

    DocSheet *sheet = DocSheet::getSheet(id);

    if (nullptr != sheet) {
        qCDebug(appLog) << "DocTabBar::insertFromMimeData - inserting sheet:" << sheet->filePath();
        insertSheet(sheet, index);
        sigTabMoveIn(sheet);
    }
    qCDebug(appLog) << "DocTabBar::insertFromMimeData end";
}

bool DocTabBar::canInsertFromMimeData(int, const QMimeData *source) const
{
    qCDebug(appLog) << "DocTabBar::canInsertFromMimeData start - source:" << source;
    return source->hasFormat("deepin_reader/tabbar");
}

void DocTabBar::dragEnterEvent(QDragEnterEvent *event)
{
    qCDebug(appLog) << "DocTabBar::dragEnterEvent start";
    DTabBar::dragEnterEvent(event);
    if (event->mimeData()->hasFormat("deepin_reader/tabbar")) {
        qCDebug(appLog) << "DocTabBar::dragEnterEvent - hasFormat";
        QTimer::singleShot(1, [this]() {
            DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
            QGuiApplication::changeOverrideCursor(Qt::DragCopyCursor);
            DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), true);
        });
    }
    qCDebug(appLog) << "DocTabBar::dragEnterEvent end";
}

void DocTabBar::resizeEvent(QResizeEvent *e)
{
    // qCDebug(appLog) << "DocTabBar::resizeEvent start";
    DTabBar::resizeEvent(e);
    updateTabWidth();
    // qCDebug(appLog) << "DocTabBar::resizeEvent end";
}

void DocTabBar::onDragActionChanged(Qt::DropAction action)
{
    qCDebug(appLog) << "DocTabBar::onDragActionChanged start - action:" << action;
    if (nullptr == dragIconWindow())    //不存在拖拽窗口执行setDisableWindowOverrideCursor会导致崩溃
        return;

    if (action == Qt::IgnoreAction) {
        qCDebug(appLog) << "DocTabBar::onDragActionChanged - action:" << action;
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
        if (count() <= 1)
            QGuiApplication::changeOverrideCursor(Qt::ForbiddenCursor);
        else
            QGuiApplication::changeOverrideCursor(Qt::DragCopyCursor);
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), true);
    } else if (action == Qt::CopyAction) {
        qCDebug(appLog) << "DocTabBar::onDragActionChanged - action:" << action;
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
        QGuiApplication::changeOverrideCursor(Qt::ArrowCursor);
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), true);
    } else if (dragIconWindow()) {
        qCDebug(appLog) << "DocTabBar::onDragActionChanged - action:" << action;
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
        if (QGuiApplication::overrideCursor())
            QGuiApplication::changeOverrideCursor(QGuiApplication::overrideCursor()->shape());
    }
    qCDebug(appLog) << "DocTabBar::onDragActionChanged end";
}

void DocTabBar::onTabChanged(int index)
{
    qCDebug(appLog) << "Tab changed to index:" << index;
    QString id = tabData(index).toString();
    DocSheet *sheet = DocSheet::getSheet(id);
    if (sheet) {
        qCInfo(appLog) << "Switched to sheet:" << sheet->filePath();
    } else {
        qCWarning(appLog) << "No sheet found for tab index:" << index;
    }
    sigTabChanged(sheet);
    qCDebug(appLog) << "DocTabBar::onTabChanged end";
}

void DocTabBar::onTabReleased(int)
{
    qCDebug(appLog) << "Tab released";
    if (count() <= 1) {
        qCDebug(appLog) << "Only one tab, ignoring release";
        return;
    }

    int dropIndex = currentIndex();     //使用dropIndex替代index ,因为index是记录刚drag的index，当拖拽的时候几个item被移动了就会出错

    DocSheet *sheet = DocSheet::getSheet(this->tabData(dropIndex).toString());

    if (nullptr == sheet)
        return;

    removeTab(dropIndex);

    emit sigTabNewWindow(sheet);
    qCDebug(appLog) << "DocTabBar::onTabReleased end";
}

void DocTabBar::onTabDroped(int, Qt::DropAction da, QObject *target)
{
    qCDebug(appLog) << "DocTabBar::onTabDroped start - da:" << da;
    Q_UNUSED(da)    //同程序da可以根据目标传回，跨程序全是copyAction

    int dropIndex = currentIndex();     //使用dropIndex替代index ,因为index是记录刚drag的index，当拖拽的时候几个item被移动了就会出错

    DocSheet *sheet = DocSheet::getSheet(this->tabData(dropIndex).toString());

    if (nullptr == sheet)
        return;

    if (nullptr == target) {
        qCDebug(appLog) << "DocTabBar::onTabDroped - target is null";
        //如果是空则为新建窗口
        if (count() <= 1) {//如果是最后一个，不允许
            return;
        }
        removeTab(dropIndex);
        emit sigTabNewWindow(sheet);
    } else if (Qt::MoveAction == da) {
        qCDebug(appLog) << "DocTabBar::onTabDroped - da is MoveAction";
        //如果是移动
        removeTab(dropIndex);
        emit sigTabMoveOut(sheet);
    }
    qCDebug(appLog) << "DocTabBar::onTabDroped end";
}

void DocTabBar::onSetCurrentIndex()
{
    qCDebug(appLog) << "DocTabBar::onSetCurrentIndex start";
    setCurrentIndex(m_delayIndex);
}

void DocTabBar::onTabCloseRequested(int index)
{
    qCDebug(appLog) << "Tab close requested for index:" << index;
    if (m_intervalTimer->isActive()) {
        qCDebug(appLog) << "Close operation throttled, ignoring";
        return;
    }

    DocSheet *sheet = DocSheet::getSheet(this->tabData(index).toString());

    if (nullptr == sheet)
        return;

    emit sigTabClosed(sheet);

    m_intervalTimer->start(100);    //100ms内的重复点击将被过滤
    qCDebug(appLog) << "DocTabBar::onTabCloseRequested end";
}

