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

    setMinimumSize(500, 500);

    Dtk::Widget::moveToCenter(this);
}

MainWindow::~MainWindow()
{
    // We don't need clean pointers because application has exit here.
    m_pMsgSubject->stopThreadRun();
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

    createAction(m_menu, tr("Open file"), SLOT(action_OpenFile()));
    createAction(m_menu, tr("Save"), SLOT(action_SaveFile()));
    createAction(m_menu, tr("Save as"), SLOT(action_SaveAsFile()));
    createAction(m_menu, tr("Open folder"), SLOT(action_OpenFolder()));
    createAction(m_menu, tr("Print"), SLOT(action_Print()));
    createAction(m_menu, tr("Attr"), SLOT(action_Attr()));
    m_menu->addSeparator();

    createAction(m_menu, tr("Find"), SLOT(action_Find()));
    createAction(m_menu, tr("Full screen"), SLOT(action_FullScreen()));
    createAction(m_menu, tr("Screening"), SLOT(action_Screening()));
    createAction(m_menu, tr("Larger"), SLOT(action_Larger()));
    createAction(m_menu, tr("Smaller"), SLOT(action_Smaller()));

//    DMenu *themeMenu = new DMenu(tr("switch theme"));
//    createAction(themeMenu, tr("dark theme"), SLOT(action_darkTheme()));
//    createAction(themeMenu, tr("light theme"), SLOT(action_lightTheme()));
//    m_menu->addMenu(themeMenu);

    m_menu->addSeparator();

    createAction(m_menu, tr("Help"), SLOT(action_Help()));

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

//void MainWindow::action_darkTheme()
//{
//    dApp->viewerTheme->setCurrentTheme(Dark);
//}

//void MainWindow::action_lightTheme()
//{
//    dApp->viewerTheme->setCurrentTheme(Light);
//}

//  打开 帮助文档
void MainWindow::action_Help()
{
    sendMsg(MSG_OPERATION_HELP);
}

void MainWindow::sendMsg(const int &msgType, const QString &msgContent)
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(nullptr, msgType, msgContent);
    }
}

void MainWindow::createAction(DMenu *menu, const QString &actionName, const char *member)
{
    QAction *action = new QAction(actionName, this);
    connect(action, SIGNAL(triggered()), member);

    menu->addAction(action);
}
