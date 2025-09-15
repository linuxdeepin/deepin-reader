// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DocTabBar.h"
#include "DocSheet.h"
#include "Application.h"
#include "Global.h"

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
    qDebug() << "Initializing DocTabBar...";
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
    qDebug() << "DocTabBar initialized";
}

int DocTabBar::indexOfFilePath(const QString &filePath)
{
    qDebug() << "DocTabBar::indexOfFilePath start - searching for:" << filePath;
    //修改成根据文件的绝对路径查重
    for (int i = 0; i < count(); ++i) {
        DocSheet *docSheet = DocSheet::getSheet(this->tabData(i).toString());
        if (docSheet && (docSheet->filePath() == filePath)) {
            qDebug() << "Found file at index:" << i;
            return i;
        }
    }
    qDebug() << "File not found, returning -1";
    return -1;
}

void DocTabBar::insertSheet(DocSheet *sheet, int index)
{
    qDebug() << "DocTabBar::insertSheet start - inserting sheet:" << sheet->filePath();
    if (sheet == nullptr) {
        qWarning() << "Cannot insert null sheet";
        return;
    }

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
    qDebug() << "DocTabBar::insertSheet end";
}

void DocTabBar::removeSheet(DocSheet *sheet)
{
    qDebug() << "Removing sheet:" << (sheet ? sheet->filePath() : "null");
    for (int i = 0; i < count(); ++i) {
        if (DocSheet::getSheet(this->tabData(i).toString()) == sheet) {
            removeTab(i);
            updateTabWidth();
            return;
        }
    }
    qWarning() << "Sheet not found in tab bar";
}

void DocTabBar::showSheet(DocSheet *sheet)
{
    qDebug() << "DocTabBar::showSheet start - showing sheet:" << sheet->filePath();
    for (int i = 0; i < count(); ++i) {
        if (DocSheet::getSheet(this->tabData(i).toString()) == sheet) {
            this->setCurrentIndex(i);
            return;
        }
    }
    qDebug() << "DocTabBar::showSheet end";
}

QList<DocSheet *> DocTabBar::getSheets()
{
    qDebug() << "DocTabBar::getSheets start";
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

    qDebug() << "DocTabBar::getSheets end";
    return orderSheets;
}

void DocTabBar::updateTabWidth()
{
    qDebug() << "DocTabBar::updateTabWidth start";
    int tabCount = count();
    if (tabCount != 0) {
        qDebug() << "DocTabBar::updateTabWidth - tabCount:" << tabCount;
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
    qDebug() << "DocTabBar::updateTabWidth end";
}

QMimeData *DocTabBar::createMimeDataFromTab(int index, const QStyleOptionTab &) const
{
    qDebug() << "DocTabBar::createMimeDataFromTab start - index:" << index;
    const QString tabName = tabText(index);

    QMimeData *mimeData = new QMimeData;

    mimeData->setData("deepin_reader/tabbar", tabName.toUtf8());

    mimeData->setData("deepin_reader/uuid", this->tabData(index).toByteArray());

    qDebug() << "DocTabBar::createMimeDataFromTab end";
    return mimeData;
}

void DocTabBar::insertFromMimeDataOnDragEnter(int index, const QMimeData *source)
{
    qDebug() << "DocTabBar::insertFromMimeDataOnDragEnter start - index:" << index;
    const QString tabName = QString::fromUtf8(source->data("deepin_reader/tabbar"));

    if (tabName.isEmpty())
        return;

    emit sigNeedActivateWindow();

    insertTab(index, tabName);

    this->setTabToolTip(index, tabName);

    updateTabWidth();
    qDebug() << "DocTabBar::insertFromMimeDataOnDragEnter end";
}

void DocTabBar::insertFromMimeData(int index, const QMimeData *source)
{
    qDebug() << "DocTabBar::insertFromMimeData start - index:" << index;
    QString id = source->data("deepin_reader/uuid");

    if (id.isEmpty())
        return;

    DocSheet *sheet = DocSheet::getSheet(id);

    if (nullptr != sheet) {
        qDebug() << "DocTabBar::insertFromMimeData - inserting sheet:" << sheet->filePath();
        insertSheet(sheet, index);
        sigTabMoveIn(sheet);
    }
    qDebug() << "DocTabBar::insertFromMimeData end";
}

bool DocTabBar::canInsertFromMimeData(int, const QMimeData *source) const
{
    qDebug() << "DocTabBar::canInsertFromMimeData start - source:" << source;
    return source->hasFormat("deepin_reader/tabbar");
}

void DocTabBar::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "DocTabBar::dragEnterEvent start";
    DTabBar::dragEnterEvent(event);
    if (event->mimeData()->hasFormat("deepin_reader/tabbar")) {
        qDebug() << "DocTabBar::dragEnterEvent - hasFormat";
        QTimer::singleShot(1, [this]() {
            DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
            QGuiApplication::changeOverrideCursor(Qt::DragCopyCursor);
            DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), true);
        });
    }
    qDebug() << "DocTabBar::dragEnterEvent end";
}

