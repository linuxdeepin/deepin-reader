// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainWindow.h"
#include "TitleMenu.h"
#include "TitleWidget.h"
#include "Central.h"
#include "CentralDocPage.h"
#include "Application.h"
#include "Utils.h"
#include "DocSheet.h"
#include "DBusObject.h"
#include "SaveDialog.h"
#include "eventlogutils.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <DGuiApplicationHelper>
#include <DFileDialog>
#include <DDialog>

#if (DTK_VERSION_MAJOR > 5                                 \
     || ((DTK_VERSION_MAJOR == 5 && DTK_VERSION_MINOR > 2) \
         || (DTK_VERSION_MAJOR == 5 && DTK_VERSION_MINOR == 2 && DTK_VERSION_PATCH > 2)))
#    include <DWindowQuitFullButton>
#else
#    include "dwindowquitfullbutton.h"   //libdtkwidget=5.2.2.10头文件引用错误，这里直接引用dwindowquitfullbutton.h
#endif

#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QTimer>
#include <QScreen>
#include <QPropertyAnimation>
#include <QJsonObject>
#include <QLibraryInfo>

extern "C" {
#include "load_libs.h"
}
DWIDGET_USE_NAMESPACE

QList<MainWindow *> MainWindow::m_list;
MainWindow::MainWindow(QStringList filePathList, DMainWindow *parent)
    : DMainWindow(parent), m_initFilePathList(filePathList)
{
    qDebug() << "MainWindow constructor called with" << filePathList.size() << "files";
    initBase();
    initUI();

    if (!filePathList.isEmpty()) {
        qDebug() << "MainWindow::MainWindow() - Processing" << filePathList.size() << "initial files";
        for (QString &filePath : m_initFilePathList) {
            QUrl url(filePath);
            if (url.isLocalFile()) {
                filePath = url.toLocalFile();
                // qDebug() << "MainWindow::MainWindow() - Converted URL to local file:" << filePath;
            }

            if (QFile(filePath).exists())   //过滤不存在的文件,需求中不含有提示文件不存在的文案
                addFile(filePath);
        }
    }
}

MainWindow::MainWindow(DocSheet *sheet, DMainWindow *parent)
    : DMainWindow(parent)
{
    qDebug() << "MainWindow::MainWindow(DocSheet*) - Starting initialization with sheet:" << (sheet ? sheet->filePath() : "null");
    initBase();

    initUI();

    addSheet(sheet);

    QJsonObject obj {
        { "tid", EventLogUtils::Start },
        { "version", QCoreApplication::applicationVersion() },
        { "mode", 1 }
    };
    EventLogUtils::get().writeLogs(obj);
    qDebug() << "MainWindow::MainWindow(DocSheet*) - Initialization completed";
}

MainWindow::~MainWindow()
{
    // qDebug() << "MainWindow destructor called";
    m_list.removeOne(this);

    if (m_list.count() <= 0) {
        // qDebug() << "MainWindow::~MainWindow() - Unregistering DBus object (last window)";
        DBusObject::instance()->unRegister();
    }
    // qDebug() << "MainWindow::~MainWindow() - Cleanup completed";
}

void MainWindow::addSheet(DocSheet *sheet)
{
    qDebug() << "Adding sheet to window:" << (sheet ? sheet->filePath() : "null");
    if (nullptr == m_central) {
        qDebug() << "MainWindow::addSheet() - Central widget is null, returning";
        return;
    }

    m_central->addSheet(sheet);
    qDebug() << "MainWindow::addSheet() - Sheet added successfully";
}

bool MainWindow::hasSheet(DocSheet *sheet)
{
    qDebug() << "Checking if window contains sheet:" << (sheet ? sheet->filePath() : "null");
    if (nullptr == m_central) {
        qDebug() << "MainWindow::hasSheet() - Central widget is null, returning false";
        return false;
    }

    return m_central->hasSheet(sheet);
}

