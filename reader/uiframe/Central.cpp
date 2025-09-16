// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Central.h"
#include "CentralNavPage.h"
#include "CentralDocPage.h"
#include "TitleMenu.h"
#include "TitleWidget.h"
#include "MainWindow.h"
#include "ShortCutShow.h"
#include "DBusObject.h"
#include "ddlog.h"

#include <DMessageManager>
#include <DFileDialog>

#include <QDragEnterEvent>
#include <QStackedLayout>
#include <QMimeData>
#include <QTimer>
#include <QDebug>

Central::Central(QWidget *parent)
    : BaseWidget(parent)
{
    qCDebug(appLog) << "Central widget initializing...";
    setAcceptDrops(true);

    m_widget = new TitleWidget(parent);
    m_navPage = new CentralNavPage(this);
    connect(m_navPage, SIGNAL(sigNeedOpenFilesExec()), SLOT(onOpenFilesExec()));

    m_mainWidget = new QWidget(this);
    m_layout = new QStackedLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_navPage);
    m_mainWidget->setLayout(m_layout);

    connect(DBusObject::instance(), &DBusObject::sigTouchPadEventSignal, this, &Central::onTouchPadEvent);

    QList<QKeySequence> keyList;
    keyList.append(QKeySequence::Find);
    keyList.append(QKeySequence::Open);
    keyList.append(QKeySequence::Print);
    keyList.append(QKeySequence::Save);
    keyList.append(QKeySequence::Copy);
    // 屏蔽右侧区域的切页快捷键，使用默认的滚动效果
    // when using slide mode, left & right keys are needed. modify by lz,at 2022.4.18
    keyList.append(QKeySequence(Qt::Key_Left));
    keyList.append(QKeySequence(Qt::Key_Right));

    keyList.append(QKeySequence(Qt::Key_Up));
    keyList.append(QKeySequence(Qt::Key_Down));
    keyList.append(QKeySequence(Qt::Key_Space));
    keyList.append(QKeySequence(Qt::Key_Escape));
    keyList.append(QKeySequence(Qt::Key_F5));
    keyList.append(QKeySequence(Qt::Key_F11));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_1));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_2));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_A));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_H));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_Z));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_1));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_2));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_3));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_D));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_M));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_R));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_Equal));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    keyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    keyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    keyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_Home));
    keyList.append(QKeySequence(Qt::CTRL | Qt::Key_End));

    foreach (auto key, keyList) {
        auto action = new QAction(this);
        action->setShortcut(key);
        this->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(onKeyTriggered()));
    }
}

Central::~Central()
{
    // qCDebug(appLog) << "Central widget destroying...";
}

TitleWidget *Central::titleWidget()
{
    // qCDebug(appLog) << "Getting title widget";
    return m_widget;
}

CentralDocPage *Central::docPage()
{
    // qCDebug(appLog) << "Getting doc page";
    if (nullptr == m_docPage) {
        // qCDebug(appLog) << "Creating doc page";
        m_docPage = new CentralDocPage(this);
        m_layout->addWidget(m_docPage);
        connect(m_docPage, SIGNAL(sigCurSheetChanged(DocSheet *)), m_menu, SLOT(onCurSheetChanged(DocSheet *)));
        connect(m_docPage, SIGNAL(sigCurSheetChanged(DocSheet *)), m_widget, SLOT(onCurSheetChanged(DocSheet *)));
        connect(m_docPage, SIGNAL(sigFindOperation(const int &)), m_widget, SLOT(onFindOperation(const int &)));
        connect(m_docPage, SIGNAL(sigNeedShowTips(QWidget *, const QString &, int)), this, SLOT(onShowTips(QWidget *, const QString &, int)));
        connect(m_docPage, SIGNAL(sigNeedClose()), this, SIGNAL(sigNeedClose()));
        connect(m_docPage, SIGNAL(sigSheetCountChanged(int)), this, SLOT(onSheetCountChanged(int)));
        connect(m_docPage, SIGNAL(sigNeedOpenFilesExec()), SLOT(onOpenFilesExec()));
        connect(m_docPage, SIGNAL(sigNeedActivateWindow()), this, SLOT(onNeedActivateWindow()));
    }
    // qCDebug(appLog) << "Getting doc page end";
    return m_docPage;
}

void Central::setMenu(TitleMenu *menu)
{
    // qCDebug(appLog) << "Setting menu";
    m_menu = menu;
    connect(m_menu, SIGNAL(sigActionTriggered(QString)), this, SLOT(onMenuTriggered(QString)));
}

