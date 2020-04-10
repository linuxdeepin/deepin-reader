#include "MainWindow.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <QSignalMapper>
#include <QDebug>
#include <DGuiApplicationHelper>

#include "business/ShortCutShow.h"
#include "business/AppInfo.h"
#include "TitleMenu.h"
#include "TitleWidget.h"
#include "Central.h"

DWIDGET_USE_NAMESPACE

QList<MainWindow *> MainWindow::m_list;

MainWindow::MainWindow(DMainWindow *parent)
    : DMainWindow(parent)
{
    m_list.append(this);

    setTitlebarShadowEnabled(true);

    setCurTheme();

    initUI();

    initThemeChanged();

    initShortCut();

    //暂定752*360，后期根据最合适效果设定
    int tWidth = 752;
    int tHeight = 360;

    dApp->adaptScreenView(tWidth, tHeight);
    setMinimumSize(tWidth, tHeight);

    showDefaultSize();

    //  在屏幕中心显示
    Dtk::Widget::moveToCenter(this);
}

MainWindow::~MainWindow()
{
    m_list.removeOne(this);
}

void MainWindow::addSheet(DocSheet *sheet)
{
    m_central->addSheet(sheet);
}

void MainWindow::addFile(const QString &filepath)
{
    m_central->openFile(filepath);
}

void MainWindow::openfile(const QString &filepath)
{
    m_central->openFile(filepath);
}

void MainWindow::setSreenRect(const QRect &rect)
{
    dApp->m_pAppInfo->setScreenRect(rect);
}

//  0 开启幻灯片, 1 取消幻灯片
void MainWindow::SetSliderShowState(const int &nState)
{
    titlebar()->setVisible(nState);

    if (nState == 1) {
        if (windowState() == Qt::WindowFullScreen) {

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

//  临时 添加 焦点
void MainWindow::showEvent(QShowEvent *ev)
{
    this->setFocus();
    DMainWindow::showEvent(ev);
}

//  窗口关闭
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_central->saveAll()) {

        dApp->m_pAppInfo->setAppKeyValue(KEY_APP_WIDTH, QString("%1").arg(this->width()));

        dApp->m_pAppInfo->setAppKeyValue(KEY_APP_HEIGHT, QString("%1").arg(this->height()));

        event->accept();

        this->deleteLater();

    } else
        event->ignore();
}

void MainWindow::initUI()
{
    m_central = new Central(this);
    connect(m_central, SIGNAL(sigNeedClose()), this, SLOT(close()));
    connect(m_central, SIGNAL(sigNeedShowState(int)), this, SLOT(onShowState(int)));

    setCentralWidget(m_central);

    titlebar()->setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    titlebar()->setTitle("");
    titlebar()->setMenu(m_central->titleMenu());
    titlebar()->addWidget(m_central->titleWidget(), Qt::AlignLeft);
}

void MainWindow::initThemeChanged()
{
    //  主题变了
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ]() {
        setCurTheme();
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

    dApp->m_pAppInfo->qSetCurrentTheme(sTheme);
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

//  快捷键 实现
void MainWindow::slotShortCut(const QString &key)
{
    if (key == KeyStr::g_ctrl_shift_slash) { //  显示快捷键预览
        displayShortcuts();
    } else  if (key == KeyStr::g_ctrl_o) {
        m_central->openFilesExec();
    } else {
        m_central->handleShortcut(key);
    }
}

void MainWindow::onShowState(int state)
{
    titlebar()->setVisible(state);

    if (state == 1) {
        if (windowState() == Qt::WindowFullScreen) {

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

MainWindow *MainWindow::create()
{
    return new MainWindow();
}

//  窗口显示默认大小
void MainWindow::showDefaultSize()
{
    int nWidth = dApp->m_pAppInfo->getAppKeyValue(KEY_APP_WIDTH).toInt();
    int nHeight = dApp->m_pAppInfo->getAppKeyValue(KEY_APP_HEIGHT).toInt();

    if (nWidth == 0 || nHeight == 0) {
        int tWidth = 1000;
        int tHeight = 680;
        QString str = "";

        dApp->adaptScreenView(tWidth, tHeight);
        resize(tWidth, tHeight);

        str = QString::number(tWidth);
        dApp->m_pAppInfo->setAppKeyValue(KEY_APP_WIDTH, str);
        str = QString::number(tHeight);
        dApp->m_pAppInfo->setAppKeyValue(KEY_APP_HEIGHT, str);
    } else {
        dApp->adaptScreenView(nWidth, nHeight);
        resize(nWidth, nHeight);
    }
}

//  初始化 快捷键操作
void MainWindow::initShortCut()
{
    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(slotShortCut(const QString &)));

    auto keyList = dApp->m_pAppInfo->getKeyList();
    foreach (auto key, keyList) {
        auto action = new QAction;
        action->setShortcut(key);
        this->addAction(action);

        connect(action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(action, key.toString());
    }
}
