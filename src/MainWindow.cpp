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
#include "MainWindow.h"

#include "TitleMenu.h"
#include "TitleWidget.h"
#include "Central.h"
#include "CentralDocPage.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <QSignalMapper>
#include <DGuiApplicationHelper>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QDBusConnection>
#include <QTimer>

DWIDGET_USE_NAMESPACE

QList<MainWindow *> MainWindow::m_list;
MainWindow::MainWindow(QStringList filePathList, DMainWindow *parent)
    : DMainWindow(parent), m_initFilePathList(filePathList)
{
    initBase();

    if (filePathList.isEmpty()) {   //不带参启动延时创建所有控件
        QTimer::singleShot(10, this, SLOT(onDelayInit()));

    } else {
        initUI();

        initShortCut();

        foreach (const QString &filePath, m_initFilePathList) {
            doOpenFile(filePath);
        }
    }
}

MainWindow::MainWindow(DocSheet *sheet, DMainWindow *parent): DMainWindow(parent)
{
    initBase();

    initUI();

    initShortCut();

    addSheet(sheet);
}

MainWindow::~MainWindow()
{
    m_list.removeOne(this);
    if (m_list.count() <= 0) {
        QDBusConnection dbus = QDBusConnection::sessionBus();
        dbus.unregisterService("com.deepin.Reader");
    }
}

void MainWindow::addSheet(DocSheet *sheet)
{
    if (nullptr == m_central)
        return;

    m_central->addSheet(sheet);
}

bool MainWindow::hasSheet(DocSheet *sheet)
{
    if (nullptr == m_central)
        return false;

    return m_central->hasSheet(sheet);
}

void MainWindow::activateSheet(DocSheet *sheet)
{
    if (nullptr == m_central)
        return;

    this->activateWindow();

    m_central->showSheet(sheet);
}

void MainWindow::closeWithoutSave()
{
    m_needSave = false;
    this->close();
}

void MainWindow::openfiles(const QStringList &filepaths)
{
    if (nullptr == m_central)
        return;

    m_central->openFiles(filepaths);
}

void MainWindow::doOpenFile(const QString &filePath)
{
    if (nullptr == m_central)
        return;

    m_central->doOpenFile(filePath);
}