void MainWindow::activateSheet(DocSheet *sheet)
{
    qDebug() << "Activating sheet:" << (sheet ? sheet->filePath() : "null");
    if (nullptr == m_central) {
        qDebug() << "MainWindow::activateSheet() - Central widget is null, returning";
        return;
    }

    m_central->showSheet(sheet);

    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

    this->activateWindow();
    qDebug() << "MainWindow::activateSheet() - Window activated";
}

bool MainWindow::handleClose(bool needToBeSaved)
{
    qDebug() << "Handling window close, needToBeSaved:" << needToBeSaved;
    if ((nullptr != m_central) && (!m_central->handleClose(needToBeSaved))) {
        qDebug() << "MainWindow::handleClose() - Central widget close failed, aborting";
        return false;
    }

    this->close();
    qDebug() << __FUNCTION__ << "关闭文档查看器主窗口！";
    return true;
}

void MainWindow::addFile(const QString &filePath)
{
    qDebug() << "Adding file to window:" << filePath;
    if (nullptr == m_central) {
        qDebug() << "MainWindow::addFile() - Central widget is null, returning";
        return;
    }

    m_central->addFileAsync(filePath);
    qDebug() << "MainWindow::addFile() - File addition initiated";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow::closeEvent() - Starting close event processing";
    if (m_central && !m_central->handleClose(true)) {
        qDebug() << "MainWindow::closeEvent() - Central widget close failed, ignoring event";
        event->ignore();
        return;
    }

    QSettings settings(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.conf"), QSettings::IniFormat, this);
    qDebug() << "配置文件路径: ***" /* << QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.conf")*/;
    settings.setValue("LASTWIDTH", QString::number(width()));

    settings.setValue("LASTHEIGHT", QString::number(height()));

    qDebug() << __FUNCTION__ << "关闭文档查看器主窗口！";
    DMainWindow::closeEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // qDebug() << "MainWindow::eventFilter() - Processing event type:" << event->type();
    if (obj == this) {
        if (event->type() == QEvent::HoverMove) {
            // qDebug() << "MainWindow::eventFilter() - Processing HoverMove event";
            QHoverEvent *mouseEvent = dynamic_cast<QHoverEvent *>(event);
            bool isFullscreen = this->windowState().testFlag(Qt::WindowFullScreen);
            if (isFullscreen && m_FullTitleWidget && !m_central->docPage()->isSlide()) {
                // qDebug() << "MainWindow::eventFilter() - Processing fullscreen title animation";
                if (m_TitleAnimation == nullptr) {
                    // qDebug() << "MainWindow::eventFilter() - Creating new title animation";
                    m_TitleAnimation = new QPropertyAnimation(m_FullTitleWidget, "geometry");
                    m_TitleAnimation->setEasingCurve(QEasingCurve::OutCubic);
                    connect(m_TitleAnimation, &QPropertyAnimation::finished, this, &MainWindow::onTitleAniFinished);
                }

                if (m_TitleAnimation->state() != QPropertyAnimation::Running) {
                    // qDebug() << "MainWindow::eventFilter() - Starting title animation";
                    m_TitleAnimation->stop();
                    int duration = 200 * (50 + m_FullTitleWidget->pos().y()) / 50;
                    duration = duration <= 0 ? 200 : duration;
                    m_TitleAnimation->setDuration(duration);
                    m_TitleAnimation->setStartValue(QRect(0, m_FullTitleWidget->pos().y(), dApp->primaryScreen()->size().width(), m_FullTitleWidget->height()));

                    if (m_FullTitleWidget->pos().y() >= 0 && mouseEvent->pos().y() > m_FullTitleWidget->height()) {
                        // qDebug() << "MainWindow::eventFilter() - Hiding title widget";
                        m_TitleAnimation->setEndValue(QRect(0, -m_FullTitleWidget->height(), dApp->primaryScreen()->size().width(), m_FullTitleWidget->height()));
                        m_TitleAnimation->start();
                    } else if (m_FullTitleWidget->pos().y() < 0 && mouseEvent->pos().y() < 2) {
                        // qDebug() << "MainWindow::eventFilter() - Showing title widget";
                        setTitleBarFocusEnable(true);
                        if (m_docTabWidget && m_FullTitleWidget->height() > titlebar()->height())
                            m_docTabWidget->setVisible(true);
                        else if (m_docTabWidget && m_FullTitleWidget->height() <= titlebar()->height())
                            m_docTabWidget->setVisible(false);

                        m_TitleAnimation->setEndValue(QRect(0, 0, dApp->primaryScreen()->size().width(), m_FullTitleWidget->height()));
                        m_TitleAnimation->start();
                    }
                }
            }
        }
    }

    if (event->type() == QEvent::Resize) {
        // qDebug() << "MainWindow::eventFilter() - Processing resize event";
        onUpdateTitleLabelRect();
    }
    // qDebug() << "MainWindow::eventFilter() - Calling parent eventFilter";
    return DMainWindow::eventFilter(obj, event);
}

void MainWindow::initUI()
{
    qDebug() << "Initializing main window UI";
    m_central = new Central(this);

    connect(m_central, SIGNAL(sigNeedClose()), this, SLOT(close()));

    m_central->setMenu(m_menu);

    setCentralWidget(m_central);

    titlebar()->setIcon(QIcon::fromTheme("deepin-reader"));

    titlebar()->setTitle("");

    titlebar()->addWidget(m_central->titleWidget(), Qt::AlignLeft);

    titlebar()->addWidget(m_central->docPage()->getTitleLabel(), Qt::AlignLeft);

    titlebar()->setAutoHideOnFullscreen(false);

    Utils::setObjectNoFocusPolicy(this);

    setFocusPolicy(Qt::StrongFocus);

    QTimer::singleShot(100, this, SLOT(onUpdateTitleLabelRect()));

    titlebar()->installEventFilter(this);

    m_central->titleWidget()->installEventFilter(this);

    DIconButton *optBtn = titlebar()->findChild<DIconButton *>("DTitlebarDWindowOptionButton");
    if (optBtn && optBtn->parentWidget()) {
        optBtn->parentWidget()->installEventFilter(this);
    }

    DWindowQuitFullButton *quitFullBtn = titlebar()->findChild<DWindowQuitFullButton *>("DTitlebarDWindowQuitFullscreenButton");
    if (quitFullBtn) {
        qDebug() << "MainWindow::initUI() - Connecting quit fullscreen button";
        connect(quitFullBtn, &DWindowQuitFullButton::clicked, this, [&]() {
            handleMainWindowExitFull();
        });
    }
    qDebug() << __FUNCTION__ << "UI界面初始化已完成";
#ifdef DTKWIDGET_CLASS_DSizeMode
    qDebug() << "MainWindow::initUI() - Setting up size mode change handler";
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        qDebug() << "MainWindow::initUI() - Size mode changed to:" << sizeMode;
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
            qDebug() << "MainWindow::initUI() - Setting normal mode titlebar height";
            titlebar()->setFixedHeight(50);
            handleMainWindowFull();
        } else {
            qDebug() << "MainWindow::initUI() - Setting compact mode titlebar height";
            titlebar()->setFixedHeight(40);
            handleMainWindowFull();
        }
    });