void Central::addFilesWithDialog()
{
    qCDebug(appLog) << "Opening file selection dialog...";
    DFileDialog dialog(this);
    dialog.setFileMode(DFileDialog::ExistingFiles);
#ifdef XPS_SUPPORT_ENABLED
    dialog.setNameFilter(tr("Documents") + " (*.pdf *.djvu *.docx *.xps)");
#else
    dialog.setNameFilter(tr("Documents") + " (*.pdf *.djvu *.docx)");
#endif
    dialog.setDirectory(QDir::homePath());

    if (QDialog::Accepted != dialog.exec()) {
        qCDebug(appLog) << "File selection dialog canceled";
        return;
    }

    QStringList filePathList = dialog.selectedFiles();

    if (filePathList.count() <= 0) {
        qCWarning(appLog) << "No files selected in dialog";
        return;
    }

    QWidget *topLevelwidget = this->topLevelWidget();
    topLevelwidget->setProperty("loading", true);
    foreach (QString filePath, filePathList) {
        if (topLevelwidget->property("windowClosed").toBool())
            break;

        if (!MainWindow::activateSheetIfExist(filePath))
            docPage()->addFileAsync(filePath);
    }
    topLevelwidget->setProperty("loading", false);
}

void Central::addFileAsync(const QString &filePath)
{
    qCDebug(appLog) << "Adding file asynchronously:" << filePath;
    docPage()->addFileAsync(filePath);
}

void Central::addSheet(DocSheet *sheet)
{
    qCDebug(appLog) << "Adding sheet:" << sheet;
    docPage()->addSheet(sheet);
}

bool Central::hasSheet(DocSheet *sheet)
{
    qCDebug(appLog) << "Checking if sheet exists:" << sheet;
    return docPage()->hasSheet(sheet);
}

void Central::showSheet(DocSheet *sheet)
{
    qCDebug(appLog) << "Showing sheet:" << sheet;
    docPage()->showSheet(sheet);
}

QList<DocSheet *> Central::getSheets()
{
    // qCDebug(appLog) << "Getting sheets";
    if (nullptr == m_docPage)
        return QList<DocSheet *>();

    return docPage()->getSheets();
}

void Central::handleShortcut(const QString &shortcut)
{
    qCDebug(appLog) << "Handling shortcut:" << shortcut;
    if (shortcut == Dr::key_ctrl_o) {
        qCDebug(appLog) << "Handling Ctrl+O shortcut - opening file dialog";
        addFilesWithDialog();
    } if (shortcut == Dr::key_ctrl_shift_slash) { //  显示快捷键预览
        qCDebug(appLog) << "Showing shortcut preview";
        ShortCutShow show;
        show.setSheet(docPage()->getCurSheet());
        show.show();
    } else {
        qCDebug(appLog) << "Handling shortcut:" << shortcut;
        docPage()->handleShortcut(shortcut);
    }
}

bool Central::handleClose(bool needToBeSaved)
{
    qCDebug(appLog) << "Handling close:" << needToBeSaved;
    if (nullptr != m_docPage) {
        qCDebug(appLog) << __FUNCTION__ << "正在关闭所有 sheet ...";
        return docPage()->closeAllSheets(needToBeSaved);
    }
    qCWarning(appLog) << "No document page available to close";

    return true;
}

void Central::onSheetCountChanged(int count)
{
    qCDebug(appLog) << "Sheet count changed:" << count;
    if (count > 0) {
        qCDebug(appLog) << "Setting current index to 1";
        m_layout->setCurrentIndex(1);
    } else {
        qCDebug(appLog) << "Setting current index to 0";
        m_layout->setCurrentIndex(0);
        m_navPage->setFocus();
        m_widget->setControlEnabled(false);
    }
}

void Central::onMenuTriggered(const QString &action)
{
    qCDebug(appLog) << "Menu triggered:" << action;
    if (action == "New window") {
        qCDebug(appLog) << "Allowing create window";
        if (MainWindow::allowCreateWindow())
            MainWindow::createWindow()->show();
    } else if (action == "New tab") {
        qCDebug(appLog) << "Adding files with dialog";
        addFilesWithDialog();
    } else if (action == "Save") { //  保存当前显示文件
        qCDebug(appLog) << "Saving current";
        docPage()->saveCurrent();
        docPage()->handleBlockShutdown();
    } else if (action == "Save as") {
        qCDebug(appLog) << "Saving as current";
        docPage()->saveAsCurrent();
    } else if (action == "Magnifer") {   //  开启放大镜
        qCDebug(appLog) << "Opening magnifer";
        docPage()->openMagnifer();
    } else if (action == "Display in file manager") {    //  文件浏览器 显示
        qCDebug(appLog) << "Opening current file folder";
        docPage()->openCurFileFolder();
    } else if (action == "Search") {
        qCDebug(appLog) << "Preparing search";
        docPage()->prepareSearch();
    } else if (action == "Print") {
        qCDebug(appLog) << "Popping print dialog";
        docPage()->getCurSheet()->onPopPrintDialog();
    }
}

