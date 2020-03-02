#include "MainWindow.h"

#include <DDialog>
#include <DTitlebar>
#include <DWidgetUtil>
#include <QSignalMapper>
#include <QDebug>
#include <DGuiApplicationHelper>

#include "MsgModel.h"

#include "business/ShortCutShow.h"
#include "business/SaveDialog.h"
#include "business/AppInfo.h"
#include "docview/docummentproxy.h"
#include "menu/TitleMenu.h"
#include "widgets/TitleWidget.h"
#include "widgets/CentralWidget.h"

#include "business/bridge/IHelper.h"

#include "widgets/main/MainTabWidgetEx.h"

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(DMainWindow *parent)
    : DMainWindow(parent)
{
    setTitlebarShadowEnabled(true);

    m_strObserverName = "MainWindow";
    m_pMsgList = {MSG_OPERATION_EXIT};

    setCurTheme();

    initUI();

    initConnections();

    initThemeChanged();

    initShortCut();

    dApp->m_pModelService->addObserver(this);

    //暂定752*360，后期根据最合适效果设定
    setMinimumSize(752, 360);

    showDefaultSize();

    //  在屏幕中心显示
    Dtk::Widget::moveToCenter(this);
}

MainWindow::~MainWindow()
{
    // We don't need clean pointers because application has exit here.
    dApp->m_pModelService->removeObserver(this);
}

void MainWindow::openfile(const QString &filepath)
{
//    notifyMsg(MSG_OPEN_FILE_PATH, filepath);
}

void MainWindow::setSreenRect(const QRect &rect)
{
    dApp->m_pAppInfo->setScreenRect(rect);
}

//  临时 添加 焦点
void MainWindow::showEvent(QShowEvent *ev)
{
    this->setFocus();
    DMainWindow::showEvent(ev);
}

//  窗口关闭
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    dApp->m_pAppInfo->setAppKeyValue(KEY_APP_WIDTH, QString("%1").arg(this->width()));
    dApp->m_pAppInfo->setAppKeyValue(KEY_APP_HEIGHT, QString("%1").arg(this->height()));

    QJsonObject obj;
    obj.insert("type", "exit_app");

    QJsonDocument doc = QJsonDocument(obj);
    notifyMsg(E_APP_MSG_TYPE, doc.toJson(QJsonDocument::Compact));
}

void MainWindow::initUI()
{
    titlebar()->setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    titlebar()->setTitle("");

    titlebar()->setMenu(new TitleMenu(this));

    titlebar()->addWidget(new TitleWidget, Qt::AlignLeft);
    setCentralWidget(new CentralWidget);
}

void MainWindow::initConnections()
{
    connect(this, SIGNAL(sigFullScreen()), SLOT(slotFullScreen()));
    connect(this, SIGNAL(sigAppShowState(const int &)), SLOT(slotAppShowState(const int &)));
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)),
            SLOT(slotDealWithData(const int &, const QString &)));

    connect(this, &MainWindow::sigSpacePressed, this, []() {
        auto helper = DocummentProxy::instance();
        if (helper) {
            if (helper->getAutoPlaySlideStatu()) {
                helper->setAutoPlaySlide(false);
            } else  {
                helper->setAutoPlaySlide(true);
            }
        }
    });
}

//  放映
void MainWindow::SlotSlideShow()
{
    //  已经是幻灯片了
    if (FILE_SLIDE == dApp->m_pAppInfo->qGetCurShowState()) {
        return;
    }
    slotAppShowState(0);
    notifyMsg(MSG_OPERATION_SLIDE);

}

void MainWindow::initThemeChanged()
{
    //  主题变了
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ]() {
//        setCurTheme();
//        notifyMsg(MSG_OPERATION_UPDATE_THEME);
    });
}

//  设置 主题
void MainWindow::setCurTheme()
{
//    DGuiApplicationHelper::ColorType colorType = DGuiApplicationHelper::instance()->themeType();
//    QString sTheme = "";
//    if (colorType == DGuiApplicationHelper::UnknownType) {  //  未知
//        sTheme = "Unknown";
//    } else if (colorType == DGuiApplicationHelper::LightType) { //  浅色
//        sTheme = "light";
//    } else if (colorType == DGuiApplicationHelper::DarkType) {  //  深色
//        sTheme = "dark";
//    }

//    dApp->m_pAppInfo->qSetCurrentTheme(sTheme);
}

//  显示快捷键
void MainWindow::displayShortcuts()
{
    ShortCutShow show;
    show.show();
}

