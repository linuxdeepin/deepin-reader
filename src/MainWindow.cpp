#include "MainWindow.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <QSignalMapper>
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

    initUI();

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

bool MainWindow::hasSheet(DocSheet *sheet)
{
    return m_central->hasSheet(sheet);
}

void MainWindow::activateSheet(DocSheet *sheet)
{
    this->activateWindow();
    m_central->showSheet(sheet);
}

void MainWindow::openfiles(const QStringList &filepaths)
{
    m_central->openFiles(filepaths);
}

void MainWindow::doOpenFile(const QString &filePath)
{
    m_central->doOpenFile(filePath);
}

void MainWindow::setSreenRect(const QRect &rect)
{
    dApp->m_pAppInfo->setScreenRect(rect);
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

    setCentralWidget(m_central);

    titlebar()->setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    titlebar()->setTitle("");
    titlebar()->setMenu(m_central->titleMenu());
    titlebar()->addWidget(m_central->titleWidget(), Qt::AlignLeft);
    titlebar()->setFocusPolicy(Qt::NoFocus);
}

//  显示快捷键
void MainWindow::displayShortcuts()
{
    ShortCutShow show;
    show.show();
}

//  快捷键 实现
void MainWindow::onShortCut(const QString &key)
{
    if (key == KeyStr::g_ctrl_shift_slash) { //  显示快捷键预览
        displayShortcuts();
    } else {
        m_central->handleShortcut(key);
    }
}

MainWindow *MainWindow::windowContainSheet(DocSheet *sheet)
{
    foreach (MainWindow *window, m_list) {
        if (window->hasSheet(sheet)) {
            return window;
        }
    }

    return nullptr;
}

bool MainWindow::allowCreateWindow()
{
    return m_list.count() < 20;
}

MainWindow *MainWindow::createWindow()
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

    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(onShortCut(const QString &)));

    auto keyList = dApp->m_pAppInfo->getKeyList();

    foreach (auto key, keyList) {
        auto action = new QAction(this);

        action->setShortcut(key);

        this->addAction(action);

        connect(action, SIGNAL(triggered()), pSigManager, SLOT(map()));

        pSigManager->setMapping(action, key.toString());
    }
}
