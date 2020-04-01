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

#include "CentralDocPage.h"
#include "DocSheet.h"
#include "DocTabBar.h"
#include "MainWindow.h"
#include "TitleMenu.h"

#include "business/AppInfo.h"
#include "business/SaveDialog.h"
#include "business/PrintManager.h"
#include "docview/docummentproxy.h"

#include "widgets/FindWidget.h"
#include "widgets/FileAttrWidget.h"
#include "widgets/PlayControlWidget.h"
#include "TitleWidget.h"

#include "gof/bridge/IHelper.h"

CentralDocPage *CentralDocPage::g_onlyApp = nullptr;

CentralDocPage::CentralDocPage(DWidget *parent)
    : CustomWidget(MAIN_TAB_WIDGET, parent)
{
    initWidget();
    InitConnections();
    g_onlyApp = this;
    m_pMsgList = {E_APP_MSG_TYPE, E_TABBAR_MSG_TYPE, MSG_FILE_IS_CHANGE};
    dApp->m_pModelService->addObserver(this);
}

CentralDocPage::~CentralDocPage()
{
    dApp->m_pModelService->removeObserver(this);
}
//  打开当前所在文件夹
void CentralDocPage::OpenCurFileFolder()
{
    QString sPath = GetCurPath();
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
        QString filePath = sheet->qGetPath();
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
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            return  s->qGetFileChange();
        }
    }
    return -1;
}

QString CentralDocPage::GetCurPath()
{
    if (m_pStackedLayout != nullptr) {
        DocSheet *splitter = static_cast<DocSheet *>(m_pStackedLayout->currentWidget());
        if (splitter != nullptr)
            return splitter->qGetPath();
    }

    return "";

//    auto splitterList = findChildren<DocSheet *>();
//    foreach (auto s, splitterList) {
//        if (s->isVisible()) {
//            return s->qGetPath();
//        }
//    }
//    return "";
}

void CentralDocPage::SetFileChange(const QString &sPath, const int &iState)
{
    auto splitterList = findChildren<DocSheet *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            s->qSetFileChange(iState);
            break;
        }
    }
}
CentralDocPage *CentralDocPage::Instance()
{
    return g_onlyApp;
}

int CentralDocPage::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == E_TABBAR_MSG_TYPE) {
        OnTabBarMsg(msgContent);
    } else if (msgType == E_APP_MSG_TYPE) {     //  应用类消息
        OnAppMsgData(msgContent);
    } else if (msgType == MSG_FILE_IS_CHANGE) {
        OnTabFileChangeMsg(msgContent);
    } else {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(msgContent.toLocal8Bit().data(), &error);
        if (error.error == QJsonParseError::NoError) {
            QJsonObject obj = doc.object();
            QString sTo = obj.value("to").toString();

            if (sTo.contains(this->m_strObserverName)) {
                emit sigDealNotifyMsg(msgType, msgContent);
                return MSG_OK;
            }
        }
    }


    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}

QStringList CentralDocPage::qGetAllPath()
{
    return GetAllPath();
}

QString CentralDocPage::qGetCurPath()
{

    return GetCurPath();
}

int CentralDocPage::getFileChanged()
{
    if (qGetCurPath().isEmpty())
        return 0;

    return GetFileChange(qGetCurPath());
}

FileDataModel CentralDocPage::qGetFileData(const QString &sPath)
{
    QString sTempPath = sPath;
    if (sTempPath == "") {
        sTempPath = qGetCurPath();
    }
    auto splitterList = this->findChildren<DocSheet *>();
    foreach (auto sP, splitterList) {
        QString sPPath = sP->qGetPath();
        if (sPPath == sTempPath) {
            return sP->qGetFileData();
        }
    }
    return FileDataModel();
}

DocummentProxy *CentralDocPage::getCurFileAndProxy(const QString &sPath)
{
    QString sTempPath = sPath;
    if (sTempPath == "") {
        sTempPath = qGetCurPath();
    }

    auto sheets = this->findChildren<DocSheet *>();
    foreach (auto sheet, sheets) {
        if (sheet->qGetPath() == sTempPath) {
            return sheet->getDocProxy();
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
        int tW = 0;
        dApp->adaptScreenView(tW, tH);
        m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - tH);
    }
}