#endif

#if _ZPD_
    //确定移除
    initDynamicLibPath();
#endif
    QJsonObject obj {
        { "tid", EventLogUtils::Start },
        { "version", QCoreApplication::applicationVersion() },
        { "mode", 1 }
    };
    EventLogUtils::get().writeLogs(obj);
    qDebug() << "MainWindow::initUI() - UI initialization completed";
}

void MainWindow::setDocTabBarWidget(QWidget *widget)
{
    qDebug() << "Setting doc tab bar widget:" << widget;
    if (m_FullTitleWidget == nullptr) {
        qDebug() << "MainWindow::setDocTabBarWidget() - Creating new full title widget";
        m_FullTitleWidget = new BaseWidget(this);

        this->stackUnder(m_FullTitleWidget);

        m_FullTitleWidget->setFocusPolicy(Qt::NoFocus);

        m_FullTitleWidget->show();

        setTitleBarFocusEnable(false);
    }

    m_docTabWidget = widget;
    qDebug() << "MainWindow::setDocTabBarWidget() - Widget set successfully";
}

void MainWindow::onTitleAniFinished()
{
    qDebug() << "MainWindow::onTitleAniFinished() - Title animation finished";
    if (m_FullTitleWidget->pos().y() < 0) {
        qDebug() << "MainWindow::onTitleAniFinished() - Disabling title bar focus (widget hidden)";
        setTitleBarFocusEnable(false);
    }
}

