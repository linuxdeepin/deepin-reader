#include "window.h"
#include "toolbar.h"
#include "danchors.h"
#include "dthememanager.h"
#include "utils.h"

#include <DSettingsWidgetFactory>
#include <DSettingsGroup>
#include <DSettings>
#include <DSettingsOption>
#include <DTitlebar>
#include <DAnchors>

#include <QApplication>
//#include <QPrintDialog>
//#include <QPrintPreviewDialog>
//#include <QPrinter>
#include <QScreen>
#include <QStyleFactory>
#include <QEvent>

#include <QGuiApplication>
#include <QWindow>

#ifdef DTKWIDGET_CLASS_DFileDialog
#include <DFileDialog>
#else
#include <QFileDialog>
#endif



Window::Window(DMainWindow *parent)
    : DMainWindow(parent),
      m_centralWidget(new QWidget),
      m_centralLayout(new QVBoxLayout(m_centralWidget)),
      m_tabbar(new Tabbar),
      m_menu(new QMenu)
{

    // Init layoutr
   m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

//    m_centralLayout->addWidget(m_editorWidget);
//    //setWindowIcon(QIcon::fromTheme("deepin-editor"));
 //  setWindowIcon(QIcon(":/images/logo_24.svg"));
//    setCentralWidget(m_centralWidget);
    // Init titlebar.
    if (titlebar()) {
        titlebar()->setIcon(QIcon(":/images/logo_24.svg"));
        initTitlebar();
    }


}

Window::~Window()
{
    // We don't need clean pointers because application has exit here.
}