void CentralDocPage::initWidget()
{
    m_pTabBar = new DocTabBar;
    connect(m_pTabBar, SIGNAL(sigTabBarIndexChange(const QString &)), SLOT(SlotSetCurrentIndexFile(const QString &)));
    connect(m_pTabBar, SIGNAL(sigAddTab(const QString &)), SLOT(SlotAddTab(const QString &)));
    connect(m_pTabBar, SIGNAL(sigCloseTab(const QString &)), SLOT(SlotCloseTab(const QString &)));

    connect(this, SIGNAL(sigRemoveFileTab(const QString &)), m_pTabBar, SLOT(SlotRemoveFileTab(const QString &)));
    connect(this, SIGNAL(sigOpenFileResult(const QString &, const bool &)), m_pTabBar, SLOT(SlotOpenFileResult(const QString &, const bool &)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pTabBar);

    m_pStackedLayout = new QStackedLayout;

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
        qDebug() << "m_reply.value().isValid():" << m_reply.value().isValid();
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
        qDebug() << "Nublock shutdown.";
        m_bBlockShutdown = false;
    }
}

void CentralDocPage::InitConnections()
{
}

void CentralDocPage::onShowFileAttr()
{
    auto pFileAttrWidget = new FileAttrWidget;
    pFileAttrWidget->showScreenCenter();
}

void CentralDocPage::OnAppMsgData(const QString &sText)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(sText.toLocal8Bit().data(), &error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        QString sType = obj.value("type").toString();
        if (sType == "exit_app") {
            OnAppExit();
        } else if (sType == "ShortCut") {
            QString sKey = obj.value("key").toString();
            OnAppShortCut(sKey);
        } else if (sType == "keyPress") {
            QString sKey = obj.value("key").toString();
            OnKeyPress(sKey);
        }
    }
}

//  应用退出, 删除所有文件
void CentralDocPage::OnAppExit()
{
    QStringList saveFileList;
    QStringList noChangeFileList;

    auto splitterList = this->findChildren<DocSheet *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        int iChange = s->qGetFileChange();
        if (iChange == 1) {
            saveFileList.append(sSplitterPath);
        } else {
            noChangeFileList.append(sSplitterPath);
        }
    }

    if (saveFileList.size() > 0) {
        SaveDialog sd;
        int nRes = sd.showDialog();
        if (nRes <= 0) {
            return;
        }

        int nMsgType = MSG_NOT_SAVE_FILE;
        if (nRes == 2) {     //  保存
            nMsgType = MSG_SAVE_FILE;
        }

        foreach (auto s, splitterList) {
            QString sSplitterPath = s->qGetPath();
            if (saveFileList.contains(sSplitterPath)) {
                SaveFile(nMsgType, sSplitterPath);
            }
        }
    }

    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (noChangeFileList.contains(sSplitterPath)) {
            SaveFile(MSG_NOT_CHANGE_SAVE_FILE, sSplitterPath);
        }
    }
    foreach (auto s, splitterList) {
        s->getDocProxy()->closeFileAndWaitThreadClearEnd();
    }
    topLevelWidget()->hide();
    dApp->exit(0);
}

void CentralDocPage::OnAppShortCut(const QString &s)
{
    auto children = this->findChildren<DocSheet *>();
    if (children.size() == 0)
        return;

    if (s == KeyStr::g_ctrl_p) {
        OnPrintFile();
    } else if (s == KeyStr::g_ctrl_s) {
        OnSaveFile();
    } else if (s == KeyStr::g_alt_1 || s == KeyStr::g_alt_2 || s == KeyStr::g_ctrl_m  ||
               s == KeyStr::g_ctrl_1 || s == KeyStr::g_ctrl_2 || s == KeyStr::g_ctrl_3 ||
               s == KeyStr::g_ctrl_r || s == KeyStr::g_ctrl_shift_r ||
               s == KeyStr::g_ctrl_larger || s == KeyStr::g_ctrl_equal || s == KeyStr::g_ctrl_smaller) {
        TitleWidget::Instance()->qDealWithShortKey(s);
    } else if (s == KeyStr::g_ctrl_f) {     //  搜索
        ShowFindWidget();
    } else if (s == KeyStr::g_ctrl_h) {     //  开启幻灯片
        OnOpenSliderShow();
    } else if (s == KeyStr::g_ctrl_shift_s) {   //  另存为
        OnSaveAsFile();
    } else if (s == KeyStr::g_esc) {   //  esc 统一处理
        OnShortCutKey_Esc();
    } else {
        emit sigDealNotifyMsg(MSG_NOTIFY_KEY_MSG, s);
    }
}

