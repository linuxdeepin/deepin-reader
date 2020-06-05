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
#include "CentralDocPage.h"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QUrl>
#include <QDesktopServices>
#include <QStackedLayout>
#include <DFileDialog>
#include <DDialog>
#include <QMimeType>
#include <QMimeDatabase>
#include <QProcess>

#include "CentralDocPage.h"
#include "DocSheet.h"
#include "DocTabBar.h"
#include "MainWindow.h"
#include "business/AppInfo.h"
#include "business/SaveDialog.h"

#include "pdfControl/docview/docummentproxy.h"
#include "widgets/FindWidget.h"
#include "widgets/FileAttrWidget.h"
#include "pdfControl/docview/docummentproxy.h"
#include "utils/utils.h"
#include "CentralDocPage.h"
#include "global.h"
#include "pdfControl/DocSheetPDF.h"
#include "djvuControl/DocSheetDJVU.h"
#include "business/PrintManager.h"

CentralDocPage::CentralDocPage(DWidget *parent)
    : CustomWidget(parent)
{
    m_pTabBar = new DocTabBar(this);
    connect(m_pTabBar, SIGNAL(sigLastTabMoved()), this, SIGNAL(sigNeedClose()));
    connect(m_pTabBar, SIGNAL(sigTabChanged(DocSheet *)), this, SLOT(onTabChanged(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigTabMoveIn(DocSheet *)), this, SLOT(onTabMoveIn(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigTabClosed(DocSheet *)), this, SLOT(onTabClosed(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigTabMoveOut(DocSheet *)), this, SLOT(onTabMoveOut(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigTabNewWindow(DocSheet *)), this, SLOT(onTabNewWindow(DocSheet *)));
    connect(m_pTabBar, SIGNAL(sigNeedOpenFilesExec()), this, SIGNAL(sigNeedOpenFilesExec()));
    connect(m_pTabBar, SIGNAL(sigNeedActivateWindow()), this, SIGNAL(sigNeedActivateWindow()));

    m_pStackedLayout = new QStackedLayout;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(m_pTabBar);
    mainLayout->addItem(m_pStackedLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    this->setLayout(mainLayout);
}

CentralDocPage::~CentralDocPage()
{

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

void CentralDocPage::onSheetChanged(DocSheet *sheet)
{
    if (nullptr == sheet && sheet != getCurSheet())
        return;

    sigCurSheetChanged(sheet);

    if (DocSheet::existFileChanged()) {
        BlockShutdown();
    } else {
        UnBlockShutdown();
    }
}

void CentralDocPage::openFile(QString &filePath)
{
    //判断在打开的文档中是否有filePath，如果有则切到相应的sheet，反之执行打开操作
    if (m_pTabBar) {
        int index = m_pTabBar->indexOfFilePath(filePath);
        if (index >= 0 && index < m_pTabBar->count()) {
            m_pTabBar->setCurrentIndex(index);
            return;
        }
    }

    int fileType = Dr::fileType(filePath);

    if (Dr::PDF == fileType) {
        DocSheet *sheet = new DocSheetPDF(filePath, this);

        connect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
        connect(sheet, SIGNAL(sigOpened(DocSheet *, bool)), this, SLOT(onOpened(DocSheet *, bool)));
        connect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

        sheet->openFile();

        m_pStackedLayout->addWidget(sheet);

        m_pStackedLayout->setCurrentWidget(sheet);

        sheet->defaultFocus();

        m_pTabBar->insertSheet(sheet);

        emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

        emit sigSheetCountChanged(m_pStackedLayout->count());

    } else if (Dr::DjVu == fileType) {
        DocSheet *sheet = new DocSheetDJVU(filePath, this);

        connect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
        connect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

        if (!sheet->openFileExec()) {
            sheet->deleteLater();
            showTips(tr("Please check if the file is damaged"), 1);
            return;
        }

        m_pStackedLayout->addWidget(sheet);

        m_pStackedLayout->setCurrentWidget(sheet);

        m_pTabBar->insertSheet(sheet);

        emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

        emit sigSheetCountChanged(m_pStackedLayout->count());

        onOpened(sheet, true);

    } else {
        showTips(tr("The format is not supported"), 1);
    }
}

void CentralDocPage::addSheet(DocSheet *sheet)
{
    m_pTabBar->insertSheet(sheet);

    enterSheet(sheet);
}

void CentralDocPage::onOpened(DocSheet *sheet, bool ret)
{
    if (!ret) {
        m_pStackedLayout->removeWidget(sheet);

        m_pTabBar->removeSheet(sheet);

        emit sigSheetCountChanged(m_pStackedLayout->count());

        emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

        if (nullptr != sheet)
            sheet->deleteLater();

        showTips(tr("Please check if the file is damaged"), 1);

        return;
    }

    this->activateWindow();
    //文档刚打开时，模拟鼠标点击文档区域事件
    QPoint pos(this->geometry().x(), this->geometry().y());

    QMouseEvent event0(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(this, &event0);

    sheet->defaultFocus();
}

void CentralDocPage::onTabChanged(DocSheet *sheet)
{
    emit sigCurSheetChanged(sheet);

    if (nullptr != sheet) {
        m_pStackedLayout->setCurrentWidget(sheet);
        sheet->defaultFocus();
    }
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

    if (sheet->fileChanged()) {
        SaveDialog dialog;

        int ret = dialog.showDialog();

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

    sheet->deleteLater();

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));
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

    MainWindow *window = MainWindow::createWindow();

    window->addSheet(sheet);

    window->move(QCursor::pos());

    window->show();
}

void CentralDocPage::onCentralMoveIn(DocSheet *sheet)
{
    addSheet(sheet);
}

void CentralDocPage::enterSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    sheet->setParent(this);

    connect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
    connect(sheet, SIGNAL(sigOpened(DocSheet *, bool)), this, SLOT(onOpened(DocSheet *, bool)));
    connect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

    m_pStackedLayout->addWidget(sheet);

    m_pStackedLayout->setCurrentWidget(sheet);

    sheet->defaultFocus();

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));
}

void CentralDocPage::leaveSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    m_pStackedLayout->removeWidget(sheet);

    disconnect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
    disconnect(sheet, SIGNAL(sigOpened(DocSheet *, bool)), this, SLOT(onOpened(DocSheet *, bool)));
    disconnect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

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

        SaveDialog sd;

        int nRes = sd.showDialog();

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

    QString sFilter = sheet->filter();

    if (Dr::PDF == sheet->type()) {
        QString saveFilePath;

        if (sFilter != "") {
            QFileDialog dialog;
            dialog.selectFile(sheet->filePath());
            saveFilePath = dialog.getSaveFileName(nullptr, tr("Save as"), sheet->filePath(), sFilter);

            if (saveFilePath.endsWith("/.pdf")) {
                DDialog dlg("", tr("Invalid file name"));
                dlg.setIcon(QIcon::fromTheme(Pri::g_module + "exception-logo"));
                dlg.addButtons(QStringList() << tr("OK"));
                QMargins mar(0, 0, 0, 30);
                dlg.setContentLayoutContentsMargins(mar);
                dlg.exec();
                return false;
            }
        }
        return sheet->saveAsData(saveFilePath);
    } else if (Dr::DjVu == sheet->type()) {
        QString saveFilePath;

        if (sFilter != "") {
            QFileDialog dialog;
            dialog.selectFile(sheet->filePath());
            saveFilePath = dialog.getSaveFileName(nullptr, tr("Save as"), sheet->filePath(), sFilter);

            if (saveFilePath.endsWith("/.djvu")) {
                DDialog dlg("", tr("Invalid file name"));
                dlg.setIcon(QIcon::fromTheme(Pri::g_module + "exception-logo"));
                dlg.addButtons(QStringList() << tr("OK"));
                QMargins mar(0, 0, 0, 30);
                dlg.setContentLayoutContentsMargins(mar);
                dlg.exec();
                return false;
            }
        }
        return sheet->saveAsData(saveFilePath);
    }

    return false;
}

void CentralDocPage::printCurrent()
{
    DocSheet *sheet = getCurSheet();
    if (nullptr == sheet)
        return;

    PrintManager p(sheet);
    p.showPrintDialog(sheet);
}

DocSheet *CentralDocPage::getCurSheet()
{
    if (m_pStackedLayout != nullptr) {
        return static_cast<DocSheet *>(m_pStackedLayout->currentWidget());
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

void CentralDocPage::BlockShutdown()
{
    if (m_bBlockShutdown)
        return;

    if (m_reply.value().isValid()) {
        return;
    }

    m_pLoginManager = new QDBusInterface("org.freedesktop.login1",
                                         "/org/freedesktop/login1",
                                         "org.freedesktop.login1.Manager",
                                         QDBusConnection::systemBus());

    m_arg << QString("shutdown")             // what
          << qApp->applicationDisplayName()           // who
          << QObject::tr("File not saved") // why
          << QString("block");                        // mode

    int fd = -1;
    m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
    if (m_reply.isValid()) {
        fd = m_reply.value().fileDescriptor();
        m_bBlockShutdown = true;
    }
}

void CentralDocPage::UnBlockShutdown()
{
    auto splitterList = this->findChildren<DocSheet *>();
    foreach (auto mainsplit, splitterList) {
        if (mainsplit->fileChanged())
            return;
    }

    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        m_bBlockShutdown = false;
    }
}

void CentralDocPage::showFileAttr()
{
    auto pFileAttrWidget = new FileAttrWidget;

    pFileAttrWidget->setFileAttr(getCurSheet());

    pFileAttrWidget->setAttribute(Qt::WA_DeleteOnClose);

    pFileAttrWidget->showScreenCenter();

}

void CentralDocPage::handleShortcut(const QString &s)
{
    if (!m_slideSheet.isNull() && m_slideSheet->slideOpened() && s == KeyStr::g_esc) {
        quitSlide();
        return;
    }

    if (!m_magniferSheet.isNull() && m_magniferSheet->magnifierOpened() && s == KeyStr::g_esc) {
        quitMagnifer();
        return;
    }

    if (!m_slideSheet.isNull() && m_slideSheet->slideOpened()) {
        m_slideSheet->handleSlideKeyPressEvent(s);
        return;
    }

    if (s == KeyStr::g_ctrl_s) {
        saveCurrent();
    } else if (s == KeyStr::g_ctrl_shift_s) {
        saveAsCurrent();
    } else if (s == KeyStr::g_ctrl_h) {
        openSlide();
    } else if (s == KeyStr::g_alt_z) {
        openMagnifer();
    } else if (s == KeyStr::g_ctrl_p) {
        printCurrent();
    } else if (s == KeyStr::g_alt_1) {
        if (getCurSheet())
            getCurSheet()->setMouseShape(Dr::MouseShapeNormal);
    } else if (s == KeyStr::g_alt_2) {
        if (getCurSheet())
            getCurSheet()->setMouseShape(Dr::MouseShapeHand);
    } else if (s == KeyStr::g_ctrl_1) {
        if (getCurSheet()) {
            getCurSheet()->setScaleMode(Dr::ScaleFactorMode);
            getCurSheet()->setScaleFactor(1);
        }
    } else if (s == KeyStr::g_ctrl_m) {
        if (getCurSheet())
            getCurSheet()->setSidebarVisible(true);
    } else if (s == KeyStr::g_ctrl_2) {
        if (getCurSheet())
            getCurSheet()->setScaleMode(Dr::FitToPageHeightMode);
    } else if (s == KeyStr::g_ctrl_3) {
        if (getCurSheet())
            getCurSheet()->setScaleMode(Dr::FitToPageWidthMode);
    } else if (s == KeyStr::g_ctrl_r) {
        if (getCurSheet())
            getCurSheet()->rotateLeft();
    } else if (s == KeyStr::g_ctrl_shift_r) {
        if (getCurSheet())
            getCurSheet()->rotateRight();
    }  else if (s == KeyStr::g_ctrl_larger) {
        if (getCurSheet())
            getCurSheet()->zoomin();
    } else if (s == KeyStr::g_ctrl_equal) {
        if (getCurSheet())
            getCurSheet()->zoomin();
    } else if (s == KeyStr::g_ctrl_smaller) {
        if (getCurSheet())
            getCurSheet()->zoomout();
    } else if (s == KeyStr::g_ctrl_b) {
        if (getCurSheet())
            getCurSheet()->setBookMark(getCurSheet()->currentIndex(), true);
    } else if (s == KeyStr::g_ctrl_f) {
        if (getCurSheet())
            getCurSheet()->handleSearch();
    } else if (s == KeyStr::g_ctrl_c) {
        if (getCurSheet())
            getCurSheet()->copySelectedText();
    } else if (s == KeyStr::g_ctrl_l) {
        if (getCurSheet())
            getCurSheet()->highlightSelectedText();
    } else if (s == KeyStr::g_ctrl_i) {
        if (getCurSheet())
            getCurSheet()->addSelectedTextHightlightAnnotation();
    } else if (s == KeyStr::g_left) {
        if (getCurSheet())
            getCurSheet()->jumpToPrevPage();
    } else if (s == KeyStr::g_right) {
        if (getCurSheet())
            getCurSheet()->jumpToNextPage();
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
    quitSlide();

    m_slideSheet = getCurSheet();

    if (!m_slideSheet.isNull())
        m_slideSheet->openSlide();
}

void CentralDocPage::quitSlide()
{
    if (!m_slideSheet.isNull() && m_slideSheet->slideOpened()) {
        m_slideSheet->closeSlide();
        m_slideSheet = nullptr;
    }
}
