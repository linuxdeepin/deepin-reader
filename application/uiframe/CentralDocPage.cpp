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

#include "CentralDocPage.h"
#include "DocSheet.h"
#include "DocTabBar.h"
#include "MainWindow.h"
#include "business/AppInfo.h"
#include "business/SaveDialog.h"
#include "business/PrintManager.h"
#include "docview/docummentproxy.h"
#include "widgets/FindWidget.h"
#include "widgets/FileAttrWidget.h"
#include "widgets/PlayControlWidget.h"
#include "business/FileFormatHelper.h"
#include "docview/docummentproxy.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"
#include "CentralDocPage.h"
#include "FileController.h"

CentralDocPage::CentralDocPage(DWidget *parent)
    : CustomWidget(parent)
{
    initWidget();
    InitConnections();
}

CentralDocPage::~CentralDocPage()
{

}

void CentralDocPage::OpenCurFileFolder()
{
    QString sPath = qGetCurPath();
    if (sPath != "") {
        int nLastPos = sPath.lastIndexOf('/');
        sPath = sPath.mid(0, nLastPos);
        sPath = QString("file://") + sPath;
        QDesktopServices::openUrl(QUrl(sPath));
    }
}

QStringList CentralDocPage::GetAllPath()
{
    QStringList filePathList;

    auto sheets = findChildren<DocSheet *>();
    foreach (auto sheet, sheets) {
        QString filePath = sheet->filePath();
        if (filePath != "") {
            filePathList.append(filePath);
        }
    }
    return filePathList;
}

int CentralDocPage::GetFileChange(const QString &sPath)
{
    auto splitterList = findChildren<DocSheet *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->filePath();
        if (sSplitterPath == sPath) {
            return  s->qGetFileChange();
        }
    }
    return -1;
}

void CentralDocPage::onSheetChanged(DocSheet *sheet)
{
    if (nullptr == sheet && sheet != getCurSheet())
        return;

    sigCurSheetChanged(sheet);

    //...以下要改成记录所有的sheet,遍历一下，查看是否需要阻塞关机,目前只是记录最后一个文档被保存，有问题
    if (sheet->qGetFileChange()) {
        BlockShutdown();
    } else {
        UnBlockShutdown();
    }
}

void CentralDocPage::openFile(QString &filePath)
{
    //判断在打开的文档中是否有filePath，如果有则切到相应的sheet，反之执行打开操作
    if (m_pTabBar) {
        int index = -1;
        index = m_pTabBar->indexOfFilePath(filePath);
        if (index >= 0 && index < m_pTabBar->count()) {
            m_pTabBar->setCurrentIndex(index);
            return;
        }
    }
    filePath = FileController::getUrlInfo(filePath).toLocalFile();

    if (FileController::FileType_PDF == FileController::getFileType(filePath)) {

        DocSheet *sheet = new DocSheet(DocType_PDF, this);

        connect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
        connect(sheet, SIGNAL(sigOpened(DocSheet *, bool)), this, SLOT(onOpened(DocSheet *, bool)));
        connect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

        sheet->openFile(filePath);

        m_pStackedLayout->addWidget(sheet);

        m_pStackedLayout->setCurrentWidget(sheet);

        m_pTabBar->insertSheet(sheet);

        emit sigCurSheetChanged(static_cast<DocSheet *>(m_pStackedLayout->currentWidget()));

        emit sigSheetCountChanged(m_pStackedLayout->count());

    } else
        showTips(tr("The format is not supported"), 1);
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

        return;
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

    if (sheet->qGetFileChange() && 2 == SaveDialog().showDialog()) {
        sheet->saveData();
    }

    m_pStackedLayout->removeWidget(sheet);

    if (m_pTabBar) {
        m_pTabBar->removeSheet(sheet);
    }

    delete sheet;

    sheet = nullptr;

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

    MainWindow *w = MainWindow::createWindow();

    disconnect(sheet, SIGNAL(sigFileChanged(DocSheet *)), this, SLOT(onSheetChanged(DocSheet *)));
    disconnect(sheet, SIGNAL(sigOpened(DocSheet *, bool)), this, SLOT(onOpened(DocSheet *, bool)));
    disconnect(sheet, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));

    w->addSheet(sheet);

    w->move(QCursor::pos());

    w->show();
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

        if (sheet->qGetFileChange())
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

    if (!sheet->qGetFileChange()) {
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
    DocSheet *sheet = static_cast<DocSheet *>(m_pStackedLayout->currentWidget());
    if (nullptr == sheet)
        return false;

    QFileInfo info(sheet->filePath());
    QString sCompleteSuffix = info.completeSuffix();
    DocType_EM nCurDocType = FFH::setCurDocuType(sCompleteSuffix);
    QString sFilter = FFH::getFileFilter(nCurDocType);
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
            auto proxy = getCurSheet()->getDocProxy();
            if (proxy) {
                proxy->closeMagnifier();
            }
        }
    }
}

