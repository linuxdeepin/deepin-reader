/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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

#include "Central.h"

#include <QFileInfo>
#include <QMimeData>
#include <QUrl>
#include <DMessageManager>
#include <QStackedLayout>
#include <DFileDialog>

#include "business/AppInfo.h"
#include "utils/utils.h"
#include "CentralNavPage.h"
#include "CentralDocPage.h"
#include "TitleMenu.h"
#include "TitleWidget.h"
#include "MainWindow.h"
#include "business/ShortCutShow.h"

Central::Central(QWidget *parent)
    : CustomWidget(parent)
{
    setAcceptDrops(true);

    m_menu    = new TitleMenu(this);
    connect(m_menu, SIGNAL(sigActionTriggered(QString)), this, SLOT(onMenuTriggered(QString)));

    m_widget  = new TitleWidget(this);

    m_docPage = new CentralDocPage(this);
    connect(m_docPage, SIGNAL(sigNeedShowTips(const QString &, int)), this, SLOT(onShowTips(const QString &, int)));
    connect(m_docPage, SIGNAL(sigNeedClose()), this, SIGNAL(sigNeedClose()));
    connect(m_docPage, SIGNAL(sigSheetCountChanged(int)), this, SLOT(onSheetCountChanged(int)));
    connect(m_docPage, SIGNAL(sigNeedOpenFilesExec()), SLOT(onOpenFilesExec()));
    connect(m_docPage, SIGNAL(sigNeedActivateWindow()), this, SLOT(onNeedActivateWindow()));

    m_navPage = new CentralNavPage(this);
    connect(m_navPage, SIGNAL(sigNeedOpenFilesExec()), SLOT(onOpenFilesExec()));

    m_layout = new QStackedLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_navPage);
    m_layout->addWidget(m_docPage);
    setLayout(m_layout);

    connect(m_docPage, SIGNAL(sigCurSheetChanged(DocSheet *)), m_menu, SLOT(onCurSheetChanged(DocSheet *)));
    connect(m_docPage, SIGNAL(sigCurSheetChanged(DocSheet *)), m_widget, SLOT(onCurSheetChanged(DocSheet *)));
    connect(m_docPage, SIGNAL(sigFindOperation(const int &)), m_widget, SLOT(slotFindOperation(const int &)));
}

Central::~Central()
{
}

TitleMenu *Central::titleMenu()
{
    return m_menu;
}

TitleWidget *Central::titleWidget()
{
    return m_widget;
}

void Central::doOpenFile(QString filePath)
{
    m_docPage->openFile(filePath);
}

void Central::openFilesExec()
{
    DFileDialog dialog;
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setNameFilter("document Files (*.pdf *.djvu)");
    dialog.setDirectory(QDir::homePath());

    if (QDialog::Accepted != dialog.exec()) {
        return;
    }

    QStringList filePathList = dialog.selectedFiles();

    if (filePathList.count() <= 0) {
        return;
    }

    openFiles(filePathList);
}

void Central::openFiles(QStringList filePathList)
{
    QList<DocSheet *> sheets = DocSheet::g_map.values();

    foreach (QString filePath, filePathList) {
        //如果存在则活跃该窗口
        bool hasFind = false;

        foreach (DocSheet *sheet, sheets) {
            if (sheet->filePath() == filePath) {
                MainWindow *window = MainWindow::windowContainSheet(sheet);
                if (nullptr != window) {
                    window->activateSheet(sheet);
                    hasFind = true;
                    break;
                }
            }
        }

        if (!hasFind) {
            doOpenFile(filePath);
        }
    }
}

void Central::addSheet(DocSheet *sheet)
{
    m_docPage->addSheet(sheet);
}

bool Central::hasSheet(DocSheet *sheet)
{
    return m_docPage->hasSheet(sheet);
}

void Central::showSheet(DocSheet *sheet)
{
    m_docPage->showSheet(sheet);
}

bool Central::saveAll()
{
    return m_docPage->saveAll();
}

void Central::handleShortcut(QString shortcut)
{
    if (shortcut == KeyStr::g_ctrl_o) {
        openFilesExec();
    } if (shortcut == KeyStr::g_ctrl_shift_slash) { //  显示快捷键预览
        ShortCutShow show;
        show.setSheet(m_docPage->getCurSheet());
        show.show();
    } else
        m_docPage->handleShortcut(shortcut);
}

void Central::onSheetCountChanged(int count)
{
    if (m_layout) {
        if (count > 0) {
            m_layout->setCurrentIndex(1);
        } else if (count == 0) {
            m_layout->setCurrentIndex(0);
            m_navPage->setFocus();
            if (m_widget) {
                m_widget->setControlEnabled(false);
            }
        }
    }
}

void Central::onMenuTriggered(const QString &action)
{
    if (action == "New window") {
        if (MainWindow::allowCreateWindow())
            MainWindow::createWindow()->show();
    } else if (action == "New tab") {
        openFilesExec();
    } else if (action == "Save") { //  保存当前显示文件
        m_docPage->saveCurrent();
    } else if (action == "Save as") {
        m_docPage->saveAsCurrent();
    } else if (action == "Print") {
        m_docPage->handleShortcut(KeyStr::g_ctrl_p);
    } else if (action == "Slide show") { //  开启幻灯片
        m_docPage->openSlide();
    } else if (action == "Magnifer") {   //  开启放大镜
        m_docPage->openMagnifer();
    } else if (action == "Document info") {
        m_docPage->showFileAttr();
    } else if (action == "Display in file manager") {    //  文件浏览器 显示
        m_docPage->openCurFileFolder();
    }
}

void Central::onOpenFilesExec()
{
    openFilesExec();
}

void Central::onNeedActivateWindow()
{
    activateWindow();
}

void Central::onShowTips(const QString &text, int iconIndex)
{
    if (m_layout->currentIndex() == 0) {
        if (0 == iconIndex)
            DMessageManager::instance()->sendMessage(m_navPage, QIcon::fromTheme(Pri::g_module + "ok")/*QIcon(":/icons/deepin/builtin/ok.svg")*/, text);
        else
            DMessageManager::instance()->sendMessage(m_navPage, QIcon::fromTheme(Pri::g_module + "warning")/*QIcon(":/icons/deepin/builtin/warning.svg")*/, text);
    } else {
        if (0 == iconIndex)
            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme(Pri::g_module + "ok")/*QIcon(":/icons/deepin/builtin/ok.svg")*/, text);
        else
            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme(Pri::g_module + "warning")/*QIcon(":/icons/deepin/builtin/warning.svg")*/, text);
    }
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
            m_docPage->onCentralMoveIn(sheet);

    } else if (mimeData->hasUrls()) {
        QStringList filePathList;

        for (auto url : mimeData->urls()) {
            QString sFilePath = url.toLocalFile();
            filePathList.append(sFilePath);
        }

        if (filePathList.count() > 0) {
            openFiles(filePathList);
        }
    }
}

