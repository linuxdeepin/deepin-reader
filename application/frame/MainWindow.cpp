#include "MainWindow.h"

#include "TitleWidget.h"
#include "MainWidget.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <QDebug>

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

    setMinimumSize(720, 560);

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

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    QString key = Utils::getKeyshortcut(e);
    qDebug() << "Window::keyPressEvent      "   << key;
    if ( key == "Esc") {
        sendMsg(MSG_MAGNIFYING_CANCEL);
//        qDebug() << "keyPressEvent";
    } else if (key == "F1") {   //  F1　跳转　帮助文档
//        qDebug() << "keyPressEvent";
    }
    DMainWindow::keyPressEvent(e);
}

void MainWindow::initUI()
{
    MainWidget *pMainWidget = new MainWidget;
    setCentralWidget(pMainWidget);

    TitleWidget *titleWidget = new TitleWidget();
    titlebar()->addWidget(titleWidget, Qt::AlignLeft);
}

void MainWindow::initConnections()
{
    m_menu = new DMenu;

    createAction(m_menu, tr("Open file"), SLOT(action_OpenFile()), false);
    m_pSaveFile = createAction(m_menu, tr("Save"), SLOT(action_SaveFile()));
    m_pSaveAsFile = createAction(m_menu, tr("Save as"), SLOT(action_SaveAsFile()));
    m_pOpenFolder = createAction(m_menu, tr("Open folder"), SLOT(action_OpenFolder()));
    m_pFilePrint = createAction(m_menu, tr("Print"), SLOT(action_Print()));
    m_pFileAttr = createAction(m_menu, tr("Attr"), SLOT(action_Attr()));
    m_menu->addSeparator();

    m_pFileFind = createAction(m_menu, tr("Find"), SLOT(action_Find()));
    m_pFileFullScreen = createAction(m_menu, tr("Full screen"), SLOT(action_FullScreen()));
    m_pFileScreening = createAction(m_menu, tr("Screening"), SLOT(action_Screening()));
    m_pFileLarger = createAction(m_menu, tr("Larger"), SLOT(action_Larger()));
    m_pFileSmaller = createAction(m_menu, tr("Smaller"), SLOT(action_Smaller()));

    m_menu->addSeparator();

    m_menu->setMinimumWidth(ConstantMsg::g_menu_width);
    titlebar()->setMenu(m_menu);
}

void MainWindow::initTitlebar()
{
    titlebar()->setIcon(QIcon(":/resources/image/logo/logo_big.svg"));
    titlebar()->setTitle(tr(""));
}

//  打开 文件
void MainWindow::action_OpenFile()
{
    sendMsg(MSG_OPERATION_OPEN_FILE);
}

//  保存文件
void MainWindow::action_SaveFile()
{
    sendMsg(MSG_OPERATION_SAVE_FILE);
}

//  另存为
void MainWindow::action_SaveAsFile()
{
    sendMsg(MSG_OPERATION_SAVE_AS_FILE);
}

//  打开 所在文件夹
void MainWindow::action_OpenFolder()
{
    sendMsg(MSG_OPERATION_OPEN_FOLDER);
}

//  打印
void MainWindow::action_Print()
{
    sendMsg(MSG_OPERATION_PRINT);
}

//  属性
void MainWindow::action_Attr()
{
    sendMsg(MSG_OPERATION_ATTR);
}

//  搜索
void MainWindow::action_Find()
{
    sendMsg(MSG_OPERATION_FIND);
}

//  全屏
void MainWindow::action_FullScreen()
{
    sendMsg(MSG_OPERATION_FULLSCREEN);
}

//  放映
void MainWindow::action_Screening()
{
    sendMsg(MSG_OPERATION_SCREENING);
}

//  放大
void MainWindow::action_Larger()
{
    sendMsg(MSG_OPERATION_LARGER);
}

//  缩小
void MainWindow::action_Smaller()
{
    sendMsg(MSG_OPERATION_SMALLER);
}

//  文件打开成，　功能性　菜单才能点击
void MainWindow::openFileOk()
{
    m_pSaveFile->setDisabled(false);
    m_pSaveAsFile->setDisabled(false);
    m_pOpenFolder->setDisabled(false);;
    m_pFilePrint->setDisabled(false);;
    m_pFileAttr->setDisabled(false);
    m_pFileFind->setDisabled(false);
    m_pFileFullScreen->setDisabled(false);
    m_pFileScreening->setDisabled(false);
    m_pFileLarger->setDisabled(false);
    m_pFileSmaller->setDisabled(false);
}

void MainWindow::sendMsg(const int &msgType, const QString &msgContent)
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(this, msgType, msgContent);
    }
}

int MainWindow::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        openFileOk();
    }
    return 0;
}

void MainWindow::setObserverName(const QString &name)
{
    m_strObserverName = name;
}

QAction *MainWindow::createAction(DMenu *menu, const QString &actionName, const char *member, const bool &disable)
{
    QAction *action = new QAction(actionName, this);
    connect(action, SIGNAL(triggered()), member);
    action->setDisabled(disable);
    menu->addAction(action);

    return action;
}