QStringList CentralDocPage::qGetAllPath()
{
    return GetAllPath();
}

QString CentralDocPage::qGetCurPath()
{
    if (m_pStackedLayout != nullptr) {
        DocSheet *splitter = static_cast<DocSheet *>(m_pStackedLayout->currentWidget());
        if (splitter != nullptr)
            return splitter->filePath();
    }

    return "";
}

int CentralDocPage::getCurFileChanged()
{
    if (qGetCurPath().isEmpty())
        return 0;

    return GetFileChange(qGetCurPath());
}

DocummentProxy *CentralDocPage::getCurFileAndProxy(const QString &sPath)
{
    QString sTempPath = sPath;
    if (sTempPath == "") {
        sTempPath = qGetCurPath();
    }

    auto sheets = this->findChildren<DocSheet *>();
    foreach (auto sheet, sheets) {
        if (sheet->filePath() == sTempPath) {
            return sheet->getDocProxy();
        }
    }
    return nullptr;
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

void CentralDocPage::showPlayControlWidget() const
{
    if (m_pctrlwidget) {
        int nScreenWidth = qApp->desktop()->geometry().width();
        int inScreenHeght = qApp->desktop()->geometry().height();
        int tH = 100;
        m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - tH);
    }
}

void CentralDocPage::initWidget()
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
        if (mainsplit->qGetFileChange())
            return;
    }

    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        m_bBlockShutdown = false;
    }
}

void CentralDocPage::InitConnections()
{
}

void CentralDocPage::onShowFileAttr()
{
    auto pFileAttrWidget = new FileAttrWidget;

    pFileAttrWidget->setFileAttr(getCurSheet());

    pFileAttrWidget->setAttribute(Qt::WA_DeleteOnClose);

    pFileAttrWidget->showScreenCenter();

}

void CentralDocPage::OnAppMsgData(const QString &sText)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(sText.toLocal8Bit().data(), &error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        QString sType = obj.value("type").toString();
        if (sType == "keyPress") {
            QString sKey = obj.value("key").toString();
            OnKeyPress(sKey);
        }
    }
}

void CentralDocPage::handleShortcut(const QString &s)
{
    auto children = this->findChildren<DocSheet *>();

    if (children.size() == 0 && getCurSheet())
        return;

    if (s == KeyStr::g_ctrl_p) {
        OnPrintFile();
    } else if (s == KeyStr::g_ctrl_s) {
        saveCurrent();
    } else if (s == KeyStr::g_ctrl_larger || s == KeyStr::g_ctrl_equal) {
        if (getCurSheet())
            getCurSheet()->zoomin();
    } else if (s == KeyStr::g_ctrl_smaller) {
        if (getCurSheet())
            getCurSheet()->zoomout();
    } else if (s == KeyStr::g_ctrl_b) {
        DocSheet *sheet = getCurSheet();
        if (sheet != nullptr)
            sheet->setBookMark(sheet->getDocProxy()->currentPageNo(), true);
    } else if (s == KeyStr::g_alt_1 || s == KeyStr::g_alt_2 || s == KeyStr::g_ctrl_m  ||
               s == KeyStr::g_ctrl_1 || s == KeyStr::g_ctrl_2 || s == KeyStr::g_ctrl_3 ||
               s == KeyStr::g_ctrl_r || s == KeyStr::g_ctrl_shift_r ||
               s == KeyStr::g_ctrl_c || s == KeyStr::g_ctrl_l || s == KeyStr::g_ctrl_i) {
        if (getCurSheet())
            getCurSheet()->onTitleShortCut(s);
    } else if (s == KeyStr::g_ctrl_f) {     //  搜索
        ShowFindWidget();
    } else if (s == KeyStr::g_ctrl_h) {     //  开启幻灯片
        OnOpenSliderShow();
    } else if (s == KeyStr::g_ctrl_shift_s) {   //  另存为
        saveAsCurrent();//saveAsCurFile();
    } else if (s == KeyStr::g_alt_z) {
        OnOpenMagnifer();
    } else if (s == KeyStr::g_esc) { //  esc 统一处理
        exitSpecialState();
    }
}

