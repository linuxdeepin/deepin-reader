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

#include <DTitlebar>
#include <DWidgetUtil>
#include <QSignalMapper>
#include <DGuiApplicationHelper>

#include "pdfControl/AppConfig.h"
#include "TitleMenu.h"
#include "TitleWidget.h"
#include "Central.h"

DWIDGET_USE_NAMESPACE

QList<MainWindow *> MainWindow::m_list;

MainWindow::MainWindow(DMainWindow *parent)
    : DMainWindow(parent)
{
    m_list.append(this);

    setTitlebarShadowEnabled(true);

    initUI();

    initShortCut();

    //暂定752*360，后期根据最合适效果设定
    int tWidth = 752;
    int tHeight = 360;

    setMinimumSize(tWidth, tHeight);

    showDefaultSize();

    //  在屏幕中心显示
    Dtk::Widget::moveToCenter(this);
}

MainWindow::~MainWindow()
{
    m_list.removeOne(this);
}

void MainWindow::addSheet(DocSheet *sheet)
{
    m_central->addSheet(sheet);
}

bool MainWindow::hasSheet(DocSheet *sheet)
{
    return m_central->hasSheet(sheet);
}

void MainWindow::activateSheet(DocSheet *sheet)
{
    this->activateWindow();
    m_central->showSheet(sheet);
}

void MainWindow::openfiles(const QStringList &filepaths)
{
    m_central->openFiles(filepaths);
}

void MainWindow::doOpenFile(const QString &filePath)
{
    m_central->doOpenFile(filePath);
}

void MainWindow::setSreenRect(const QRect &rect)
{
//    dApp->m_pAppInfo->setScreenRect(rect);
    dApp->m_pAppCfg->setScreenRect(rect);
}

//  窗口关闭
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_central->saveAll()) {
        dApp->m_pAppCfg->setAppKeyValue(KEY_APP_WIDTH, QString("%1").arg(this->width()));

        dApp->m_pAppCfg->setAppKeyValue(KEY_APP_HEIGHT, QString("%1").arg(this->height()));

        event->accept();

        this->deleteLater();

    } else
        event->ignore();
}

void MainWindow::initUI()
{
    m_central = new Central(this);
    connect(m_central, SIGNAL(sigNeedClose()), this, SLOT(close()));

    setCentralWidget(m_central);

    titlebar()->setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    titlebar()->setTitle("");
    titlebar()->setMenu(m_central->titleMenu());
    titlebar()->addWidget(m_central->titleWidget(), Qt::AlignLeft);
    titlebar()->setFocusPolicy(Qt::NoFocus);
}

//  快捷键 实现
void MainWindow::onShortCut(const QString &key)
{
    m_central->handleShortcut(key);
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

MainWindow *MainWindow::createWindow()
{
    return new MainWindow();
}

//  窗口显示默认大小
void MainWindow::showDefaultSize()
{
    int nWidth = dApp->m_pAppCfg->getAppKeyValue(KEY_APP_WIDTH).toInt();
    int nHeight = dApp->m_pAppCfg->getAppKeyValue(KEY_APP_HEIGHT).toInt();

    if (nWidth == 0 || nHeight == 0) {
        int tWidth = 1000;
        int tHeight = 680;
        QString str = "";

        resize(tWidth, tHeight);

        str = QString::number(tWidth);
        dApp->m_pAppCfg->setAppKeyValue(KEY_APP_WIDTH, str);
        str = QString::number(tHeight);
        dApp->m_pAppCfg->setAppKeyValue(KEY_APP_HEIGHT, str);
    } else {
        resize(nWidth, nHeight);
    }
}

//  初始化 快捷键操作
void MainWindow::initShortCut()
{
    auto pSigManager = new QSignalMapper(this);

    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(onShortCut(const QString &)));

    auto keyList = dApp->m_pAppCfg->getKeyList();

    foreach (auto key, keyList) {
        auto action = new QAction(this);

        action->setShortcut(key);

        this->addAction(action);

        connect(action, SIGNAL(triggered()), pSigManager, SLOT(map()));

        pSigManager->setMapping(action, key.toString());
    }
}
