/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#include "CentralDocPage.h"
#include "DocSheet.h"
#include "DocTabBar.h"
#include "MainWindow.h"
#include "SaveDialog.h"
#include "SlideWidget.h"
#include "Application.h"

#include <DTitlebar>
#include <DFileDialog>
#include <DDialog>

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QUrl>
#include <QDesktopServices>
#include <QStackedLayout>
#include <QMimeType>
#include <QMimeDatabase>
#include <QProcess>
#include <QUuid>
#include <QTimer>
#include <QDebug>

CentralDocPage::CentralDocPage(DWidget *parent)
    : BaseWidget(parent)
{
    m_pTabBar = new DocTabBar(this);
    connect(m_pTabBar, SIGNAL(sigTabChanged(DocSheet *)), this, SLOT(onTabChanged(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigTabMoveIn(DocSheet *)), this, SLOT(onTabMoveIn(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigTabClosed(DocSheet *)), this, SLOT(onTabClosed(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigTabMoveOut(DocSheet *)), this, SLOT(onTabMoveOut(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigTabNewWindow(DocSheet *)), this, SLOT(onTabNewWindow(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigNeedOpenFilesExec()), this, SIGNAL(sigNeedOpenFilesExec()));
    connect(m_pTabBar, SIGNAL(sigNeedActivateWindow()), this, SIGNAL(sigNeedActivateWindow()));

    m_pStackedLayout = new QStackedLayout;
    m_mainLayout = new QVBoxLayout(this);

    m_mainLayout->addWidget(m_pTabBar);
    m_mainLayout->addItem(m_pStackedLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    this->setLayout(m_mainLayout);

    m_pDocTabLabel = new DLabel(this);
    m_pDocTabLabel->setElideMode(Qt::ElideMiddle);
    m_pDocTabLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    m_pDocTabLabel->setAlignment(Qt::AlignCenter);
    connect(this, SIGNAL(sigSheetCountChanged(int)), this, SLOT(onSheetCountChanged(int)));

    QWidget *mainwindow = parent->parentWidget();
    if (mainwindow) {
        DIconButton *leftButton = m_pTabBar->findChild<DIconButton *>("leftButton");
        DIconButton *rightButton = m_pTabBar->findChild<DIconButton *>("rightButton");
        DIconButton *addButton = m_pTabBar->findChild<DIconButton *>("AddButton");

        QList<QWidget *> orderlst = mainwindow->property("orderlist").value<QList<QWidget *>>();
        orderlst << leftButton;
        orderlst << rightButton;
        orderlst << addButton;
        mainwindow->setProperty("orderlist", QVariant::fromValue(orderlst));
        mainwindow->setProperty("orderWidgets", QVariant::fromValue(orderlst));
    }
}

CentralDocPage::~CentralDocPage()
{

}

bool CentralDocPage::firstThumbnail(QString filePath, QString thumbnailPath)
{
    int fileType = Dr::fileType(filePath);
    if (Dr::DJVU == fileType) {
        QImage image = DocSheet::firstThumbnail(filePath);
        if (image.isNull())
            return false;
        return image.save(thumbnailPath, "PNG");
    }

    return false;
}

void CentralDocPage::openCurFileFolder()
{
    DocSheet *sheet = getCurSheet();
    if (nullptr == sheet)
        return;

    QString filePath = sheet->filePath();
    bool result = QProcess::startDetached(QString("dde-file-manager \"%1\" --show-item").arg(filePath));
    if (!result) {
        QDesktopServices::openUrl(QUrl(QFileInfo(filePath).dir().path()));
    }
}

void CentralDocPage::onSheetFileChanged(DocSheet *sheet)
{
    if (DocSheet::existFileChanged()) {
        dApp->blockShutdown();
    } else {
        dApp->unBlockShutdown();
    }

    if (nullptr == sheet && sheet != getCurSheet())
        return;

    emit sigCurSheetChanged(sheet);
}

void CentralDocPage::onSheetOperationChanged(DocSheet *sheet)
{
    if (nullptr == sheet && sheet != getCurSheet())
        return;

    emit sigCurSheetChanged(sheet);
}

void CentralDocPage::addFileAsync(const QString &filePath)
{
    //判断在打开的文档中是否有filePath，如果有则切到相应的sheet，反之执行打开操作
    if (m_pTabBar) {
        int index = m_pTabBar->indexOfFilePath(filePath);
        if (index >= 0 && index < m_pTabBar->count()) {
            m_pTabBar->setCurrentIndex(index);
            return;
        }
    }

    Dr::FileType fileType = Dr::fileType(filePath);

    if (Dr::PDF != fileType && Dr::DJVU != fileType && Dr::DOCX != fileType) {
        showTips(tr("The format is not supported"), 1);
        return;
    }

    DocSheet *sheet = new DocSheet(fileType, filePath, this);

    connect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetFileChanged(DocSheet *)));
    connect(sheet, SIGNAL(sigOperationChanged(DocSheet *)), this, SLOT(onSheetOperationChanged(DocSheet *)));
    connect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));
    connect(sheet, &DocSheet::sigFileOpened, this, &CentralDocPage::onOpened);

    m_pStackedLayout->addWidget(sheet);

    m_pStackedLayout->setCurrentWidget(sheet);

    m_pTabBar->insertSheet(sheet);

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

    if (sheet->needPassword()) {
        sheet->showEncryPage();
    } else {
        sheet->openFileAsync("");
    }
}

void CentralDocPage::addSheet(DocSheet *sheet)
{
    m_pTabBar->insertSheet(sheet);

    enterSheet(sheet);
}

void CentralDocPage::onOpened(DocSheet *sheet, bool ret, QString error)
{
    if (sheet == nullptr)
        return;

    if (!ret) {
        m_pStackedLayout->removeWidget(sheet);

        m_pTabBar->removeSheet(sheet);

        emit sigSheetCountChanged(m_pStackedLayout->count());

        emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

        sheet->deleteLater();

        showTips(error, 1);

        return;
    }

    this->activateWindow();

    sheet->defaultFocus();
}

void CentralDocPage::onTabChanged(DocSheet *sheet)
{
    if (nullptr != sheet) {
        m_pStackedLayout->setCurrentWidget(sheet);

        sheet->defaultFocus();
    }

    emit sigCurSheetChanged(sheet);
}

void CentralDocPage::onTabMoveIn(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    enterSheet(sheet);
}

void CentralDocPage::onTabClosed(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    if (!DocSheet::existSheet(sheet))
        return;

    if (sheet->fileChanged()) {

        int ret = SaveDialog::showExitDialog();

        if (ret < 1)
            return;

        if (2 == ret) {
            sheet->saveData();
        }
    }

    m_pStackedLayout->removeWidget(sheet);

    if (m_pTabBar) {
        m_pTabBar->removeSheet(sheet);
    }

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

    sheet->deadDeleteLater();

}

void CentralDocPage::onTabMoveOut(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    leaveSheet(sheet);

    if (m_pStackedLayout->count() <= 0) {
        emit sigNeedClose();
        return;
    }
}

void CentralDocPage::onTabNewWindow(DocSheet *sheet)
{
    leaveSheet(sheet);

    MainWindow *window = MainWindow::createWindow(sheet);

    window->move(QCursor::pos());

    window->show();
}

void CentralDocPage::onCentralMoveIn(DocSheet *sheet)
{
    addSheet(sheet);
}

void CentralDocPage::leaveSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    m_pStackedLayout->removeWidget(sheet);

    disconnect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetFileChanged(DocSheet *)));
    disconnect(sheet, SIGNAL(sigOperationChanged(DocSheet *)), this, SLOT(onSheetOperationChanged(DocSheet *)));
    disconnect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));
}