//  窗口关闭
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_needSave || (m_central && m_central->saveAll())) {
        QSettings settings(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.conf"), QSettings::IniFormat, this);

        settings.setValue("LASTWIDTH", QString::number(this->width()));

        settings.setValue("LASTHEIGHT", QString::number(this->height()));

        event->accept();

        this->deleteLater();

    } else
        event->ignore();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this) {
        if (event->type() == QEvent::HoverMove) {
            QHoverEvent *mouseEvent = dynamic_cast<QHoverEvent *>(event);
            bool isFullscreen = this->windowState().testFlag(Qt::WindowFullScreen);
            if (isFullscreen) {
                int doctabbarH = m_docTabbarWidget ? m_docTabbarWidget->height() : 0;
                if (mouseEvent->pos().y() > titlebar()->height() + doctabbarH) {
                    if (m_docTabbarWidget)
                        m_docTabbarWidget->setVisible(false);

                    if (this->titlebar())
                        this->titlebar()->setVisible(false);
                }
                if (mouseEvent->pos().y() < 2) {
                    if (m_docTabbarWidget && m_central && m_central->docPage()->getTitleLabel()->property("text").toString().isEmpty()) {
                        m_docTabbarWidget->setVisible(true);
                    }

                    if (this->titlebar())
                        this->titlebar()->setVisible(true);
                }
            }
        } else if (event->type() == QEvent::WindowStateChange) {
            if (isFullScreen()) {
                if (m_docTabbarWidget)
                    m_docTabbarWidget->setVisible(false);

                if (this->titlebar())
                    this->titlebar()->setVisible(false);
            } else {
                if (m_docTabbarWidget && m_central && m_central->docPage()->getTitleLabel()->property("text").toString().isEmpty()) {
                    m_docTabbarWidget->setVisible(true);
                }

                if (this->titlebar())
                    this->titlebar()->setVisible(true);
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void MainWindow::initUI()
{
    m_central = new Central(this);
    connect(m_central, SIGNAL(sigNeedClose()), this, SLOT(close()));
    m_central->setMenu(m_menu);
    setCentralWidget(m_central);

    titlebar()->setAutoHideOnFullscreen(true);
    titlebar()->setIcon(QIcon::fromTheme("deepin-reader"));
    titlebar()->setTitle("");
    titlebar()->addWidget(m_central->titleWidget(), Qt::AlignLeft);
    titlebar()->addWidget(m_central->docPage()->getTitleLabel(), Qt::AlignLeft);
    titlebar()->setAutoHideOnFullscreen(false);

    //移除焦点抢占和避免出现焦点样式
    titlebar()->setFocusPolicy(Qt::NoFocus);
//    QList<QWidget *> list = titlebar()->findChildren<QWidget *>();
//    foreach (QWidget *w, list) {
//        if (!w->objectName().isEmpty())
//            w->setFocusPolicy(Qt::NoFocus);
//    }

    QTimer::singleShot(10, this, SLOT(onUpdateTitleLabelRect()));
}

//  快捷键 实现
void MainWindow::onShortCut(const QString &key)
{
    if (nullptr == m_central)
        return;

    m_central->handleShortcut(key);
}

void MainWindow::setDocTabBarWidget(QWidget *widget)
{
    m_docTabbarWidget = widget;
}

MainWindow *MainWindow::windowContainSheet(DocSheet *sheet)
{
    foreach (MainWindow *window, m_list) {
        if (window->hasSheet(sheet)) {
            return window;
        }
    }

    return nullptr;
}

bool MainWindow::allowCreateWindow()
{
    return m_list.count() < 20;
}

MainWindow *MainWindow::createWindow(QStringList filePathList)
{
    return new MainWindow(filePathList);
}

MainWindow *MainWindow::createWindow(DocSheet *sheet)
{
    return new MainWindow(sheet);
}

//  窗口显示默认大小
void MainWindow::showDefaultSize()
{
    QSettings settings(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.conf"), QSettings::IniFormat, this);

    int width  = settings.value("LASTWIDTH").toInt();
    int height = settings.value("LASTHEIGHT").toInt();

    if (width == 0 || height == 0) {
        resize(1000, 680);
    } else {
        resize(width, height);
    }
}

//  初始化 快捷键操作
void MainWindow::initShortCut()
{
    QList<QKeySequence> keyList;
    keyList.append(QKeySequence::Find);
    keyList.append(QKeySequence::Open);
    keyList.append(QKeySequence::Print);
    keyList.append(QKeySequence::Save);
    keyList.append(QKeySequence::Copy);
    keyList.append(QKeySequence(Qt::Key_Left));
    keyList.append(QKeySequence(Qt::Key_Right));
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

    auto pSigManager = new QSignalMapper(this);

    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(onShortCut(const QString &)));

    foreach (auto key, keyList) {
        auto action = new QAction(this);

        action->setShortcut(key);

        this->addAction(action);

        connect(action, SIGNAL(triggered()), pSigManager, SLOT(map()));

        pSigManager->setMapping(action, key.toString());
    }
}

void MainWindow::onDelayInit()
{
    initUI();

    initShortCut();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    DMainWindow::resizeEvent(event);

    onUpdateTitleLabelRect();
}

void MainWindow::initBase()
{
    m_list.append(this);

    setTitlebarShadowEnabled(true);

    setMinimumSize(752, 360);

    showDefaultSize();

    Dtk::Widget::moveToCenter(this);

    this->installEventFilter(this);

    m_menu = new TitleMenu(this);

    titlebar()->setMenu(m_menu);
}

void MainWindow::onUpdateTitleLabelRect()
{
    if (nullptr == m_central)
        return;

    QWidget *titleLabel = m_central->docPage()->getTitleLabel();
    titleLabel->setFixedWidth(this->width() - m_central->titleWidget()->width() - titlebar()->buttonAreaWidth() - 60);
}
