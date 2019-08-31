#include "window.h"

#include <DWidget>
#include <DPushButton>
#include <DTitlebar>
#include <DWidgetUtil>
#include <QStyleFactory>

#include <DThemeManager>
#include "utils.h"

#ifdef DTKWIDGET_CLASS_DFileDialog
#include <DFileDialog>
#else
#include <QFileDialog>
#endif

Window::Window(DMainWindow *parent)
    : DMainWindow(parent),
      m_centralLayout(new QVBoxLayout(m_centralWidget))
{
    initUI();

    // Init layoutr
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    // Init titlebar.
    initTitlebar();

    initConnections();

    m_pMsgSubject = MsgSubject::getInstance();

    Dtk::Widget::moveToCenter(this);
}

Window::~Window()
{
    // We don't need clean pointers because application has exit here.
}

void Window::initUI()
{
    m_centralWidget = new MainWidget();
    setCentralWidget(m_centralWidget);

    m_titleWidget = new TitleWidget();
    titlebar()->addWidget(m_titleWidget, Qt::AlignLeft);
}

void Window::initConnections()
{
    m_menu = new DMenu;

    createAction(tr("Open file"), tr("Open"), tr(""), SLOT(action_OpenFile()));
    createAction(tr("Save"), tr("Save"), tr(""), SLOT(action_SaveFile()));
    createAction(tr("Save as"), tr("Save as"), tr(""), SLOT(action_SaveAsFile()));
    createAction(tr("Open folder"), tr("Open folder"), tr(""), SLOT(action_OpenFolder()));
    createAction(tr("Print"), tr("Print"), tr(""), SLOT(action_Print()));
    createAction(tr("Attr"), tr("Attr"), tr(""), SLOT(action_Attr()));
    m_menu->addSeparator();

    createAction(tr("Find"), tr("Find"), tr(""), SLOT(action_Find()));
    createAction(tr("Full screen"), tr("Full"), tr(""), SLOT(action_FullScreen()));
    createAction(tr("Screening"), tr("Screening"), tr(""), SLOT(action_Screening()));
    createAction(tr("Larger"), tr("Larger"), tr(""), SLOT(action_Larger()));
    createAction(tr("Smaller"), tr("Smaller"), tr(""), SLOT(action_Smaller()));
    createAction(tr("Switch theme"), tr("Switch theme"), tr(""), SLOT(action_SwitchTheme()));
    m_menu->addSeparator();

    createAction(tr("Help"), tr("Help"), tr(""), SLOT(action_Help()));

    m_menu->setStyle(QStyleFactory::create("dlight"));
    m_menu->setMinimumWidth(150);
    titlebar()->setMenu(m_menu);
}

void Window::initTitlebar()
{
    if (titlebar()) {
        titlebar()->setIcon(QIcon(":/images/logo_24.svg"));
        titlebar()->setTitle(tr(""));
    }
}

//  打开 文件
void Window::action_OpenFile()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_OPEN_FILE);
    }
}

//  保存文件
void Window::action_SaveFile()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_SAVE_FILE);
    }
}

//  另存为
void Window::action_SaveAsFile()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_SAVE_AS_FILE);
    }
}

//  打开 所在文件夹
void Window::action_OpenFolder()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_OPEN_FOLDER);
    }
}

//  打印
void Window::action_Print()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_PRINT);
    }
}

//  属性
void Window::action_Attr()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_ATTR);
    }
}

//  搜索
void Window::action_Find()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_FIND);
    }
}

//  全屏
void Window::action_FullScreen()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_FULLSCREEN);
    }
}

//  放映
void Window::action_Screening()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_SCREENING);
    }
}

//  放大
void Window::action_Larger()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_LARGER);
    }
}

//  缩小
void Window::action_Smaller()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_SMALLER);
    }
}

//  主题切换
void Window::action_SwitchTheme()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_OPERATION_SWITCH_THEME);
    }
}

//  打开 帮助文档
void Window::action_Help()
{

}

void Window::createAction(const QString &actionName, const QString &objectName, const QString &iconName, const char *member)
{
    QAction *action = new QAction(actionName, this);

    action->setObjectName(objectName);

    QIcon icon = Utils::getActionIcon(iconName);
    action->setIcon(icon);

    connect(action, SIGNAL(triggered()), member);

    addAction(action);
    m_menu->addAction(action);
}