void CentralDocPage::enterSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    sheet->setParent(this);

    connect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetFileChanged(DocSheet *)));
    connect(sheet, SIGNAL(sigOperationChanged(DocSheet *)), this, SLOT(onSheetOperationChanged(DocSheet *)));
    connect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

    m_pStackedLayout->addWidget(sheet);

    m_pStackedLayout->setCurrentWidget(sheet);

    sheet->defaultFocus();

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));
}

bool CentralDocPage::hasSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        return false;

    auto sheets = this->findChildren<DocSheet *>();

    for (int i = 0; i < sheets.count(); ++i) {
        if (sheets[i] == sheet)
            return true;
    }

    return false;
}

void CentralDocPage::showSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    m_pTabBar->showSheet(sheet);
}

bool CentralDocPage::saveAll()
{
    QList<DocSheet *> changedList;

    auto sheets = this->findChildren<DocSheet *>();

    foreach (auto sheet, sheets) {
        if (sheet->fileChanged())
            changedList.append(sheet);
    }

    if (changedList.size() > 0) {   //需要提示保存
        int nRes = SaveDialog::showExitDialog();

        if (nRes <= 0) {
            return false;
        }

        if (nRes == 2) {
            foreach (auto sheet, changedList) {
                sheet->saveData();
            }
        }
    }

    return true;
}

bool CentralDocPage::saveCurrent()
{
    DocSheet *sheet = static_cast<DocSheet *>(m_pStackedLayout->currentWidget());

    if (nullptr == sheet)
        return false;

    if (!sheet->fileChanged()) {
        return false;
    }

    if (!sheet->saveData()) {
        showTips(tr("Save failed"), 1);
        return false;
    }

    sigCurSheetChanged(sheet);

    showTips(tr("Saved successfully"));

    return true;
}