void CentralDocPage::OnTabBarMsg(const QString &s)
{
    if (s == "New window") {
        Utils::runApp(QString());
    } else if (s == "New tab") {
        notifyMsg(E_OPEN_FILE);
    } else if (s == "Save") { //  保存当前显示文件
        OnSaveFile();
    } else if (s == "Save as") {
        OnSaveAsFile();
    } else if (s == "Print") {
        OnPrintFile();
    } else if (s == "Slide show") { //  开启幻灯片
        OnOpenSliderShow();
    } else if (s == "Magnifer") {   //  开启放大镜
        OnOpenMagnifer();
    } else if (s == "Document info") {
        onShowFileAttr();
    } else if (s == "Display in file manager") {    //  文件浏览器 显示

        OpenCurFileFolder();
    }
}

void CentralDocPage::OnTabFileChangeMsg(const QString &sVale)
{

    QString sCurPath = qGetCurPath();

    SetFileChange(sCurPath, sVale.toInt());
}

void CentralDocPage::SaveFile(const int &iType, const QString &sPath)
{
    QString sRes = dApp->m_pHelper->qDealWithData(iType, sPath);
    if (sRes != "") {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(sRes.toLocal8Bit().data(), &error);
        if (error.error == QJsonParseError::NoError) {
            QJsonObject obj = doc.object();
            int nReturn = obj.value("return").toInt();
            if (nReturn == MSG_OK) {
                DWidget *w = m_pStackedLayout->currentWidget();
                if (w) {
                    auto pSplitter = qobject_cast<DocSheet *>(w);
                    if (pSplitter) {
                        pSplitter->saveData();
                    }
                }
            }
        }
    }

    TitleMenu::Instance()->flushSaveButton();
}

//  保存当前显示文件
void CentralDocPage::OnSaveFile()
{
    DWidget *w = m_pStackedLayout->currentWidget();
    if (w) {
        auto pSplitter = qobject_cast<DocSheet *>(w);
        if (pSplitter) {
            int nChange = pSplitter->qGetFileChange();
            if (nChange == 1) {
                QString sPath = pSplitter->qGetPath();
                SaveFile(MSG_SAVE_FILE, sPath);
            }
        }
    }
}

void CentralDocPage::OnSaveAsFile()
{
    QString sRes = dApp->m_pHelper->qDealWithData(MSG_SAVE_AS_FILE_PATH, "");
}

//  打印
void CentralDocPage::OnPrintFile()
{
    QString sPath = qGetCurPath();
    if (sPath != "") {
        PrintManager p(sPath);
        p.showPrintDialog(this);
    }
}

void CentralDocPage::OnShortCutKey_Esc()
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

void CentralDocPage::slotfilechanged(bool bchanged)
{
    if (bchanged) {
        BlockShutdown();
    } else {
        UnBlockShutdown();
    }
}

//  切换文档, 需要取消之前文档 放大镜模式
void CentralDocPage::SlotSetCurrentIndexFile(const QString &sPath)
{

    auto splitterList = this->findChildren<DocSheet *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {

            //  切换文档 需要将放大镜状态 取消
            int nState = getCurrentState();
            if (nState == Magnifer_State) {
                setCurrentState(Default_State);

                auto proxy = getCurFileAndProxy(m_strMagniferPath);
                if (proxy) {
                    proxy->closeMagnifier();
                }
            }

            int iIndex = m_pStackedLayout->indexOf(s);
            m_pStackedLayout->setCurrentIndex(iIndex);

            notifyMsg(MSG_TAB_SHOW_FILE_CHANGE, sPath);

            break;
        }
    }
}

void CentralDocPage::SlotAddTab(const QString &sPath)
{
    if (m_pStackedLayout) {
        DocSheet *splitter = new DocSheet(DocType_PDF, this);
        connect(this, SIGNAL(sigDealNotifyMsg(const int &, const QString &)), splitter, SLOT(SlotNotifyMsg(const int &, const QString &)));
        connect(splitter, SIGNAL(sigOpenFileResult(const QString &, const bool &)), SLOT(SlotOpenFileResult(const QString &, const bool &)));
        splitter->qSetPath(sPath);
        m_pStackedLayout->addWidget(splitter);
    }
}

