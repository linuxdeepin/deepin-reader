#include "MainWindow.h"

#include "TitleWidget.h"
#include "MainWidget.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <QDebug>
#include <QDesktopServices>
#include <DMessageBox>
#include <QSignalMapper>
#include "controller/DataManager.h"

MainWindow::MainWindow(DMainWindow *parent)
    : DMainWindow(parent)
{
    initUI();

    initTitlebar();

    initConnections();

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }

    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }

    setMinimumSize(1000, 680);
    setObserverName();

    //  在屏幕中心显示
    Dtk::Widget::moveToCenter(this);
}

MainWindow::~MainWindow()
{
    // We don't need clean pointers because application has exit here.
    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
        m_pMsgSubject->stopThreadRun();
    }

    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
        m_pNotifySubject->stopThreadRun();
    }
}

//  窗口关闭
void MainWindow::closeEvent(QCloseEvent *event)
{
    SlotAppExit();

    DMainWindow::closeEvent(event);
}

void MainWindow::initUI()
{
    setCentralWidget(new MainWidget);

    titlebar()->addWidget(new TitleWidget, Qt::AlignLeft);
}

void MainWindow::initConnections()
{
    connect(this, SIGNAL(sigAppExit()), this, SLOT(SlotAppExit()));

    DMenu *m_menu = new DMenu(this);

    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(SlotActionTrigger(const QString &)));

    QStringList firstActionList = QStringList() << Frame::sOpenFile << Frame::sSaveFile << Frame::sSaveAsFile
                                  << Frame::sOpenFolder << Frame::sPrint << Frame::sFileAttr;

    foreach (const QString &s, firstActionList) {
        QAction *_action = createAction(m_menu, s);
        connect(_action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(_action, s);
    }
    m_menu->addSeparator();

    QStringList secondActionList = QStringList() << Frame::sSearch << Frame::sFullScreen << Frame::sScreening
                                   << Frame::sLarger << Frame::sSmaller;
    foreach (const QString &s, secondActionList) {
        QAction *_action = createAction(m_menu, s);
        connect(_action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(_action, s);
    }
    m_menu->addSeparator();

    m_menu->setMinimumWidth(ConstantMsg::g_menu_width);
    titlebar()->setMenu(m_menu);

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (a->text() == Frame::sOpenFile) {
            a->setDisabled(false);
            break;
        }
    }
}

void MainWindow::initTitlebar()
{
    titlebar()->setIcon(QIcon(":/resources/image/logo/logo_big.svg"));
    titlebar()->setTitle("");
}

//  打开 所在文件夹
void MainWindow::onOpenFolder()
{
    QString strFilePath = DataManager::instance()->strOnlyFilePath();
    if (strFilePath != "") {
        int nLastPos = strFilePath.lastIndexOf('/');
        strFilePath = strFilePath.mid(0, nLastPos);
        QDesktopServices::openUrl(QUrl(strFilePath));
    }
}

//  全屏
void MainWindow::onFullScreen()
{
    titlebar()->setVisible(false);
    this->setWindowState(Qt::WindowFullScreen);
    DataManager::instance()->setCurShowState(FILE_FULLSCREEN);  //  全屏状态
    sendMsg(MSG_OPERATION_FULLSCREEN);
}

//  放映
void MainWindow::onScreening()
{
    titlebar()->setVisible(false);
    this->setWindowState(Qt::WindowFullScreen);
    sendMsg(MSG_OPERATION_SLIDE);
}

//  退出 应用
void MainWindow::SlotAppExit()
{
    QString sFilePath = DataManager::instance()->strOnlyFilePath();
    if (sFilePath != "") {
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            if (QMessageBox::Yes == DMessageBox::question(nullptr, Frame::sSaveFile, Frame::sSaveFileTitle)) {
                DocummentProxy::instance()->save(sFilePath, true);
            }
        }
        sendMsg(MSG_CLOSE_FILE);
        DocummentProxy::instance()->closeFile();
    }
    dApp->exit();
}

//  文件打开成，　功能性　菜单才能点击
void MainWindow::openFileOk()
{
    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setDisabled(false);
    }
}

//  点击菜单　发送指令
void MainWindow::SlotActionTrigger(const QString &sAction)
{
    if (sAction == Frame::sOpenFile) {
        sendMsg(MSG_OPERATION_OPEN_FILE);
    } else if (sAction == Frame::sSaveFile) {
        sendMsg(MSG_OPERATION_SAVE_FILE);
    } else if (sAction == Frame::sSaveAsFile) {
        sendMsg(MSG_OPERATION_SAVE_AS_FILE);
    } else if (sAction == Frame::sOpenFolder) {
        onOpenFolder();
    } else if (sAction == Frame::sPrint) {
        sendMsg(MSG_OPERATION_PRINT);
    } else if (sAction == Frame::sFileAttr) {
        sendMsg(MSG_OPERATION_ATTR);
    } else if (sAction == Frame::sSearch) {
        sendMsg(MSG_OPERATION_FIND);
    } else if (sAction == Frame::sFullScreen) {
        onFullScreen();
    } else if (sAction == Frame::sScreening) {
        onScreening();
    } else if (sAction == Frame::sLarger) {
        sendMsg(MSG_OPERATION_LARGER);
    } else if (sAction == Frame::sSmaller) {
        sendMsg(MSG_OPERATION_SMALLER);
    }
}

void MainWindow::sendMsg(const int &msgType, const QString &msgContent)
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(this, msgType, msgContent);
    }
}

int MainWindow::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        openFileOk();
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == "Esc") {  //  退出全屏模式
            titlebar()->setVisible(true);
            this->setWindowState(Qt::WindowNoState);
        } else if (msgContent == "F1") {    //  显示帮助文档
            QDesktopServices::openUrl(QUrl(Constant::sAcknowledgementLink));
            return ConstantMsg::g_effective_res;
        }
    } else if (msgType == MSG_OPERATION_OPEN_FILE_TITLE) {
        titlebar()->setTitle(msgContent);
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_OPERATION_EXIT) {
        emit sigAppExit();
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

void MainWindow::setObserverName()
{
    m_strObserverName = "MainWindow";
}

QAction *MainWindow::createAction(DMenu *menu, const QString &actionName)
{
    QAction *action = new QAction(actionName, this);
    action->setDisabled(true);
    menu->addAction(action);

    return action;
}
