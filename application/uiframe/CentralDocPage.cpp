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

#include "CentralDocPage.h"
#include "DocSheet.h"
#include "DocTabBar.h"
#include "MainWindow.h"
#include "business/AppInfo.h"
#include "business/SaveDialog.h"

#include "pdfControl/docview/docummentproxy.h"
#include "widgets/FindWidget.h"
#include "widgets/FileAttrWidget.h"
#include "widgets/PlayControlWidget.h"
#include "pdfControl/docview/docummentproxy.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"
#include "CentralDocPage.h"
#include "global.h"
#include "pdfControl/DocSheetPDF.h"
#include "djvuControl/DocSheetDJVU.h"

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
    int nLastPos = filePath.lastIndexOf('/');
    filePath = filePath.mid(0, nLastPos);
    filePath = QString("file://") + filePath;
    QDesktopServices::openUrl(QUrl(filePath));
}

void CentralDocPage::onSheetChanged(DocSheet *sheet)
{
    if (nullptr == sheet && sheet != getCurSheet())
        return;

    sigCurSheetChanged(sheet);

    //...以下要改成记录所有的sheet,遍历一下，查看是否需要阻塞关机,目前只是记录最后一个文档被保存，有问题
    if (sheet->getFileChanged()) {
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

        m_pTabBar->insertSheet(sheet);

        emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

        emit sigSheetCountChanged(m_pStackedLayout->count());

    } else if (Dr::DjVu == fileType) {
        DocSheet *sheet = new DocSheetDJVU(filePath, this);

        connect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
        connect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

        if (!sheet->openFileExec())
            return;

        m_pStackedLayout->addWidget(sheet);

        m_pStackedLayout->setCurrentWidget(sheet);

        m_pTabBar->insertSheet(sheet);

        onOpened(sheet, true);

    } else {
        showTips(tr("The format is not supported"), 1);
    }


}

void CentralDocPage::onOpened(DocSheet *sheet, bool ret)
{
    this->activateWindow();
    //文档刚打开时，模拟鼠标点击文档区域事件
    QPoint pos(this->geometry().x(), this->geometry().y());

    QMouseEvent event0(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(this, &event0);

    if (!ret) {
        m_pStackedLayout->removeWidget(sheet);

        m_pTabBar->removeSheet(sheet);

        emit sigSheetCountChanged(m_pStackedLayout->count());

        emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

    }

}

void CentralDocPage::onTabChanged(DocSheet *sheet)
{
    clearState();

    emit sigCurSheetChanged(sheet);

    if (nullptr != sheet)
        m_pStackedLayout->setCurrentWidget(sheet);
}

void CentralDocPage::onTabMoveIn(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    sheet->setParent(this);

    m_pStackedLayout->addWidget(sheet);

    m_pStackedLayout->setCurrentWidget(sheet);

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));
}

void CentralDocPage::onTabClosed(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    sheet->saveOper();

    if (sheet->getFileChanged() && 2 == SaveDialog().showDialog()) {
        sheet->saveData();
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

    m_pStackedLayout->removeWidget(sheet);

    if (m_pStackedLayout->count() <= 0) {
        emit sigNeedClose();
        return;
    }

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));
}

void CentralDocPage::onTabNewWindow(DocSheet *sheet)
{
    m_pStackedLayout->removeWidget(sheet);

    emit sigSheetCountChanged(m_pStackedLayout->count());

    emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

    disconnect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
    disconnect(sheet, SIGNAL(sigOpened(DocSheet *, bool)), this, SLOT(onOpened(DocSheet *, bool)));
    disconnect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

    MainWindow *window = MainWindow::createWindow();

    window->addSheet(sheet);

    window->move(QCursor::pos());

    window->show();
}

void CentralDocPage::onCentralMoveIn(DocSheet *sheet)
{
    addSheet(sheet);
}

void CentralDocPage::addSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    sheet->setParent(this);

    connect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
    connect(sheet, SIGNAL(sigOpened(DocSheet *, bool)), this, SLOT(onOpened(DocSheet *, bool)));
    connect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

    m_pTabBar->insertSheet(sheet);

    m_pStackedLayout->addWidget(sheet);

    m_pStackedLayout->setCurrentWidget(sheet);

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
        sheet->saveOper();

        if (sheet->getFileChanged())
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

    if (!sheet->getFileChanged()) {
        return false;
    }

    if (!sheet->saveData()) {
        showTips(tr("Save failed"));
        return false;
    }

    sheet->saveOper();

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

    QString saveFilePath;

    if (sFilter != "") {
        QFileDialog dialog;
        dialog.selectFile(sheet->filePath());
        saveFilePath = dialog.getSaveFileName(nullptr, tr("Save as"), sheet->filePath(), sFilter);

        if (saveFilePath.endsWith("/.pdf")) {
            DDialog dlg("", tr("Invalid file name"));
            QIcon icon(PF::getIconPath("exception-logo"));
            dlg.setIcon(icon /*QIcon(":/resources/exception-logo.svg")*/);
            dlg.addButtons(QStringList() << tr("OK"));
            QMargins mar(0, 0, 0, 30);
            dlg.setContentLayoutContentsMargins(mar);
            dlg.exec();
            return false;
        }
    }

    return sheet->saveAsData(saveFilePath);
}