void MainWindow::handleMainWindowFull()
{
    qDebug() << "Handling window fullscreen state";
    if (m_FullTitleWidget == nullptr || m_docTabWidget == nullptr) {
        qDebug() << "MainWindow::handleMainWindowFull() - Required widgets are null, returning";
        return;
    }

    m_lastWindowState = Qt::WindowFullScreen;
    if (this->menuWidget()) {
        qDebug() << "MainWindow::handleMainWindowFull() - Removing menu widget";
        this->menuWidget()->setParent(nullptr);
        this->setMenuWidget(nullptr);
    }

    setTitleBarFocusEnable(false);

    bool tabbarVisible = m_docTabWidget->isVisible();

    titlebar()->setParent(m_FullTitleWidget);

    m_docTabWidget->setParent(m_FullTitleWidget);

    titlebar()->show();

    m_docTabWidget->setVisible(tabbarVisible);

    titlebar()->setGeometry(0, 0, QGuiApplication::primaryScreen()->size().width(), titlebar()->height());

    m_docTabWidget->setGeometry(0, titlebar()->height(), QGuiApplication::primaryScreen()->size().width(), 37);

    int fulltitleH = tabbarVisible ? titlebar()->height() + 37 : titlebar()->height();

    m_FullTitleWidget->setMinimumHeight(fulltitleH);

    m_FullTitleWidget->setGeometry(0, -fulltitleH, QGuiApplication::primaryScreen()->size().width(), fulltitleH);

    updateOrderWidgets(this->property("orderlist").value<QList<QWidget *>>());
    qDebug() << "MainWindow::handleMainWindowFull() - Fullscreen setup completed";
}

void MainWindow::handleMainWindowExitFull()
{
    qDebug() << "Handling window exit fullscreen state";
    if (m_FullTitleWidget == nullptr) {
        qDebug() << "MainWindow::handleMainWindowExitFull() - Full title widget is null, returning";
        return;
    }

    if (m_lastWindowState == Qt::WindowFullScreen) {
        qDebug() << "MainWindow::handleMainWindowExitFull() - Exiting fullscreen mode";
        m_lastWindowState = static_cast<int>(this->windowState());

        if (m_central->docPage()->getCurSheet()) {
            qDebug() << "MainWindow::handleMainWindowExitFull() - Closing sheet fullscreen";
            m_central->docPage()->getCurSheet()->closeFullScreen(true);
        }

        this->setMenuWidget(titlebar());

        setTitleBarFocusEnable(true);

        m_FullTitleWidget->setGeometry(0, -m_FullTitleWidget->height(), QGuiApplication::primaryScreen()->size().width(), m_FullTitleWidget->height());

        updateOrderWidgets(this->property("orderlist").value<QList<QWidget *>>());
    }
}

void MainWindow::setTitleBarFocusEnable(bool enable)
{
    qDebug() << "MainWindow::setTitleBarFocusEnable() - Setting focus enable:" << enable;
    auto updateWidgetFocus = [&](const QString &name, Qt::FocusPolicy policy) {
        if (!this->titlebar())
            return;
        QWidget *w = this->titlebar()->findChild<QWidget *>(name);
        if (!w)
            return;
        w->setFocusPolicy(policy);
    };

    updateWidgetFocus("Thumbnails", enable ? Qt::TabFocus : Qt::NoFocus);
    updateWidgetFocus("SP_DecreaseElement", enable ? Qt::TabFocus : Qt::NoFocus);
    updateWidgetFocus("scaleEdit", enable ? Qt::StrongFocus : Qt::NoFocus);
    updateWidgetFocus("editArrowBtn", enable ? Qt::TabFocus : Qt::NoFocus);
    updateWidgetFocus("SP_IncreaseElement", enable ? Qt::TabFocus : Qt::NoFocus);

    updateWidgetFocus("DTitlebarDWindowOptionButton", enable ? Qt::TabFocus : Qt::NoFocus);
    updateWidgetFocus("DTitlebarDWindowMinButton", enable ? Qt::TabFocus : Qt::NoFocus);
    updateWidgetFocus("DTitlebarDWindowQuitFullscreenButton", enable ? Qt::TabFocus : Qt::NoFocus);
    updateWidgetFocus("DTitlebarDWindowMaxButton", enable ? Qt::TabFocus : Qt::NoFocus);
    updateWidgetFocus("DTitlebarDWindowCloseButton", enable ? Qt::TabFocus : Qt::NoFocus);
    qDebug() << "MainWindow::setTitleBarFocusEnable() - Focus policies updated";
}

