#include "MainWindow.h"

#include <DDialog>
#include <DTitlebar>
#include <DWidgetUtil>
#include <QSignalMapper>
#include <QDebug>
#include <DGuiApplicationHelper>

#include "application.h"

#include "business/ShortCutShow.h"
#include "business/DocummentFileHelper.h"
#include "controller/DataManager.h"
#include "controller/AppSetting.h"
#include "docview/docummentproxy.h"
#include "menu/TitleMenu.h"
#include "widgets/TitleWidget.h"
#include "widgets/CentralWidget.h"

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

    m_pNotifySubject = g_NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }

    //暂定752*360，后期根据最合适效果设定
    setMinimumSize(752, 360);

    showDefaultSize();

    //  在屏幕中心显示
    Dtk::Widget::moveToCenter(this);
}

MainWindow::~MainWindow()
{
    // We don't need clean pointers because application has exit here.
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

void MainWindow::openfile(const QString &filepath)
{
    notifyMsg(MSG_OPEN_FILE_PATH, filepath);
}

void MainWindow::setSreenRect(const QRect &rect)
{
    DataManager::instance()->setScreenRect(rect);
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
    QString sFilePath = DataManager::instance()->strOnlyFilePath();
    if (sFilePath != "") {
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            DDialog dlg("", tr("Do you want to save the changes?"));
            dlg.setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
            dlg.addButtons(QStringList() <<  tr("Cancel") << tr("Discard"));
            dlg.addButton(tr("Save"), true, DDialog::ButtonRecommend);
            QMargins mar(0, 0, 0, 30);
            dlg.setContentLayoutContentsMargins(mar);

            int nRes = dlg.exec();
            if (nRes <= 0) {
                event->ignore();
                return;
            }

            if (nRes == 2) {
                //  保存
                DocummentFileHelper::instance()->save(sFilePath, true);
                //  保存 书签数据r
                dApp->m_BookMarkDB->saveData();
            }
        }

        //  保存文档字号参数信息
        dApp->m_histroyDB->saveData();
        notifyMsg(MSG_CLOSE_FILE);

        DocummentProxy *_proxy = DocummentProxy::instance();
        if (_proxy) {
            _proxy->closeFile();
        }
    }

    AppSetting::instance()->setAppKeyValue(KEY_APP_WIDTH, QString("%1").arg(this->width()));
    AppSetting::instance()->setAppKeyValue(KEY_APP_HEIGHT, QString("%1").arg(this->height()));
    hide();
    DMainWindow::closeEvent(event);
}

void MainWindow::initUI()
{
    titlebar()->setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    titlebar()->setTitle("");

    TitleMenu *menu = new TitleMenu(this);
    connect(menu, SIGNAL(sigSetSlideShow()), SLOT(SlotSlideShow()));
    titlebar()->setMenu(menu);

    titlebar()->addWidget(new TitleWidget, Qt::AlignLeft);
    setCentralWidget(new CentralWidget);
}

void MainWindow::initConnections()
{
    connect(this, SIGNAL(sigOpenFileOk(const QString &)), SLOT(slotOpenFileOk(const QString &)));
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
    if (FILE_SLIDE == DataManager::instance()->CurShowState()) {
        return;
    }
    slotAppShowState(0);
    notifyMsg(MSG_OPERATION_SLIDE);

}

void MainWindow::initThemeChanged()
{
    //  主题变了
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ]() {
        setCurTheme();
        notifyMsg(MSG_OPERATION_UPDATE_THEME);
    });
}

//  设置 主题
void MainWindow::setCurTheme()
{
    DGuiApplicationHelper::ColorType colorType = DGuiApplicationHelper::instance()->themeType();
    QString sTheme = "";
    if (colorType == DGuiApplicationHelper::UnknownType) {  //  未知
        sTheme = "Unknown";
    } else if (colorType == DGuiApplicationHelper::LightType) { //  浅色
        sTheme = "light";
    } else if (colorType == DGuiApplicationHelper::DarkType) {  //  深色
        sTheme = "dark";
    }

    DataManager::instance()->settrCurrentTheme(sTheme);
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
    int nCurState = DataManager::instance()->CurShowState();
    if (nCurState != FILE_FULLSCREEN) {
        slotAppShowState(0);
        DataManager::instance()->setCurShowState(FILE_FULLSCREEN);  //  全屏状态
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
            DataManager::instance()->setCurShowState(FILE_NORMAL);  //  正常状态

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

//  文件打开成，　功能性　菜单才能点击
void MainWindow::slotOpenFileOk(const QString &msgContent)
{
    auto actions = titlebar()->menu()->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setDisabled(false);
    }

    onSetAppTitle(msgContent);
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
    } else if (key == KeyStr::g_ctrl_o) {   //  打开文件
        notifyMsg(MSG_NOTIFY_KEY_MSG, key);
    } else {
        QString sFilePath = DataManager::instance()->strOnlyFilePath();     //  没有打开的文件
        if (sFilePath != "") {
            if (key == KeyStr::g_ctrl_h) {  //  播放幻灯片
                SlotSlideShow();
            } else {
                notifyMsg(MSG_NOTIFY_KEY_MSG, key);
            }
        }
    }
}

void MainWindow::sendMsg(const int &, const QString &)
{

}

void MainWindow::notifyMsg(const int &msgType, const QString &msgContent)
{
    if (m_pNotifySubject) {
        m_pNotifySubject->notifyMsg(msgType, msgContent);
    }
}

//  窗口显示默认大小
void MainWindow::showDefaultSize()
{
    int nWidth = AppSetting::instance()->getAppKeyValue(KEY_APP_WIDTH).toInt();
    int nHeight = AppSetting::instance()->getAppKeyValue(KEY_APP_HEIGHT).toInt();

    if (nWidth == 0 || nHeight == 0) {
        resize(1000, 680);

        AppSetting::instance()->setAppKeyValue(KEY_APP_WIDTH, "1000");
        AppSetting::instance()->setAppKeyValue(KEY_APP_HEIGHT, "680");
    } else {
        resize(nWidth, nHeight);
    }
}


//  初始化 快捷键操作
void MainWindow::initShortCut()
{
    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(slotShortCut(const QString &)));

    auto keyList = DataManager::instance()->getPKeyList();
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

    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk(msgContent);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_f11 && DataManager::instance()->CurShowState() != FILE_SLIDE) {
            if (DataManager::instance()->CurShowState() == FILE_FULLSCREEN)
                emit sigAppShowState(1);
            else {
                emit sigFullScreen();
            }
        } else if (msgContent == KeyStr::g_esc) {        //  退出全屏模式
            emit sigAppShowState(1);
        } else if (msgContent == KeyStr::g_space) {
            if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
                emit sigSpacePressed();
            }
        }
    }
    return 0;
}