//  退出 应用
void MainWindow::onAppExit()
{
    close();
}

//  全屏
void MainWindow::slotFullScreen()
{
    int nCurState = dApp->m_pAppInfo->qGetCurShowState();
    if (nCurState != FILE_FULLSCREEN) {
        slotAppShowState(0);
        dApp->m_pAppInfo->qSetCurShowState(FILE_FULLSCREEN);  //  全屏状态
    }
}

/**
 * @brief MainWindow::slotAppShowState
 * @param nState 1 退出全屏, 0 全屏显示
 */
void MainWindow::slotAppShowState(const int &nState)
{
    titlebar()->setVisible(nState);
    // setTitlebarShadowEnabled(nState);

    if (nState == 1) {
        if (windowState() == Qt::WindowFullScreen) {
            dApp->m_pAppInfo->qSetCurShowState(FILE_NORMAL);  //  正常状态

            showNormal();

            if (m_nOldState == Qt::WindowMaximized) {
                showMaximized();
            }

            m_nOldState = Qt::WindowNoState;        // 状态恢复     2019-12-23  wzx
        }
    } else {
        m_nOldState = this->windowState();      //  全屏之前 保存当前状态     2019-12-23  wzx
        this->setWindowState(Qt::WindowFullScreen);
    }
}

//  设置 文档标题
void MainWindow::onSetAppTitle(const QString &sData)
{
    titlebar()->setTitle(sData);
}

void MainWindow::slotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_EXIT) {
        onAppExit();
    }
}

//  快捷键 实现
void MainWindow::slotShortCut(const QString &key)
{
    if (key == KeyStr::g_ctrl_shift_slash) { //  显示快捷键预览
        displayShortcuts();
    } else  if (key == KeyStr::g_ctrl_o) {
        notifyMsg(E_OPEN_FILE);
    } else {
        QJsonObject obj;
        obj.insert("type", "ShortCut");
        obj.insert("key", key);

        QJsonDocument doc = QJsonDocument(obj);
        notifyMsg(E_APP_MSG_TYPE, doc.toJson(QJsonDocument::Compact));
    }
    /*
        auto pMtwe = MainTabWidgetEx::Instance();
        if (pMtwe) {
            QString sFilePath = pMtwe->qGetCurPath();
            if (sFilePath != "") {
                if (key == KeyStr::g_ctrl_h) {  //  播放幻灯片
                    SlotSlideShow();
                } else if (key == KeyStr::g_ctrl_s || key == KeyStr::g_ctrl_shift_s || key == KeyStr::g_ctrl_c) {
                    dApp->m_pHelper->qDealWithData(MSG_NOTIFY_KEY_MSG, key);
                } else {
                    notifyMsg(MSG_NOTIFY_KEY_MSG, key);
                }
            }
        }
    }*/
}

void MainWindow::sendMsg(const int &, const QString &) {}

void MainWindow::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

//  窗口显示默认大小
void MainWindow::showDefaultSize()
{
    int nWidth = dApp->m_pAppInfo->getAppKeyValue(KEY_APP_WIDTH).toInt();
    int nHeight = dApp->m_pAppInfo->getAppKeyValue(KEY_APP_HEIGHT).toInt();

    if (nWidth == 0 || nHeight == 0) {
        resize(1000, 680);

        dApp->m_pAppInfo->setAppKeyValue(KEY_APP_WIDTH, "1000");
        dApp->m_pAppInfo->setAppKeyValue(KEY_APP_HEIGHT, "680");
    } else {
        resize(nWidth, nHeight);
    }
}


//  初始化 快捷键操作
void MainWindow::initShortCut()
{
    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(slotShortCut(const QString &)));

    auto keyList = dApp->m_pAppInfo->getKeyList();
    foreach (auto key, keyList) {
        auto action = new QAction;
        action->setShortcut(key);
        this->addAction(action);

        connect(action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(action, key.toString());
    }
}

int MainWindow::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_f11 && dApp->m_pAppInfo->qGetCurShowState() != FILE_SLIDE) {
            if (dApp->m_pAppInfo->qGetCurShowState() == FILE_FULLSCREEN)
                emit sigAppShowState(1);
            else {
                emit sigFullScreen();
            }
        } else if (msgContent == KeyStr::g_esc) {        //  退出全屏模式
            emit sigAppShowState(1);
        } else if (msgContent == KeyStr::g_space) {
            if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE) {
                emit sigSpacePressed();
            }
        }
    }
    return 0;
}
