#include "MainWindow.h"
#include "TitleWidget.h"
#include "MainWidget.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <QDebug>
#include <QDesktopServices>
#include <DMessageBox>
#include "controller/DataManager.h"
#include <DGuiApplicationHelper>
#include "DocummentFileHelper.h"

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(DMainWindow *parent)
    : DMainWindow(parent)
{
    m_strObserverName = "MainWindow";

    setCurTheme();

    initUI();

    initTitlebar();

    initConnections();

    initThemeChanged();

    m_pFilterList = QStringList() << KeyStr::g_esc << KeyStr::g_f1 << KeyStr::g_f11 << KeyStr::g_up
                    << KeyStr::g_down << KeyStr::g_del << KeyStr::g_pgup << KeyStr::g_pgdown
                    << KeyStr::g_ctrl_a << KeyStr::g_ctrl_c << KeyStr::g_ctrl_f << KeyStr::g_ctrl_o
                    << KeyStr::g_ctrl_p << KeyStr::g_ctrl_s << KeyStr::g_ctrl_v << KeyStr::g_ctrl_x
                    << KeyStr::g_ctrl_z
                    << KeyStr::g_ctrl_alt_f << KeyStr::g_ctrl_shift_slash << KeyStr::g_ctrl_shift_s;

    installEventFilter(this);

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }

    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }

    setMinimumSize(1000, 680);

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
    sendMsg(MSG_OPEN_FILE_PATH, filepath);
}

void MainWindow::setSreenRect(const QRect &rect)
{
    DataManager::instance()->setScreenRect(rect);
}

//  窗口关闭
void MainWindow::closeEvent(QCloseEvent *event)
{
    slotAppExit();

    DMainWindow::closeEvent(event);
}

//  事件 过滤器
bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    int nType = e->type();
    if (nType == QEvent::KeyPress) {    //  按下
        QKeyEvent *event = static_cast<QKeyEvent *>(e);
        QString key = Utils::getKeyshortcut(event);
        if (m_pFilterList.contains(key)) {

            //  没有打开文件, 只能 执行 ctrl+o, f1
            QString sFilePath = DataManager::instance()->strOnlyFilePath();
            if (sFilePath == "") {
                if (key == KeyStr::g_f1 || key == KeyStr::g_ctrl_o
                        || key == KeyStr::g_ctrl_alt_f || key == KeyStr::g_ctrl_shift_slash) {
                    sendMsg(MSG_NOTIFY_KEY_MSG, key);
                }
            } else {
                sendMsg(MSG_NOTIFY_KEY_MSG, key);
            }
            return true;
        }
    }
    return DMainWindow::eventFilter(obj, e);
}

void MainWindow::initUI()
{
    titlebar()->addWidget(new TitleWidget, Qt::AlignLeft);

    setCentralWidget(new MainWidget);
}

void MainWindow::createActionMap(DMenu *m_menu, QSignalMapper *pSigManager,
                                 const QStringList &actionList, const QStringList &actionObjList)
{
    int nFirstSize = actionList.size();
    for ( int iLoop = 0; iLoop < nFirstSize; iLoop++) {
        QString sActionName = actionList.at(iLoop);
        QString sObjName = actionObjList.at(iLoop);

        QAction *_action = createAction(m_menu, sActionName, sObjName);
        connect(_action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(_action, sObjName);
    }
}

void MainWindow::initConnections()
{
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigOpenAppHelp()), this, SLOT(slotOpenAppHelp()));
    connect(this, SIGNAL(sigAppExit()), this, SLOT(slotAppExit()));
    connect(this, SIGNAL(sigAppShowState(const int &)), this, SLOT(slotAppShowState(const int &)));
    connect(this, SIGNAL(sigSetAppTitle(const QString &)), this, SLOT(slotSetAppTitle(const QString &)));

    auto m_menu = new DMenu(this);
    //m_menu->setFixedWidth();
    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(slotActionTrigger(const QString &)));

    QStringList firstActionList = QStringList() << tr("Open File") << tr("Save File") << tr("Save As File")
                                  << tr("Open Folder") << tr("Print") << tr("File Attr");
    QStringList firstActionObjList = QStringList() << "Open File" << "Save File" << "Save As File"
                                     << "Open Folder" << "Print" << "File Attr";

    createActionMap(m_menu, pSigManager, firstActionList, firstActionObjList);
    m_menu->addSeparator();

    QStringList secondActionList = QStringList() << tr("Search") << tr("Full Screen") << tr("Screening")
                                   << tr("Larger") << tr("Smaller");
    QStringList secondActionObjList = QStringList() << "Search" << "Full Screen" << "Screening"
                                      << "Larger" << "Smaller";

    createActionMap(m_menu, pSigManager, secondActionList, secondActionObjList);
    m_menu->addSeparator();

    titlebar()->setMenu(m_menu);

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (a->objectName() == "Open File") {
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
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ] () {
        setCurTheme();
        sendMsg(MSG_OPERATION_UPDATE_THEME);
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

//  退出 应用
void MainWindow::slotAppExit()
{
    QString sFilePath = DataManager::instance()->strOnlyFilePath();
    if (sFilePath != "") {
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            if (QMessageBox::Yes == DMessageBox::question(nullptr, tr("Save File"), tr("Do you need to save the file opened?"))) {
                DocummentFileHelper::instance()->save(sFilePath, true);

                //  保存 书签数据
                dApp->dbM->saveBookMark();
            }
        }
        sendMsg(MSG_CLOSE_FILE);
        DocummentFileHelper::instance()->closeFile();
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
    if (sAction == "Open File") {
        sendMsg(MSG_OPERATION_OPEN_FILE);
    } else if (sAction == "Save File") {
        sendMsg(MSG_OPERATION_SAVE_FILE);
    } else if (sAction == "Save As File") {
        sendMsg(MSG_OPERATION_SAVE_AS_FILE);
    } else if (sAction == "Open Folder") {
        onOpenFolder();
    } else if (sAction == "Print") {
        sendMsg(MSG_OPERATION_PRINT);
    } else if (sAction == "File Attr") {
        sendMsg(MSG_OPERATION_ATTR);
    } else if (sAction == "Search") {
        sendMsg(MSG_OPERATION_FIND);
    } else if (sAction == "Full Screen") {
        onFullScreen();
    } else if (sAction == "Screening") {
        onScreening();
    } else if (sAction == "Larger") {
        sendMsg(MSG_OPERATION_LARGER);
    } else if (sAction == "Smaller") {
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
    }  else if (msgType == MSG_OPERATION_OPEN_FILE_TITLE) {
        emit sigSetAppTitle(msgContent);
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_OPERATION_EXIT) {
        emit sigAppExit();
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_esc) {          //  退出全屏模式
            emit sigAppShowState(1);
        } else if (msgContent == KeyStr::g_f1) {    //  显示帮助文档
            emit sigOpenAppHelp();
            return ConstantMsg::g_effective_res;
        } else if (msgContent == KeyStr::g_ctrl_alt_f) {    //  窗口大小切换
            return ConstantMsg::g_effective_res;
        } else if (msgContent == KeyStr::g_ctrl_shift_slash) {    //  显示快捷键预览
            return ConstantMsg::g_effective_res;
        }
    }
    return 0;
}

QAction *MainWindow::createAction(DMenu *menu, const QString &actionName, const QString &objName)
{
    QAction *action = new QAction(actionName, this);
    action->setObjectName(objName);
    action->setDisabled(true);
    menu->addAction(action);

    return action;
}