void CentralDocPage::clearState()
{
    //  切换文档 需要将放大镜状态 取消
    int nState = getCurrentState();

    if (nState == Magnifer_State) {

        setCurrentState(Default_State);

        if (getCurSheet() != nullptr) {         //...需要修改为，要保存正在放大镜的doc
            getCurSheet()->closeMagnifier();
        }
    }
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

void CentralDocPage::showPlayControlWidget()
{
    if (m_pctrlwidget) {
        int nScreenWidth = qApp->desktop()->geometry().width();
        int inScreenHeght = qApp->desktop()->geometry().height();
        int tH = 100;
        m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - tH);
    }
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
        if (mainsplit->getFileChanged())
            return;
    }

    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        m_bBlockShutdown = false;
    }
}

void CentralDocPage::setCurrentState(const int &currentState)
{
    m_currentState = currentState;
}

int CentralDocPage::getCurrentState()
{
    return m_currentState;
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
    if (getCurrentState() == SLIDER_SHOW) {
        if (s == KeyStr::g_esc) {
            quitSlide();
        }
        if (nullptr == m_pctrlwidget) {
            setCurrentState(Default_State);
            return;
        }

        if (!m_slideSheet.isNull()) {
            if (!m_slideSheet.isNull()) {
                if (m_slideSheet->getAutoPlaySlideStatu()) {
                    m_slideSheet->setAutoPlaySlide(false);
                } else  {
                    m_slideSheet->setAutoPlaySlide(true);
                }
            }
        }
        m_pctrlwidget->PageChangeByKey(s);
        return;
    }

    if (getCurrentState() == Magnifer_State) {
        if (s == KeyStr::g_esc) {
            quitMagnifer();
        }
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
        if (getCurSheet())
            getCurSheet()->print();
    } else if (s == KeyStr::g_alt_1) {
        if (getCurSheet())
            getCurSheet()->setMouseDefault();
    } else if (s == KeyStr::g_alt_2) {
        if (getCurSheet())
            getCurSheet()->setMouseHand();
    } else if (s == KeyStr::g_ctrl_1) {
        if (getCurSheet()) {
            getCurSheet()->setFit(NO_ADAPTE_State);
            getCurSheet()->setScale(100);
        }
    } else if (s == KeyStr::g_ctrl_m) {
        if (getCurSheet())
            getCurSheet()->openSideBar();
    } else if (s == KeyStr::g_ctrl_2) {
        if (getCurSheet())
            getCurSheet()->setFit(ADAPTE_HEIGHT_State);
    } else if (s == KeyStr::g_ctrl_3) {
        if (getCurSheet())
            getCurSheet()->setFit(ADAPTE_WIDGET_State);
    } else if (s == KeyStr::g_ctrl_r) {
        if (getCurSheet())
            getCurSheet()->setRotateLeft();
    } else if (s == KeyStr::g_ctrl_shift_r) {
        if (getCurSheet())
            getCurSheet()->setRotateRight();
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
            getCurSheet()->setBookMark(getCurSheet()->currentPageNo(), true);
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
    }
}

void CentralDocPage::quitSpecialState()
{
    int nState = getCurrentState();
    if (nState == SLIDER_SHOW) {  //  当前是幻灯片模式
        quitSlide();
    } else if (nState == Magnifer_State) {
        quitMagnifer();
    }

    setCurrentState(Default_State);
}

void CentralDocPage::showTips(const QString &tips, int iconIndex)
{
    emit sigNeedShowTips(tips, iconIndex);
}

void CentralDocPage::openSlide()
{
    DocSheet *sheet = getCurSheet();

    if (nullptr == sheet)
        return;

    int nState = getCurrentState();

    if (nState != SLIDER_SHOW) {

        setCurrentState(SLIDER_SHOW);

        m_pTabBar->setVisible(false);

        sheet->OnOpenSliderShow();

        emit sigNeedShowState(0);

        m_slideSheet = sheet;

        sheet->setAutoPlaySlide(true);
        sheet->showSlideModel();

        if (m_pctrlwidget == nullptr) {
            m_pctrlwidget = new PlayControlWidget(sheet, this);
        }

        int nScreenWidth = qApp->desktop()->geometry().width();

        int inScreenHeght = qApp->desktop()->geometry().height();

        m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - 100);

    }
}

//  退出幻灯片
void CentralDocPage::quitSlide()
{
    int nState = getCurrentState();

    if (nState == SLIDER_SHOW) {

        setCurrentState(Default_State);

        emit sigNeedShowState(1);

        m_pTabBar->setVisible(true);

        if (!m_slideSheet.isNull()) {

            m_slideSheet->exitSliderShow();
            m_slideSheet->exitSlideModel();

            delete m_pctrlwidget;

            m_pctrlwidget = nullptr;
        }
    }
}

bool CentralDocPage::openMagnifer()
{
    int nState = getCurrentState();

    if (nState != Magnifer_State) {

        setCurrentState(Magnifer_State);

        m_magniferSheet = getCurSheet();

        return true;
    }

    return false;
}

//  取消放大镜
void CentralDocPage::quitMagnifer()
{
    int nState = getCurrentState();
    if (nState == Magnifer_State) {
        setCurrentState(Default_State);

        if (m_magniferSheet.isNull())
            return;
        m_magniferSheet->closeMagnifier();
    }
}
