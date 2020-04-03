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
#include "CentralDocPage.h"

DWIDGET_USE_NAMESPACE

MainWindow *MainWindow::g_onlyMainWindow = nullptr;

MainWindow *MainWindow::Instance()
{
    return g_onlyMainWindow;
}

MainWindow::MainWindow(DMainWindow *parent)
    : DMainWindow(parent)
{
    setTitlebarShadowEnabled(true);

    m_strObserverName = "MainWindow";

    m_pMsgList = {MSG_OPERATION_EXIT};

    setCurTheme();

    initUI();

    initThemeChanged();

    initShortCut();

    g_onlyMainWindow = this;

    dApp->m_pModelService->addObserver(this);

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
    // We don't need clean pointers because application has exit here.
    dApp->m_pModelService->removeObserver(this);
}

void MainWindow::openfile(const QString &filepath)
{
    emit  sigopenfile(filepath);
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
    event->ignore();

    dApp->m_pAppInfo->setAppKeyValue(KEY_APP_WIDTH, QString("%1").arg(this->width()));

    dApp->m_pAppInfo->setAppKeyValue(KEY_APP_HEIGHT, QString("%1").arg(this->height()));

    QJsonObject obj;

    obj.insert("type", "exit_app");

    QJsonDocument doc = QJsonDocument(obj);

    notifyMsg(E_APP_MSG_TYPE, doc.toJson(QJsonDocument::Compact));

}

void MainWindow::initUI()
{
    Central *central = new Central;
    setCentralWidget(central);

    titlebar()->setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    titlebar()->setTitle("");
    titlebar()->setMenu(central->titleMenu());
    titlebar()->addWidget(central->titleWidget(), Qt::AlignLeft);

    connect(this, &MainWindow::sigopenfile, central, &Central::SlotOpenFiles);
}

void MainWindow::initThemeChanged()
{
    //  主题变了
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ]() {
        setCurTheme();
        notifyMsg(MSG_OPERATION_UPDATE_THEME);
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
        notifyMsg(E_OPEN_FILE);
    } else {
        QJsonObject obj;
        obj.insert("type", "ShortCut");
        obj.insert("key", key);

        QJsonDocument doc = QJsonDocument(obj);
        notifyMsg(E_APP_MSG_TYPE, doc.toJson(QJsonDocument::Compact));
    }
}

void MainWindow::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
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

int MainWindow::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_EXIT) {
        onAppExit();
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }
    return MSG_NO_OK;
}
