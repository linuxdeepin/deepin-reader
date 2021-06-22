/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
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
#include "Central.h"
#include "CentralNavPage.h"
#include "CentralDocPage.h"
#include "TitleMenu.h"
#include "TitleWidget.h"
#include "MainWindow.h"
#include "ShortCutShow.h"
#include "DBusObject.h"

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
    setAcceptDrops(false);

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
//    connect(DBusObject::instance(), &DBusObject::sigImActiveChanged, this, &Central::onImActiveChanged); // 虚拟键盘弹出不做自定义处理，先注释

    QList<QKeySequence> keyList;
    keyList.append(QKeySequence::Find);
    keyList.append(QKeySequence::Open);

    keyList.append(QKeySequence::Save);
    keyList.append(QKeySequence::Copy);
    keyList.append(QKeySequence(Qt::Key_Left));
    keyList.append(QKeySequence(Qt::Key_Right));
    keyList.append(QKeySequence(Qt::Key_Space));
    keyList.append(QKeySequence(Qt::Key_Escape));
    keyList.append(QKeySequence(Qt::Key_F5));

    keyList.append(QKeySequence(Qt::ALT | Qt::Key_1));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_2));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_A));
    keyList.append(QKeySequence(Qt::ALT | Qt::Key_H));

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

    foreach (auto key, keyList) {
        auto action = new QAction(this);
        action->setShortcut(key);
        this->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(onKeyTriggered()));
    }
}

Central::~Central()
{
}

TitleWidget *Central::titleWidget()
{
    return m_widget;
}

CentralDocPage *Central::docPage()
{
    if (nullptr == m_docPage) {
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

    return m_docPage;
}

void Central::setMenu(TitleMenu *menu)
{
    m_menu = menu;
    connect(m_menu, SIGNAL(sigActionTriggered(QString)), this, SLOT(onMenuTriggered(QString)));
}

void Central::setUpValue(int value)
{
    m_upValue = value;
    m_mainWidget->move(0, -m_upValue);
    m_mainWidget->resize(this->size());
}

void Central::addFilesWithDialog()
{
    DFileDialog dialog(this);
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Documents") + " (*.pdf *.djvu *.docx)");
    dialog.setDirectory(QDir::homePath());

    if (QDialog::Accepted != dialog.exec()) {
        return;
    }

    QStringList filePathList = dialog.selectedFiles();

    if (filePathList.count() <= 0) {
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
    docPage()->addFileAsync(filePath);
}

void Central::addSheet(DocSheet *sheet)
{
    docPage()->addSheet(sheet);
}

bool Central::hasSheet(DocSheet *sheet)
{
    return docPage()->hasSheet(sheet);
}

void Central::showSheet(DocSheet *sheet)
{
    docPage()->showSheet(sheet);
}

QList<DocSheet *> Central::getSheets()
{
    if (nullptr == m_docPage)
        return QList<DocSheet *>();

    return docPage()->getSheets();
}

void Central::handleShortcut(QString shortcut)
{
    if (shortcut == Dr::key_ctrl_o) {
        addFilesWithDialog();
    } if (shortcut == Dr::key_ctrl_shift_slash) { //  显示快捷键预览
        ShortCutShow show;
        show.setSheet(docPage()->getCurSheet());
        show.show();
    } else
        docPage()->handleShortcut(shortcut);
}

bool Central::handleClose(bool needToBeSaved, bool needSavetip)
{
    if (nullptr != m_docPage) {
        return docPage()->closeAllSheets(needToBeSaved, needSavetip);
    }

    return true;
}

void Central::onSheetCountChanged(int count)
{
    if (count > 0) {
        m_layout->setCurrentIndex(1);
    } else {
        m_layout->setCurrentIndex(0);
        m_navPage->setFocus();
        m_widget->setControlEnabled(false);
    }
}

void Central::onMenuTriggered(const QString &action)
{
    if (action == "New window") {
        if (MainWindow::allowCreateWindow())
            MainWindow::createWindow()->show();
    } else if (action == "New tab") {
        addFilesWithDialog();
    } else if (action == "Save") { //  保存当前显示文件
        docPage()->saveCurrent();
    } else if (action == "Save as") {
        docPage()->saveAsCurrent();
    } else if (action == "Magnifer") {   //  开启放大镜
        docPage()->openMagnifer();
    } else if (action == "Display in file manager") {    //  文件浏览器 显示
        docPage()->openCurFileFolder();
    } else if (action == "Search") {
        docPage()->prepareSearch();
    }
}

void Central::onOpenFilesExec()
{
    addFilesWithDialog();
}

void Central::onNeedActivateWindow()
{
    activateWindow();
}

void Central::onShowTips(QWidget *parent, const QString &text, int iconIndex)
{
    if (m_layout->currentIndex() == 0) {
        if (0 == iconIndex)
            DMessageManager::instance()->sendMessage(m_navPage, QIcon::fromTheme(QString("dr_") + "ok"), text);
        else
            DMessageManager::instance()->sendMessage(m_navPage, QIcon::fromTheme(QString("dr_") + "warning"), text);
    } else if (parent != nullptr) {
        if (0 == iconIndex)
            DMessageManager::instance()->sendMessage(parent, QIcon::fromTheme(QString("dr_") + "ok"), text);
        else
            DMessageManager::instance()->sendMessage(parent, QIcon::fromTheme(QString("dr_") + "warning"), text);
    } else {
        if (0 == iconIndex)
            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme(QString("dr_") + "ok"), text);
        else
            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme(QString("dr_") + "warning"), text);
    }
}