void DocTabBar::resizeEvent(QResizeEvent *e)
{
    // qDebug() << "DocTabBar::resizeEvent start";
    DTabBar::resizeEvent(e);
    updateTabWidth();
    // qDebug() << "DocTabBar::resizeEvent end";
}

void DocTabBar::onDragActionChanged(Qt::DropAction action)
{
    qDebug() << "DocTabBar::onDragActionChanged start - action:" << action;
    if (nullptr == dragIconWindow())    //不存在拖拽窗口执行setDisableWindowOverrideCursor会导致崩溃
        return;

    if (action == Qt::IgnoreAction) {
        qDebug() << "DocTabBar::onDragActionChanged - action:" << action;
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
        if (count() <= 1)
            QGuiApplication::changeOverrideCursor(Qt::ForbiddenCursor);
        else
            QGuiApplication::changeOverrideCursor(Qt::DragCopyCursor);
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), true);
    } else if (action == Qt::CopyAction) {
        qDebug() << "DocTabBar::onDragActionChanged - action:" << action;
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
        QGuiApplication::changeOverrideCursor(Qt::ArrowCursor);
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), true);
    } else if (dragIconWindow()) {
        qDebug() << "DocTabBar::onDragActionChanged - action:" << action;
        DPlatformWindowHandle::setDisableWindowOverrideCursor(dragIconWindow(), false);
        if (QGuiApplication::overrideCursor())
            QGuiApplication::changeOverrideCursor(QGuiApplication::overrideCursor()->shape());
    }
    qDebug() << "DocTabBar::onDragActionChanged end";
}

void DocTabBar::onTabChanged(int index)
{
    qDebug() << "Tab changed to index:" << index;
    QString id = tabData(index).toString();
    DocSheet *sheet = DocSheet::getSheet(id);
    if (sheet) {
        qInfo() << "Switched to sheet:" << sheet->filePath();
    } else {
        qWarning() << "No sheet found for tab index:" << index;
    }
    sigTabChanged(sheet);
    qDebug() << "DocTabBar::onTabChanged end";
}

void DocTabBar::onTabReleased(int)
{
    qDebug() << "Tab released";
    if (count() <= 1) {
        qDebug() << "Only one tab, ignoring release";
        return;
    }

    int dropIndex = currentIndex();     //使用dropIndex替代index ,因为index是记录刚drag的index，当拖拽的时候几个item被移动了就会出错

    DocSheet *sheet = DocSheet::getSheet(this->tabData(dropIndex).toString());

    if (nullptr == sheet)
        return;

    removeTab(dropIndex);

    emit sigTabNewWindow(sheet);
    qDebug() << "DocTabBar::onTabReleased end";
}

void DocTabBar::onTabDroped(int, Qt::DropAction da, QObject *target)
{
    qDebug() << "DocTabBar::onTabDroped start - da:" << da;
    Q_UNUSED(da)    //同程序da可以根据目标传回，跨程序全是copyAction

    int dropIndex = currentIndex();     //使用dropIndex替代index ,因为index是记录刚drag的index，当拖拽的时候几个item被移动了就会出错

    DocSheet *sheet = DocSheet::getSheet(this->tabData(dropIndex).toString());

    if (nullptr == sheet)
        return;

    if (nullptr == target) {
        qDebug() << "DocTabBar::onTabDroped - target is null";
        //如果是空则为新建窗口
        if (count() <= 1) {//如果是最后一个，不允许
            return;
        }
        removeTab(dropIndex);
        emit sigTabNewWindow(sheet);
    } else if (Qt::MoveAction == da) {
        qDebug() << "DocTabBar::onTabDroped - da is MoveAction";
        //如果是移动
        removeTab(dropIndex);
        emit sigTabMoveOut(sheet);
    }
    qDebug() << "DocTabBar::onTabDroped end";
}

void DocTabBar::onSetCurrentIndex()
{
    qDebug() << "DocTabBar::onSetCurrentIndex start";
    setCurrentIndex(m_delayIndex);
}

void DocTabBar::onTabCloseRequested(int index)
{
    qDebug() << "Tab close requested for index:" << index;
    if (m_intervalTimer->isActive()) {
        qDebug() << "Close operation throttled, ignoring";
        return;
    }

    DocSheet *sheet = DocSheet::getSheet(this->tabData(index).toString());

    if (nullptr == sheet)
        return;

    emit sigTabClosed(sheet);

    m_intervalTimer->start(100);    //100ms内的重复点击将被过滤
    qDebug() << "DocTabBar::onTabCloseRequested end";
}