void MainWindow::resizeFullTitleWidget()
{
    qDebug() << "Resizing full title widget";
    if (m_FullTitleWidget == nullptr || m_docTabWidget == nullptr) {
        qDebug() << "MainWindow::resizeFullTitleWidget() - Required widgets are null, returning";
        return;
    }

    int fulltitleH = m_docTabWidget->isVisible() ? titlebar()->height() + 37 : titlebar()->height();

    m_FullTitleWidget->setMinimumHeight(fulltitleH);

    m_FullTitleWidget->resize(QGuiApplication::primaryScreen()->size().width(), fulltitleH);
    qDebug() << "MainWindow::resizeFullTitleWidget() - Resize completed";
}

MainWindow *MainWindow::windowContainSheet(DocSheet *sheet)
{
    qDebug() << "Finding window containing sheet:" << (sheet ? sheet->filePath() : "null");
    foreach (MainWindow *window, m_list) {
        if (window->hasSheet(sheet)) {
            // qDebug() << "MainWindow::windowContainSheet() - Found containing window";
            return window;
        }
    }

    qDebug() << "MainWindow::windowContainSheet() - No containing window found";
    return nullptr;
}

bool MainWindow::allowCreateWindow()
{
    // qDebug() << "Checking if new window can be created, current count:" << m_list.count();
    bool canCreate = m_list.count() < 20;
    // qDebug() << "MainWindow::allowCreateWindow() - Can create window:" << canCreate;
    return canCreate;
}

bool MainWindow::activateSheetIfExist(const QString &filePath)
{
    qDebug() << "Attempting to activate sheet for file:" << filePath;
    DocSheet *sheet = DocSheet::getSheetByFilePath(filePath);

    if (nullptr == sheet) {
        qDebug() << "MainWindow::activateSheetIfExist() - Sheet not found for file";
        return false;
    }

    MainWindow *window = MainWindow::windowContainSheet(sheet);

    if (nullptr != window) {
        qDebug() << "MainWindow::activateSheetIfExist() - Activating sheet in existing window";
        window->activateSheet(sheet);
        return true;
    }

    qDebug() << "MainWindow::activateSheetIfExist() - No window contains the sheet";
    return false;
}

MainWindow *MainWindow::createWindow(QStringList filePathList)
{
    qDebug() << "Creating new window with" << filePathList.size() << "files";
    int iCount = MainWindow::m_list.count();   // 获取现有窗口数目
    MainWindow *pMainWindow = new MainWindow(filePathList);   // 创建文档查看器对话框

    // 现有数目大于0时，新创建的文档查看器对话框按照一定的规律偏移显示，每次向右、向下偏移50个像素，达到错开显示的效果，防止一直显示在桌面中间
    if (iCount > 0) {
        int windowOffset = iCount * 50;
        QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
        pMainWindow->move(screenGeometry.x() + windowOffset, screenGeometry.y() + windowOffset);
    }
    qDebug() << "MainWindow::createWindow() - Window creation completed";
    return pMainWindow;
}

MainWindow *MainWindow::createWindow(DocSheet *sheet)
{
    // qDebug() << "Creating new window for sheet:" << (sheet ? sheet->filePath() : "null");
    return new MainWindow(sheet);
}