void Window::initTitlebar()
{
    QAction *newWindowAction(new QAction(tr("New window"), this));  
    QAction *openFileAction(new QAction(tr("Open file"), this));
    QAction *saveAction(new QAction(tr("Save"), this));
    QAction *saveAsAction(new QAction(tr("Save as"), this));
    QAction *printAction(new QAction(tr("Print"), this));
    QAction *switchThemeAction(new QAction(tr("Switch theme"), this));
    QAction *settingAction(new QAction(tr("Settings"), this));
    QAction *findAction(new QAction(QApplication::translate("DTextEdit", "Find"), this));
    QAction *replaceAction(new QAction(QApplication::translate("DTextEdit", "Replace"), this));

    m_menu->addAction(newWindowAction);  
    m_menu->addAction(openFileAction);
    m_menu->addSeparator();
    m_menu->addAction(findAction);
    m_menu->addAction(replaceAction);
    m_menu->addAction(saveAction);
    m_menu->addAction(saveAsAction);
    m_menu->addAction(printAction);
    m_menu->addAction(switchThemeAction);
    m_menu->addSeparator();

    m_menu->setStyle(QStyleFactory::create("dlight"));
    m_menu->setMinimumWidth(150);
    titlebar()->setMenu(m_menu);
//    ToolBar *toolBar = new ToolBar;
//    //toolBar->setTabbar(m_tabbar);r

//    titlebar()->setCustomWidget(toolBar, Qt::AlignVCenter, false);
//    titlebar()->setAutoHideOnFullscreen(true);
//    titlebar()->setSeparatorVisible(true);
//    titlebar()->setFixedHeight(40);
//    titlebar()->setMenu(m_menu);

//    connect(m_tabbar, &DTabBar::tabBarDoubleClicked, titlebar(), &DTitlebar::doubleClicked, Qt::QueuedConnection);

//    connect(m_tabbar, &Tabbar::closeTabs, this, &Window::handleTabsClosed, Qt::QueuedConnection);
//    connect(m_tabbar, &Tabbar::requestHistorySaved, this, [=] (const QString &filePath) {
//        if (QFileInfo(filePath).dir().absolutePath() == m_blankFileDir) {
//            return;
//        }

//        if (!m_closeFileHistory.contains(filePath)) {
//            m_closeFileHistory << filePath;
//        }
//    });

//    connect(m_tabbar, &DTabBar::tabCloseRequested, this, &Window::handleTabCloseRequested, Qt::QueuedConnection);
//    connect(m_tabbar, &DTabBar::tabAddRequested, this, static_cast<void (Window::*)()>(&Window::addBlankTab), Qt::QueuedConnection);
//    connect(m_tabbar, &DTabBar::currentChanged, this, &Window::handleCurrentChanged, Qt::QueuedConnection);

//    connect(newWindowAction, &QAction::triggered, this, &Window::newWindow);
//    connect(newTabAction, &QAction::triggered, this, static_cast<void (Window::*)()>(&Window::addBlankTab));
//    connect(openFileAction, &QAction::triggered, this, &Window::openFile);
//    connect(findAction, &QAction::triggered, this, &Window::popupFindBar);
//    connect(replaceAction, &QAction::triggered, this, &Window::popupReplaceBar);
//    connect(saveAction, &QAction::triggered, this, &Window::saveFile);
//    connect(saveAsAction, &QAction::triggered, this, &Window::saveAsFile);
//    connect(printAction, &QAction::triggered, this, &Window::popupPrintDialog);
//    connect(settingAction, &QAction::triggered, this, &Window::popupSettingsDialog);
//    connect(switchThemeAction, &QAction::triggered, this, &Window::popupThemePanel); m_menu->addAction(settingAction);

//    m_menu->setStyle(QStyleFactory::create("dlight"));
//    m_menu->setMinimumWidth(150);

//    ToolBar *toolBar = new ToolBar;
//    toolBar->setTabbar(m_tabbar);

//    titlebar()->setCustomWidget(toolBar, Qt::AlignVCenter, false);
//    titlebar()->setAutoHideOnFullscreen(true);
//    titlebar()->setSeparatorVisible(true);
//    titlebar()->setFixedHeight(40);
//    titlebar()->setMenu(m_menu);

//    connect(m_tabbar, &DTabBar::tabBarDoubleClicked, titlebar(), &DTitlebar::doubleClicked, Qt::QueuedConnection);

//    connect(m_tabbar, &Tabbar::closeTabs, this, &Window::handleTabsClosed, Qt::QueuedConnection);
//    connect(m_tabbar, &Tabbar::requestHistorySaved, this, [=] (const QString &filePath) {
//        if (QFileInfo(filePath).dir().absolutePath() == m_blankFileDir) {
//            return;
//        }

//        if (!m_closeFileHistory.contains(filePath)) {
//            m_closeFileHistory << filePath;
//        }
//    });

//    connect(m_tabbar, &DTabBar::tabCloseRequested, this, &Window::handleTabCloseRequested, Qt::QueuedConnection);
//    connect(m_tabbar, &DTabBar::tabAddRequested, this, static_cast<void (Window::*)()>(&Window::addBlankTab), Qt::QueuedConnection);
//    connect(m_tabbar, &DTabBar::currentChanged, this, &Window::handleCurrentChanged, Qt::QueuedConnection);

//    connect(newWindowAction, &QAction::triggered, this, &Window::newWindow);
//    connect(newTabAction, &QAction::triggered, this, static_cast<void (Window::*)()>(&Window::addBlankTab));
//    connect(openFileAction, &QAction::triggered, this, &Window::openFile);
//    connect(findAction, &QAction::triggered, this, &Window::popupFindBar);
//    connect(replaceAction, &QAction::triggered, this, &Window::popupReplaceBar);
//    connect(saveAction, &QAction::triggered, this, &Window::saveFile);
//    connect(saveAsAction, &QAction::triggered, this, &Window::saveAsFile);
//    connect(printAction, &QAction::triggered, this, &Window::popupPrintDialog);
//    connect(settingAction, &QAction::triggered, this, &Window::popupSettingsDialog);
//    connect(switchThemeAction, &QAction::triggered, this, &Window::popupThemePanel);
}