//  删除单个文件
void CentralDocPage::SlotCloseTab(const QString &sPath)
{
    auto splitterList = this->findChildren<DocSheet *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            QString sRes  = "";
            int iChange = s->qGetFileChange();
            if (iChange == 1) {
                SaveDialog sd;
                int nRes = sd.showDialog();
                if (nRes <= 0) {
                    return;
                }

                int nMsgType = MSG_NOT_SAVE_FILE;
                if (nRes == 2) {     //  保存
                    nMsgType = MSG_SAVE_FILE;
                }

                QString s = sPath + Constant::sQStringSep;
                sRes = dApp->m_pHelper->qDealWithData(nMsgType, s);
            } else {
                QString s = sPath + Constant::sQStringSep;
                sRes = dApp->m_pHelper->qDealWithData(MSG_NOT_CHANGE_SAVE_FILE, s);
            }

            if (sRes != "") {
                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson(sRes.toLocal8Bit().data(), &error);
                if (error.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    int nReturn = obj.value("return").toInt();
                    if (nReturn == MSG_OK) {
                        //  保存成功
                        s->saveData();

                        emit sigRemoveFileTab(sPath);

                        m_pStackedLayout->removeWidget(s);

                        delete  s;
                        s = nullptr;
                    }
                }
            }
            break;
        }
    }
}

void CentralDocPage::SlotOpenFileResult(const QString &sPath, const bool &res)
{
    if (!res) { //  打开失败了
        auto splitterList = this->findChildren<DocSheet *>();
        foreach (auto s, splitterList) {
            QString sSplitterPath = s->qGetPath();
            if (sSplitterPath == sPath) {
                m_pStackedLayout->removeWidget(s);

                delete s;
                s = nullptr;
                break;
            }
        }
    }

    emit sigOpenFileResult(sPath, res);
}

void CentralDocPage::setCurrentState(const int &nCurrentState)
{
    m_nCurrentState = nCurrentState;
}

void CentralDocPage::SetFileChange()
{

    SetFileChange(qGetCurPath(), 1);
}

void CentralDocPage::setCurrentTabByFilePath(const QString &filePath)
{
    auto splitterList = this->findChildren<DocSheet *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == filePath) {
            int index = m_pTabBar->indexOfFilePath(filePath);
            if (-1 != index)
                m_pTabBar->setCurrentIndex(index);
        }
    }
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
    int nState = getCurrentState();
    if (nState != SLIDER_SHOW) {
        setCurrentState(SLIDER_SHOW);

        m_pTabBar->setVisible(false);

        auto sheet = qobject_cast<DocSheet *>(m_pStackedLayout->currentWidget());
        if (sheet) {
            sheet->OnOpenSliderShow();

            MainWindow::Instance()->SetSliderShowState(0);

            QString sPath = sheet->qGetPath();

            m_strSliderPath = sPath;

            auto _proxy = getCurFileAndProxy(sPath);
            _proxy->setAutoPlaySlide(true);
            _proxy->showSlideModel();

            if (m_pctrlwidget == nullptr) {
                m_pctrlwidget = new PlayControlWidget(this);
            }

            m_pctrlwidget->setSliderPath(sPath);
            int nScreenWidth = qApp->desktop()->geometry().width();
            int inScreenHeght = qApp->desktop()->geometry().height();
            m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - 100);
        }
    }
}

//  退出幻灯片
void CentralDocPage::OnExitSliderShow()
{


    int nState = getCurrentState();
    if (nState == SLIDER_SHOW) {
        setCurrentState(Default_State);

        MainWindow::Instance()->SetSliderShowState(1);
        m_pTabBar->setVisible(true);

        auto sheet = qobject_cast<DocSheet *>(m_pStackedLayout->currentWidget());
        if (sheet) {
            sheet->OnExitSliderShow();

            DocummentProxy *_proxy = getCurFileAndProxy(m_strSliderPath);
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

void CentralDocPage::OnOpenMagnifer()
{


    int nState = getCurrentState();
    if (nState != Magnifer_State) {

        TitleWidget::Instance()->setMagnifierState();

        setCurrentState(Magnifer_State);
        m_strMagniferPath = qGetCurPath();
    }
}

//  取消放大镜
void CentralDocPage::OnExitMagnifer()
{
    int nState = getCurrentState();
    if (nState == Magnifer_State) {
        setCurrentState(Default_State);

        auto sheet = qobject_cast<DocSheet *>(m_pStackedLayout->currentWidget());
        if (sheet) {
            QString sPath = sheet->qGetPath();
            DocummentProxy *_proxy = getCurFileAndProxy(sPath);
            if (!_proxy) {
                return;
            }
            _proxy->closeMagnifier();
        }
    }
}