void MainWindow::showDefaultSize()
{
    qDebug() << "Setting window default size";
    QSettings settings(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.conf"), QSettings::IniFormat, this);

    int width = settings.value("LASTWIDTH").toInt();
    int height = settings.value("LASTHEIGHT").toInt();

    if (width == 0 || height == 0) {
        qDebug() << "MainWindow::showDefaultSize() - Using default size 1000x680";
        resize(1000, 680);
    } else {
        qDebug() << "MainWindow::showDefaultSize() - Using saved size";
        resize(width, height);
    }
    qDebug() << "MainWindow::showDefaultSize() - Size setup completed";
}

void MainWindow::initDynamicLibPath()
{
    qDebug() << "Initializing dynamic library paths";
    // 解析ZPD定制需求提供的库 libzpdcallback.so
    LoadLibNames tmp;
    QByteArray documentpr = libPath("libzpdcallback.so").toLatin1();
    if (QFile::exists(documentpr)) {
        qDebug() << "MainWindow::initDynamicLibPath() - Found libzpdcallback.so";
        tmp.chDocumentPr = documentpr.data();
    } else {
        qDebug() << "MainWindow::initDynamicLibPath() - libzpdcallback.so not found";
        tmp.chDocumentPr = NULL;
    }
    setLibNames(tmp);
    qDebug() << "MainWindow::initDynamicLibPath() - Dynamic library initialization completed";
}

QString MainWindow::libPath(const QString &strlib)
{
    qDebug() << "MainWindow::libPath() - Searching for library:" << strlib;
    QDir dir;
    QString path = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
    dir.setPath(path);
    QStringList list = dir.entryList(QStringList() << (strlib + "*"), QDir::NoDotAndDotDot | QDir::Files);   //filter name with strlib

    if (list.contains(strlib)) {
        qDebug() << "MainWindow::libPath() - Found exact match:" << strlib;
        return dir.filePath(strlib);
    }

    list.sort();
    if (list.size() > 0) {
        qDebug() << "MainWindow::libPath() - Using latest version:" << list.last();
        return dir.filePath(list.last());
    }

    qDebug() << "MainWindow::libPath() - No library found";
    return "";
}

void MainWindow::onDelayInit()
{
    qDebug() << "Performing delayed initialization";
    initUI();
}

void MainWindow::initBase()
{
    qDebug() << "Initializing window base resources";

    m_list.append(this);

    setTitlebarShadowEnabled(true);

    setMinimumSize(680, 300);

    showDefaultSize();

    Dtk::Widget::moveToCenter(this);

    this->installEventFilter(this);

    this->setProperty("loading", false);

    this->setProperty("windowClosed", false);

    m_menu = new TitleMenu(this);

    m_menu->setAccessibleName("Menu_Title");

    titlebar()->setMenu(m_menu);

    setAttribute(Qt::WA_DeleteOnClose);

    qDebug() << "MainWindow::initBase() - Base initialization completed";
}

void MainWindow::onUpdateTitleLabelRect()
{
    qDebug() << "Updating title label rectangle";
    if (nullptr == m_central) {
        qDebug() << "MainWindow::onUpdateTitleLabelRect() - Central widget is null, returning";
        return;
    }

    QWidget *titleLabel = m_central->docPage()->getTitleLabel();

    int titleWidth = this->width() - m_central->titleWidget()->width() - titlebar()->buttonAreaWidth() - 60;

    if (titleWidth > 0) {
        qDebug() << "MainWindow::onUpdateTitleLabelRect() - Setting title label width";
        titleLabel->setFixedWidth(titleWidth);
    }
    qDebug() << "MainWindow::onUpdateTitleLabelRect() - Title label update completed";
}

void MainWindow::updateOrderWidgets(const QList<QWidget *> &orderlst)
{
    qDebug() << "Updating widget tab order for" << orderlst.size() << "widgets";
    for (int i = 0; i < orderlst.size() - 1; i++) {
        QWidget::setTabOrder(orderlst.at(i), orderlst.at(i + 1));
    }
    qDebug() << "MainWindow::updateOrderWidgets() - Tab order update completed";
}
