#include "window.h"

#include <DWidget>
#include <DPushButton>
#include <DSettingsWidgetFactory>
#include <DSettingsGroup>
#include <DSettings>
#include <DSettingsOption>
#include <DTitlebar>
#include <DAnchors>

#include <QApplication>
#include <QScreen>
#include <QStyleFactory>
#include <QEvent>

#include <QGuiApplication>
#include <QWindow>

#include <DThemeManager>
#include "toolbar.h"
#include "utils.h"

#ifdef DTKWIDGET_CLASS_DFileDialog
#include <DFileDialog>
#else
#include <QFileDialog>
#endif



Window::Window(DMainWindow *parent)
    : DMainWindow(parent),
      m_centralLayout(new QVBoxLayout(m_centralWidget)),
      m_menu(new QMenu)
{
    initUI();

    // Init layoutr
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    // Init titlebar.
    if (titlebar()) {
        titlebar()->setIcon(QIcon(":/images/logo_24.svg"));
        initTitlebar();
    }

    initConnections();
}

Window::~Window()
{
    // We don't need clean pointers because application has exit here.
}

void Window::initUI()
{
    m_centralWidget = new MainWidget();
    connect(this, SIGNAL(sigOpenFile()), m_centralWidget, SIGNAL(sigHomeOpenFile()));

    setCentralWidget(m_centralWidget);

    m_titleWidget = new TitleWidget();
    connect(m_titleWidget, SIGNAL(sendThumbnailState(const bool&)), m_centralWidget, SLOT(setSliderState(const bool&)));

    titlebar()->addWidget(m_titleWidget, Qt::AlignLeft);
}

void Window::initConnections()
{
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
    titlebar()->setTitle(tr(""));
}

//  打开 文件
void Window::action_OpenFile()
{
    emit sigOpenFile();
}

//  保存文件
void Window::action_SaveFile()
{
    qDebug() << "action_SaveFile";
}

//  另存为
void Window::action_SaveAsFile()
{
    qDebug() << "action_SaveAsFile";
}

//  打开 所在文件夹
void Window::action_OpenFolder()
{
    qDebug() << "action_OpenFolder";
}

//  打印
void Window::action_Print()
{
    qDebug() << "action_Print";
}

//  属性
void Window::action_Attr()
{
    qDebug() << "action_Attr";
}

//  搜索
void Window::action_Find()
{
    qDebug() << "action_Find";
}

//  全屏
void Window::action_FullScreen()
{
    qDebug() << "action_FullScreen";
}

//  放映
void Window::action_Screening()
{
    qDebug() << "action_Screening";
}

//  放大
void Window::action_Larger()
{
    qDebug() << "action_Larger";
}

//  缩小
void Window::action_Smaller()
{
    qDebug() << "action_Smaller";
}

//  主题切换
void Window::action_SwitchTheme()
{
    qDebug() << "action_SwitchTheme";
}

//  帮助
void Window::action_Help()
{
    qDebug() << "action_Help";
}

void Window::createAction(const QString& actionName, const QString& objectName, const QString& iconName, const char* member)
{
    QAction *action = new QAction(actionName, this);

    action->setObjectName(objectName);

    QIcon icon = Utils::getActionIcon(iconName);
    action->setIcon(icon);

    connect(action, SIGNAL(triggered()), member);

    addAction(action);
    m_menu->addAction(action);
}