bool CentralDocPage::saveAsCurrent()
{
    DocSheet *sheet = getCurSheet();

    if (nullptr == sheet)
        return false;

    QString saveFilePath = DFileDialog::getSaveFileName(this, tr("Save as"), sheet->filePath(), sheet->filter());

    if (Dr::PDF == sheet->fileType()) {
        if (saveFilePath.endsWith("/.pdf")) {
            DDialog dlg("", tr("Invalid file name"));
            dlg.setIcon(QIcon::fromTheme(QString("dr_") + "exception-logo"));
            dlg.addButtons(QStringList() << tr("OK"));
            QMargins mar(0, 0, 0, 30);
            dlg.setContentLayoutContentsMargins(mar);
            dlg.exec();
            return false;
        }
    } else if (Dr::DJVU == sheet->fileType()) {
        if (saveFilePath.endsWith("/.djvu")) {
            DDialog dlg("", tr("Invalid file name"));
            dlg.setIcon(QIcon::fromTheme(QString("dr_") + "exception-logo"));
            dlg.addButtons(QStringList() << tr("OK"));
            QMargins mar(0, 0, 0, 30);
            dlg.setContentLayoutContentsMargins(mar);
            dlg.exec();
            return false;
        }
    }

    return sheet->saveAsData(saveFilePath);
}

DocSheet *CentralDocPage::getCurSheet()
{
    if (m_pStackedLayout != nullptr) {
        return dynamic_cast<DocSheet *>(m_pStackedLayout->currentWidget());
    }

    return nullptr;
}

DocSheet *CentralDocPage::getSheet(const QString &filePath)
{
    auto sheets = this->findChildren<DocSheet *>();
    foreach (auto sheet, sheets) {
        if (sheet->filePath() == filePath) {
            return sheet;
        }
    }

    return nullptr;
}

void CentralDocPage::handleShortcut(const QString &s)
{
    if (s == Dr::key_esc && m_slideWidget) {
        quitSlide();
        return;
    }

    if (s == Dr::key_f11 && m_slideWidget) {    //幻灯片时 f11设置为不起作    用
        return;
    }

    if (s == Dr::key_esc && !m_magniferSheet.isNull() && m_magniferSheet->magnifierOpened()) {
        quitMagnifer();
        return;
    }

    if ((s == Dr::key_esc || s == Dr::key_f11) && getCurSheet() && getCurSheet()->closeFullScreen())
        return;

    if (m_slideWidget) {
        m_slideWidget->handleKeyPressEvent(s);
        return;
    }

    if (s == Dr::key_ctrl_s) {
        saveCurrent();
    } else if (s == Dr::key_ctrl_shift_s) {
        saveAsCurrent();
    } else if (s == Dr::key_f5) {
        openSlide();
    } else if (s == Dr::key_alt_z) {
        openMagnifer();
    } else if (s == Dr::key_ctrl_p) {
        if (getCurSheet())
            QTimer::singleShot(1, getCurSheet(), SLOT(onPopPrintDialog()));
    } else if (s == Dr::key_alt_1) {
        if (getCurSheet())
            getCurSheet()->setMouseShape(Dr::MouseShapeNormal);
    } else if (s == Dr::key_alt_2) {
        if (getCurSheet())
            getCurSheet()->setMouseShape(Dr::MouseShapeHand);
    } else if (s == Dr::key_ctrl_1) {
        if (getCurSheet())
            getCurSheet()->setScaleMode(Dr::FitToPageDefaultMode);
    } else if (s == Dr::key_ctrl_m) {
        if (getCurSheet())
            getCurSheet()->setSidebarVisible(true);
    } else if (s == Dr::key_ctrl_2) {
        if (getCurSheet())
            getCurSheet()->setScaleMode(Dr::FitToPageHeightMode);
    } else if (s == Dr::key_ctrl_3) {
        if (getCurSheet())
            getCurSheet()->setScaleMode(Dr::FitToPageWidthMode);
    } else if (s == Dr::key_ctrl_r) {
        if (getCurSheet())
            getCurSheet()->rotateLeft();
    } else if (s == Dr::key_ctrl_shift_r) {
        if (getCurSheet())
            getCurSheet()->rotateRight();
    }  else if (s == Dr::key_alt_harger) {
        if (getCurSheet())
            getCurSheet()->zoomin();
    } else if (s == Dr::key_ctrl_equal) {
        if (getCurSheet())
            getCurSheet()->zoomin();
    } else if (s == Dr::key_ctrl_smaller) {
        if (getCurSheet())
            getCurSheet()->zoomout();
    } else if (s == Dr::key_ctrl_d) {
        if (getCurSheet())
            getCurSheet()->setBookMark(getCurSheet()->currentIndex(), true);
    } else if (s == Dr::key_ctrl_f) {
        if (getCurSheet())
            getCurSheet()->handleSearch();
    } else if (s == Dr::key_ctrl_c) {
        if (getCurSheet())
            getCurSheet()->copySelectedText();
    } else if (s == Dr::key_alt_h) {
        if (getCurSheet())
            getCurSheet()->highlightSelectedText();
    } else if (s == Dr::key_alt_a) {
        if (getCurSheet())
            getCurSheet()->addSelectedTextHightlightAnnotation();
    } else if (s == Dr::key_left) {
        if (getCurSheet())
            getCurSheet()->jumpToPrevPage();
    } else if (s == Dr::key_right) {
        if (getCurSheet())
            getCurSheet()->jumpToNextPage();
    } else if (s == Dr::key_f11) {
        if (getCurSheet())
            getCurSheet()->openFullScreen();
    }
}