void Central::onOpenFilesExec()
{
    qCDebug(appLog) << "Executing open files request";
    addFilesWithDialog();
}

void Central::onNeedActivateWindow()
{
    qCDebug(appLog) << "Activating window";
    activateWindow();
}

void Central::onShowTips(QWidget *parent, const QString &text, int iconIndex)
{
    qCDebug(appLog) << "Showing tips:" << text << "icon index:" << iconIndex;
    if (m_layout->currentIndex() == 0) {
        qCDebug(appLog) << "Showing tips in nav page";
        if (0 == iconIndex)
            DMessageManager::instance()->sendMessage(m_navPage, QIcon::fromTheme(QString("dr_") + "ok"), text);
        else
            DMessageManager::instance()->sendMessage(m_navPage, QIcon::fromTheme(QString("dr_") + "warning"), text);
    } else if (parent != nullptr) {
        qCDebug(appLog) << "Showing tips in parent";
        if (0 == iconIndex)
            DMessageManager::instance()->sendMessage(parent, QIcon::fromTheme(QString("dr_") + "ok"), text);
        else
            DMessageManager::instance()->sendMessage(parent, QIcon::fromTheme(QString("dr_") + "warning"), text);
    } else {
        qCDebug(appLog) << "Showing tips in central";
        if (0 == iconIndex)
            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme(QString("dr_") + "ok"), text);
        else
            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme(QString("dr_") + "warning"), text);
    }
}

void Central::onTouchPadEvent(const QString &name, const QString &direction, int fingers)
{
    // qCDebug(appLog) << "Touch pad event:" << name << "direction:" << direction << "fingers:" << fingers;
    // 当前窗口被激活,且有焦点
    if (this->isActiveWindow()) {
        // qCDebug(appLog) << "Touch pad event is active window";
        if ("pinch" == name && 2 == fingers) {
            if ("in" == direction) {
                // 捏合 in是手指捏合的方向 向内缩小
                if (m_docPage) {
                    // qCDebug(appLog) << "Zooming out";
                    m_docPage->zoomOut();
                }
            } else if ("out" == direction) {
                // 捏合 out是手指捏合的方向 向外放大
                if (m_docPage) {
                    // qCDebug(appLog) << "Zooming in";
                    m_docPage->zoomIn();
                }
            }
        }
    }
}

void Central::onKeyTriggered()
{
    qCDebug(appLog) << "Key triggered";
    QAction *action = static_cast<QAction *>(sender());
    if (nullptr == action) {
        qCWarning(appLog) << "Received key trigger from null action";
        return;
    }

    handleShortcut(action->shortcut().toString());
}

void Central::dragEnterEvent(QDragEnterEvent *event)
{
    // qCDebug(appLog) << "Central::dragEnterEvent start";
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        // qCDebug(appLog) << "URLs detected in drag event";
        event->accept();
        activateWindow();
    } else if (mimeData->hasFormat("reader/tabbar")) {
        // qCDebug(appLog) << "Tabbar format detected in drag event";
        event->accept();
        activateWindow();
    } else {
        // qCDebug(appLog) << "Unknown format in drag event, ignoring";
        event->ignore();
    }
    // qCDebug(appLog) << "Central::dragEnterEvent end";
}

void Central::dropEvent(QDropEvent *event)
{
    // qCDebug(appLog) << "Central::dropEvent start";
    auto mimeData = event->mimeData();
    if (mimeData->hasFormat("deepin_reader/tabbar")) {
        // qCDebug(appLog) << "Tabbar format detected in drop event";
        event->setDropAction(Qt::MoveAction);

        event->accept();

        activateWindow();

        QString id = mimeData->data("deepin_reader/uuid");

        DocSheet *sheet = DocSheet::getSheet(id);

        if (nullptr != sheet)
            docPage()->onCentralMoveIn(sheet);

    } else if (mimeData->hasUrls()) {
        // qCDebug(appLog) << "URLs format detected in drop event";
        QWidget *topLevelwidget = topLevelWidget();

        topLevelwidget->setProperty("loading", true);

        for (auto url : mimeData->urls()) {
            if (topLevelwidget->property("windowClosed").toBool())
                break;

            QString filePath = url.toLocalFile();

            qCInfo(appLog) << "拖动打开文档:" <<  filePath;
            if (!MainWindow::activateSheetIfExist(filePath)) {
                docPage()->addFileAsync(filePath);
            }
        }

        topLevelwidget->setProperty("loading", false);
    }
    // qCDebug(appLog) << "Central::dropEvent end";
}

void Central::resizeEvent(QResizeEvent *event)
{
    // qCDebug(appLog) << "Central::resizeEvent start - size:" << event->size();
    m_mainWidget->move(0, 0);
    m_mainWidget->resize(event->size());
    BaseWidget::resizeEvent(event);
    // qCDebug(appLog) << "Central::resizeEvent end";
}