void Central::onTouchPadEvent(QString name, QString direction, int fingers)
{
    // 当前窗口被激活,且有焦点
    if (this->isActiveWindow()) {
        if (name == "pinch" && fingers == 2) {
            if (direction == "in") {
                // 捏合 in是手指捏合的方向 向内缩小
                if (m_docPage) {
                    m_docPage->zoomOut();
                }
            } else if (direction == "out") {
                // 捏合 out是手指捏合的方向 向外放大
                if (m_docPage) {
                    m_docPage->zoomIn();
                }
            }
        }
    }
}

void Central::onImActiveChanged(bool actived)
{
    QTimer::singleShot(100, this, [ = ]() {
        QWidget *transparentTextWidget = focusWidget();
        if (actived && transparentTextWidget && "TransparentTextEdit" == transparentTextWidget->objectName()) {
            int upValue = qMin(transparentTextWidget->mapToGlobal(transparentTextWidget->pos()).y() - m_widget->height() - 68, DBusObject::instance()->virtualKeyboardGeometry().height());
            setUpValue(upValue);
        } else if (actived) {
            setUpValue(DBusObject::instance()->virtualKeyboardGeometry().height());
        } else if (!actived) {
            setUpValue(0);
        }
    });
}

void Central::onKeyTriggered()
{
    QAction *action = static_cast<QAction *>(sender());
    if (nullptr == action)
        return;

    handleShortcut(action->shortcut().toString());
}

void Central::dragEnterEvent(QDragEnterEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->accept();
        activateWindow();
    } else if (mimeData->hasFormat("reader/tabbar")) {
        event->accept();
        activateWindow();
    } else
        event->ignore();
}

void Central::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasFormat("deepin_reader/tabbar")) {
        event->setDropAction(Qt::MoveAction);

        event->accept();

        activateWindow();

        QString id = mimeData->data("deepin_reader/uuid");

        DocSheet *sheet = DocSheet::getSheet(id);

        if (nullptr != sheet)
            docPage()->onCentralMoveIn(sheet);

    } else if (mimeData->hasUrls()) {
        QWidget *topLevelwidget = topLevelWidget();

        topLevelwidget->setProperty("loading", true);

        for (auto url : mimeData->urls()) {
            if (topLevelwidget->property("windowClosed").toBool())
                break;

            QString filePath = url.toLocalFile();

            if (!MainWindow::activateSheetIfExist(filePath)) {
                docPage()->addFileAsync(filePath);
            }
        }

        topLevelwidget->setProperty("loading", false);
    }
}

void Central::resizeEvent(QResizeEvent *event)
{
    m_mainWidget->move(0, -m_upValue);
    m_mainWidget->resize(event->size());
    BaseWidget::resizeEvent(event);
}