void CentralDocPage::showTips(const QString &tips, int iconIndex)
{
    emit sigNeedShowTips(tips, iconIndex);
}

void CentralDocPage::openMagnifer()
{
    quitMagnifer();

    m_magniferSheet = getCurSheet();

    if (!m_magniferSheet.isNull())
        m_magniferSheet->openMagnifier();

}

//  取消放大镜
void CentralDocPage::quitMagnifer()
{
    if (!m_magniferSheet.isNull() && m_magniferSheet->magnifierOpened()) {
        m_magniferSheet->closeMagnifier();
        m_magniferSheet = nullptr;
    }
}

void CentralDocPage::openSlide()
{
    DocSheet *docSheet = getCurSheet();
    if (docSheet && docSheet->isUnLocked() && m_slideWidget == nullptr) {
        m_slideWidget = new SlideWidget(getCurSheet());
    }
}

void CentralDocPage::quitSlide()
{
    if (m_slideWidget) {
        m_slideWidget->close();
        m_slideWidget = nullptr;
    }
}

bool CentralDocPage::isSlide()
{
    return m_slideWidget != nullptr;
}

void CentralDocPage::handleSearch()
{
    DocSheet *docSheet = getCurSheet();
    if (docSheet)
        docSheet->handleSearch();
}

bool CentralDocPage::isFullScreen()
{
    DMainWindow *mainWindow = static_cast<DMainWindow *>(parentWidget()->parentWidget());

    if (nullptr == mainWindow)
        return false;

    return mainWindow->isFullScreen();
}

void CentralDocPage::openFullScreen()
{
    MainWindow *mainWindow = dynamic_cast<MainWindow *>(parentWidget()->parentWidget());

    if (nullptr == mainWindow)
        return;

    if (!mainWindow->isFullScreen()) {
        m_mainLayout->removeWidget(m_pTabBar);
        mainWindow->setDocTabBarWidget(m_pTabBar);
        mainWindow->showFullScreen();
    }
}

bool CentralDocPage::quitFullScreen(bool force)
{
    MainWindow *mainWindow = dynamic_cast<MainWindow *>(parentWidget()->parentWidget());
    if (nullptr == mainWindow)
        return false;

    if (mainWindow->isFullScreen() || force) {
        m_pTabBar->setParent(this);
        m_mainLayout->insertWidget(0, m_pTabBar);
        m_pTabBar->setVisible(m_pTabBar->count() > 1);
        mainWindow->setDocTabBarWidget(nullptr);
        if (mainWindow->isFullScreen())
            mainWindow->showNormal();
        return true;
    }

    return false;
}

void CentralDocPage::onSheetCountChanged(int count)
{
    if (count == 0)
        return;

    if (count == 1) {
        //tabText(0)可能存在还没取到值的情况，稍微延迟下做处理
        QTimer::singleShot(10, this, SLOT(onUpdateTabLabelText()));
        m_pDocTabLabel->setVisible(true);
        m_pTabBar->setVisible(false);
    } else {
        m_pDocTabLabel->setVisible(false);
        m_pTabBar->setVisible(true);
    }

    MainWindow *mainWindow = dynamic_cast<MainWindow *>(parentWidget()->parentWidget());
    if (mainWindow && mainWindow->isFullScreen()) {
        mainWindow->resizeFullTitleWidget();
    } else if (m_pTabBar->parent() != this) {
        m_pTabBar->setParent(this);
        m_mainLayout->insertWidget(0, m_pTabBar);
        m_pTabBar->setVisible(count > 1);
    }
}

void CentralDocPage::onUpdateTabLabelText()
{
    if (m_pTabBar->count() > 0)
        m_pDocTabLabel->setText(m_pTabBar->tabText(0));
}

QWidget *CentralDocPage::getTitleLabel()
{
    return m_pDocTabLabel;
}

void CentralDocPage::zoomIn()
{
    if (getCurSheet()) {
        getCurSheet()->zoomin();
    }
}

void CentralDocPage::zoomOut()
{
    if (getCurSheet()) {
        getCurSheet()->zoomout();
    }
}
