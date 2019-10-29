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

#include <DGuiApplicationHelper>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(DMainWindow *parent)
    : DMainWindow(parent)
{
    initUI();

    initTitlebar();

    initConnections();

    initThemeChanged();

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

void MainWindow::openfile(const QString &filepath)
{
   // sendMsg(MSG_OPERATION_OPEN_FILE,filepath);
    sendMsg(MSG_OPEN_FILE_PATH, filepath);
}

//  窗口关闭
void MainWindow::closeEvent(QCloseEvent *event)
{
    slotAppExit();

    DMainWindow::closeEvent(event);
}

void MainWindow::initUI()
{
    titlebar()->addWidget(new TitleWidget, Qt::AlignLeft);

    setCentralWidget(new MainWidget);
}

void MainWindow::initConnections()
{
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigOpenAppHelp()), this, SLOT(slotOpenAppHelp()));
    connect(this, SIGNAL(sigAppExit()), this, SLOT(slotAppExit()));
    connect(this, SIGNAL(sigAppShowState(const int &)), this, SLOT(slotAppShowState(const int &)));
    connect(this, SIGNAL(sigSetAppTitle(const QString &)), this, SLOT(slotSetAppTitle(const QString &)));

    DMenu *m_menu = new DMenu(this);

    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(slotActionTrigger(const QString &)));

    QStringList firstActionList = QStringList() << tr("Open File") << tr("Save File") << tr("Save As File")
                                  << tr("Open Folder") << tr("Print") << tr("File Attr");

    foreach (const QString &s, firstActionList) {
        QAction *_action = createAction(m_menu, s);
        connect(_action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(_action, s);
    }
    m_menu->addSeparator();

    QStringList secondActionList = QStringList() << tr("Search") << tr("Full Screen") << tr("Screening")
                                   << tr("Larger") << tr("Smaller");

    foreach (const QString &s, secondActionList) {
        QAction *_action = createAction(m_menu, s);
        connect(_action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(_action, s);
    }
    m_menu->addSeparator();

    titlebar()->setMenu(m_menu);

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (a->text() == tr("Open File")) {
            a->setDisabled(false);
            break;
        }
    }
}

void MainWindow::initTitlebar()
{
    titlebar()->setIcon(QIcon(Constant::sLogoPath));
    titlebar()->setTitle("");
}

//  打开 所在文件夹
void MainWindow::onOpenFolder()
{
    QString strFilePath = DataManager::instance()->strOnlyFilePath();
    if (strFilePath != "") {
        int nLastPos = strFilePath.lastIndexOf('/');
        strFilePath = strFilePath.mid(0, nLastPos);
        strFilePath = QString("file://") + strFilePath;
        QDesktopServices::openUrl(QUrl(strFilePath));
    }
}

//  全屏
void MainWindow::onFullScreen()
{
    slotAppShowState(0);
    DataManager::instance()->setCurShowState(FILE_FULLSCREEN);  //  全屏状态
    sendMsg(MSG_OPERATION_FULLSCREEN);
}

//  放映
void MainWindow::onScreening()
{
    slotAppShowState(0);
    sendMsg(MSG_OPERATION_SLIDE);
}

void MainWindow::initThemeChanged()
{
    //  主题变了
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ] (DGuiApplicationHelper::ColorType colorType) {
        QString sTheme = "";
        if (colorType == DGuiApplicationHelper::UnknownType) {  //  未知
            sTheme = "0";
        } else if (colorType == DGuiApplicationHelper::LightType) { //  浅色
            sTheme = "1";
        } else if (colorType == DGuiApplicationHelper::DarkType) {  //  深色
            sTheme = "2";
        }

        sendMsg(MSG_OPERATION_UPDATE_THEME, sTheme);
    });
}

//  退出 应用
void MainWindow::slotAppExit()
{
    QString sFilePath = DataManager::instance()->strOnlyFilePath();
    if (sFilePath != "") {
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            if (QMessageBox::Yes == DMessageBox::question(nullptr, tr("Save File"), tr("Do you need to save the file opened?"))) {
                DocummentProxy::instance()->save(sFilePath, true);
            }
        }
        sendMsg(MSG_CLOSE_FILE);
        DocummentProxy::instance()->closeFile();
    }
    dApp->exit();
}

/**
 * @brief MainWindow::slotAppShowState
 * @param nState 1 退出全屏, 0 全屏显示
 */
void MainWindow::slotAppShowState(const int &nState)
{
    titlebar()->setVisible(nState);
    if (nState == 1) {
        if (windowState() == Qt::WindowFullScreen) {
            showNormal();
            this->setWindowState(Qt::WindowMaximized);
        }

    } else {
        this->setWindowState(Qt::WindowFullScreen);
    }
}

void MainWindow::slotSetAppTitle(const QString &sData)
{
    titlebar()->setTitle(sData);
}

//  文件打开成，　功能性　菜单才能点击
void MainWindow::slotOpenFileOk()
{
    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setDisabled(false);
    }
}

//  打开帮助文档
void MainWindow::slotOpenAppHelp()
{
    QDesktopServices::openUrl(QUrl(Constant::sAcknowledgementLink));
}

//  点击菜单　发送指令
void MainWindow::slotActionTrigger(const QString &sAction)
{
    if (sAction == tr("Open File")) {
        sendMsg(MSG_OPERATION_OPEN_FILE);
    } else if (sAction == tr("Save File")) {
        sendMsg(MSG_OPERATION_SAVE_FILE);
    } else if (sAction == tr("Save As File")) {
        sendMsg(MSG_OPERATION_SAVE_AS_FILE);
    } else if (sAction == tr("Open Folder")) {
        onOpenFolder();
    } else if (sAction == tr("Print")) {
        sendMsg(MSG_OPERATION_PRINT);
    } else if (sAction == tr("File Attr")) {
        sendMsg(MSG_OPERATION_ATTR);
    } else if (sAction == tr("Search")) {
        sendMsg(MSG_OPERATION_FIND);
    } else if (sAction == tr("Full Screen")) {
        onFullScreen();
    } else if (sAction == tr("Screening")) {
        onScreening();
    } else if (sAction == tr("Larger")) {
        sendMsg(MSG_OPERATION_LARGER);
    } else if (sAction == tr("Smaller")) {
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
        emit sigOpenFileOk();
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == "Esc") {  //  退出全屏模式
            emit sigAppShowState(1);
        } else if (msgContent == "F1") {    //  显示帮助文档
            emit sigOpenAppHelp();
            return ConstantMsg::g_effective_res;
        }
    } else if (msgType == MSG_OPERATION_OPEN_FILE_TITLE) {
        emit sigSetAppTitle(msgContent);
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