//  打印
void CentralDocPage::OnPrintFile()
{
    if (nullptr == getCurSheet())
        return;

    PrintManager p(getCurSheet());
    p.showPrintDialog(this);
}

void CentralDocPage::exitSpecialState()
{
    int nState = getCurrentState();
    if (nState == SLIDER_SHOW) {  //  当前是幻灯片模式
        OnExitSliderShow();
    } else if (nState == Magnifer_State) {
        OnExitMagnifer();
    }

    setCurrentState(Default_State);
}

void CentralDocPage::OnKeyPress(const QString &sKey)
{
    int nState = getCurrentState();
    if (nState == SLIDER_SHOW && m_pctrlwidget) {
        if (sKey == KeyStr::g_space) {
            auto helper = getCurFileAndProxy(m_strSliderPath);
            if (helper) {
                if (helper->getAutoPlaySlideStatu()) {
                    helper->setAutoPlaySlide(false);
                } else  {
                    helper->setAutoPlaySlide(true);
                }
            }
        }

        m_pctrlwidget->PageChangeByKey(sKey);
    }
}

void CentralDocPage::setCurrentState(const int &nCurrentState)
{
    m_nCurrentState = nCurrentState;
}

void CentralDocPage::showTips(const QString &tips, int iconIndex)
{
    emit sigNeedShowTips(tips, iconIndex);
}

int CentralDocPage::getCurrentState()
{
    return m_nCurrentState;
}

//  搜索框
void CentralDocPage::ShowFindWidget()
{
    int nState = getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    DWidget *w = m_pStackedLayout->currentWidget();
    if (w) {
        auto sheet = qobject_cast<DocSheet *>(w);
        if (sheet) {
            sheet->ShowFindWidget();
        }
    }
}

//  开启 幻灯片
void CentralDocPage::OnOpenSliderShow()
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

        QString sPath = sheet->filePath();

        m_strSliderPath = sPath;

        auto _proxy = sheet->getDocProxy();

        _proxy->setAutoPlaySlide(true);

        _proxy->showSlideModel();

        if (m_pctrlwidget == nullptr) {
            m_pctrlwidget = new PlayControlWidget(sheet, this);
        }

        m_pctrlwidget->setSliderPath(sPath);

        int nScreenWidth = qApp->desktop()->geometry().width();

        int inScreenHeght = qApp->desktop()->geometry().height();

        m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - 100);

    }
}

//  退出幻灯片
void CentralDocPage::OnExitSliderShow()
{
    int nState = getCurrentState();

    if (nState == SLIDER_SHOW) {

        setCurrentState(Default_State);

        emit sigNeedShowState(1);

        m_pTabBar->setVisible(true);

        auto sheet = getSheet(m_strSliderPath);

        if (sheet) {

            sheet->OnExitSliderShow();

            DocummentProxy *_proxy = sheet->getDocProxy();

            if (!_proxy) {
                return;
            }
            _proxy->exitSlideModel();

            delete m_pctrlwidget;

            m_pctrlwidget = nullptr;
        }

        m_strSliderPath = "";
    }
}

bool CentralDocPage::OnOpenMagnifer()
{
    int nState = getCurrentState();

    if (nState != Magnifer_State) {

        setCurrentState(Magnifer_State);

        m_strMagniferPath = qGetCurPath();

        return true;

    }

    return false;
}

//  取消放大镜
void CentralDocPage::OnExitMagnifer()
{
    int nState = getCurrentState();
    if (nState == Magnifer_State) {
        setCurrentState(Default_State);

        auto sheet = qobject_cast<DocSheet *>(m_pStackedLayout->currentWidget());
        if (sheet) {
            QString sPath = sheet->filePath();
            DocummentProxy *_proxy = getCurFileAndProxy(sPath);
            if (!_proxy) {
                return;
            }
            _proxy->closeMagnifier();
        }
    }
}
