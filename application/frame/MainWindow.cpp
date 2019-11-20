#include "MainWindow.h"
#include "TitleWidget.h"
#include "MainWidget.h"

#include <DDialog>
#include <DTitlebar>
#include <DWidgetUtil>
#include <DFloatingMessage>
#include <DMessageManager>
#include <QDebug>
#include <QDesktopServices>
#include <QProcess>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "controller/DataManager.h"
#include <DGuiApplicationHelper>
#include "DocummentFileHelper.h"
#include <DMessageManager>

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

    m_pFilterList = QStringList() << KeyStr::g_esc << KeyStr::g_f1 << KeyStr::g_f11 << KeyStr::g_del
                    << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2 << KeyStr::g_ctrl_3 << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r
                    << KeyStr::g_pgup << KeyStr::g_pgdown << KeyStr::g_ctrl_f << KeyStr::g_ctrl_o
                    << KeyStr::g_ctrl_p << KeyStr::g_ctrl_s << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_smaller
                    << KeyStr::g_alt_1 << KeyStr::g_alt_2 << KeyStr::g_m << KeyStr::g_z
                    << KeyStr::g_ctrl_alt_f << KeyStr::g_ctrl_shift_s
                    << KeyStr::g_down << KeyStr::g_up << KeyStr::g_left << KeyStr::g_right
                    << KeyStr::g_ctrl_e << KeyStr::g_ctrl_b << KeyStr::g_ctrl_i << KeyStr::g_ctrl_l << KeyStr::g_ctrl_shift_slash
                    << KeyStr::g_space;


    installEventFilter(this);

    m_pMsgSubject = MsgSubject::getInstance();

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
        m_pMsgSubject->stopThreadRun();
    }

    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
        m_pNotifySubject->stopThreadRun();
    }
}

void MainWindow::openfile(const QString &filepath)
{
    notifyMsg(MSG_OPEN_FILE_PATH, filepath);
}

void MainWindow::setSreenRect(const QRect &rect)
{
    DataManager::instance()->setScreenRect(rect);
}

//  窗口关闭
void MainWindow::closeEvent(QCloseEvent *event)
{
    QString sFilePath = DataManager::instance()->strOnlyFilePath();
    if (sFilePath != "") {
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            DDialog dlg(tr("Save File"), tr("Do you need to save the file opened?"));
            dlg.setIcon(QIcon::fromTheme("deepin-reader"));
            dlg.addButtons(QStringList() <<  tr("Cancel") << tr("Not Save") <<  tr("Save"));
            int nRes = dlg.exec();
            if (nRes <= 0) {
                event->ignore();
                return;
            }

            if (nRes == 2) {
                //  保存
                DocummentFileHelper::instance()->save(sFilePath, true);
                //  保存 书签数据
                dApp->dbM->saveBookMark();
            }

            notifyMsg(MSG_CLOSE_FILE);
            DocummentFileHelper::instance()->closeFile();
        }
    }
    DMainWindow::closeEvent(event);
}

//  事件 过滤器
bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    int nType = e->type();
    if (nType == QEvent::KeyPress) {    //  按下
        QKeyEvent *event = static_cast<QKeyEvent *>(e);
        QString key = Utils::getKeyshortcut(event);

        qDebug() << "key = " << key;

        if (m_pFilterList.contains(key)) {
            dealWithKeyEvent(key);
            return true;
        }
    } else if (nType == QEvent::Wheel) {
        QWheelEvent *event = static_cast<QWheelEvent *>(e);
        if ( QApplication::keyboardModifiers () == Qt::ControlModifier) {
            QString sFilePath = DataManager::instance()->strOnlyFilePath();
            if (sFilePath != "") {
                if (event->delta() > 0) {
                    notifyMsg(MSG_OPERATION_LARGER);
                } else {
                    notifyMsg(MSG_OPERATION_SMALLER);
                }
                return true;
            }
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
    connect(this, SIGNAL(sigAppExit()), this, SLOT(slotAppExit()));
    connect(this, SIGNAL(sigAppShowState(const int &)), this, SLOT(slotAppShowState(const int &)));
    connect(this, SIGNAL(sigSetAppTitle(const QString &)), this, SLOT(slotSetAppTitle(const QString &)));
    connect(this, SIGNAL(sigShowTips(const QString &)), this, SLOT(slotShowTips(const QString &)));
    connect(this, &MainWindow::sigSpacePressed, this, []() {
        if (DocummentProxy::instance()) {
            if (DocummentProxy::instance()->getAutoPlaySlideStatu()) {
                DocummentProxy::instance()->setAutoPlaySlide(false);
            } else  {
                DocummentProxy::instance()->setAutoPlaySlide(true);
            }
        }
    });

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
    notifyMsg(MSG_OPERATION_FULLSCREEN);
}

//  放映
void MainWindow::onScreening()
{
    slotAppShowState(0);
    notifyMsg(MSG_OPERATION_SLIDE);
}

void MainWindow::initThemeChanged()
{
    //  主题变了
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ] () {
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

    DataManager::instance()->settrCurrentTheme(sTheme);
}

//  显示帮助文档
void MainWindow::onOpenAppHelp()
{
    QDesktopServices::openUrl(QUrl(Constant::sAcknowledgementLink));
}

//  切换窗口大小
void MainWindow::onChangeWindowState()
{
    if (windowState() == Qt::WindowMaximized) {
        this->setWindowState(Qt::WindowNoState);
    } else {
        this->setWindowState(Qt::WindowMaximized);
    }
}

void MainWindow::dealWithKeyEvent(const QString &key)
{
    qDebug() << __FUNCTION__ << key;
    if (key == KeyStr::g_f1) {
        onOpenAppHelp();
    } else if (key == KeyStr::g_ctrl_alt_f) {
        onChangeWindowState();
    } else if (key == KeyStr::g_ctrl_shift_slash) {
        displayShortcuts();
    }    else {
        QString sFilePath = DataManager::instance()->strOnlyFilePath();
        if (sFilePath != "") {
            if (key == KeyStr::g_ctrl_larger) {
                notifyMsg(MSG_OPERATION_LARGER);
            } else if (key == KeyStr::g_ctrl_smaller) {
                notifyMsg(MSG_OPERATION_SMALLER);
            } else if (key == KeyStr::g_f11) {
                onFullScreen();
            } else {
                notifyMsg(MSG_NOTIFY_KEY_MSG, key);
            }
        }
    }
}
void MainWindow::displayShortcuts()
{
    QRect rect = window()->geometry();
    QPoint pos(rect.x() + rect.width() / 2,
               rect.y() + rect.height() / 2);

    QStringList shortcutnames;
    QStringList windowKeymaps;
    windowKeymaps << KeyStr::g_ctrl_alt_f << KeyStr::g_f11 << KeyStr::g_esc << KeyStr::g_alt_f4 << KeyStr::g_f1
                  << KeyStr::g_ctrl_f << KeyStr::g_pgup << KeyStr::g_pgdown << KeyStr::g_ctrl_o << KeyStr::g_ctrl_larger
                  << KeyStr::g_ctrl_smaller << KeyStr::g_ctrl_wheel << KeyStr::g_ctrl_shift_s << KeyStr::g_ctrl_e
                  << KeyStr::g_ctrl_p << KeyStr::g_ctrl_s << KeyStr::g_m << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2
                  << KeyStr::g_ctrl_3 << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r << KeyStr::g_alt_1 << KeyStr::g_alt_2
                  << KeyStr::g_ctrl_b << KeyStr::g_ctrl_i << KeyStr::g_ctrl_l << KeyStr::g_del << KeyStr::g_z << KeyStr::g_ctrl_shift_slash;

    shortcutnames << tr("Restore") << tr("FullScreen") << tr("Escape") << tr("Quit") << tr("Help")
                  << tr("Search") << tr("PageUp") << tr("PageDown") << tr("Open") << tr("Enlarge")
                  << tr("Narrow") << tr("PageScale") << tr("SaveAs") << tr("Export") << tr("Print")
                  << tr("Save") << tr("OpenThumbnail") << tr("AdaptePage") << tr("AdapteHeight")
                  << tr("AdapteWidth") << tr("LeftRotation") << tr("RightRotation") << tr("SelectTool")
                  << tr("HandTool") << tr("AddBookMark") << tr("AddNote") << tr("AddHighlight")
                  << tr("Delete") << tr("Magnifier") << tr("ShortcutPreview");

    // windowKeymaps=m_pFilterList;
    QJsonObject shortcutObj;
    QJsonArray jsonGroups;

    QJsonObject windowJsonGroup;
    windowJsonGroup.insert("groupName", "Window");
    QJsonArray windowJsonItems;

    int index = 0;
    for (const QString &shortcutname : shortcutnames ) {

        QJsonObject jsonItem;
        jsonItem.insert("name", shortcutname);
        jsonItem.insert("value", windowKeymaps.at(index));
        windowJsonItems.append(jsonItem);
        index++;
    }

    windowJsonGroup.insert("groupItems", windowJsonItems);
    jsonGroups.append(windowJsonGroup);
    shortcutObj.insert("shortcut", jsonGroups);

    QJsonDocument doc(shortcutObj);

    QStringList shortcutString;
    QString param1 = "-j=" + QString(doc.toJson().data());
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());

    shortcutString << param1 << param2;

    QProcess *shortcutViewProcess = new QProcess();
    shortcutViewProcess->startDetached("deepin-shortcut-viewer", shortcutString);

    connect(shortcutViewProcess, SIGNAL(finished(int)), shortcutViewProcess, SLOT(deleteLater()));
}

//  退出 应用
void MainWindow::slotAppExit()
{
    QString sFilePath = DataManager::instance()->strOnlyFilePath();
    if (sFilePath != "") {
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            DDialog dlg(tr("Save File"), tr("Do you need to save the file opened?"));
            dlg.setIcon(QIcon::fromTheme("deepin-reader"));
            dlg.addButtons(QStringList() <<  tr("Cancel") << tr("Not Save") <<  tr("Save"));
            int nRes = dlg.exec();
            if (nRes <= 0) {
                return;
            }

            if (nRes == 2) {
                //  保存
                DocummentFileHelper::instance()->save(sFilePath, true);
                //  保存 书签数据
                dApp->dbM->saveBookMark();
            }

            notifyMsg(MSG_CLOSE_FILE);
            DocummentFileHelper::instance()->closeFile();
        }
    }
    dApp->exit(0);
}

/**
 * @brief MainWindow::slotAppShowState
 * @param nState 1 退出全屏, 0 全屏显示
 */
void MainWindow::slotAppShowState(const int &nState)
{
    titlebar()->setVisible(nState);
    setTitlebarShadowEnabled(nState);

    if (nState == 1) {
        if (windowState() == Qt::WindowFullScreen) {
            DataManager::instance()->setCurShowState(FILE_NORMAL);  //  正常状态
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

//  点击菜单　发送指令
void MainWindow::slotActionTrigger(const QString &sAction)
{
    if (sAction == "Open File") {
        notifyMsg(MSG_OPERATION_OPEN_FILE);
    } else if (sAction == "Save File") {
        notifyMsg(MSG_OPERATION_SAVE_FILE);
    } else if (sAction == "Save As File") {
        notifyMsg(MSG_OPERATION_SAVE_AS_FILE);
    } else if (sAction == "Open Folder") {
        onOpenFolder();
    } else if (sAction == "Print") {
        notifyMsg(MSG_OPERATION_PRINT);
    } else if (sAction == "File Attr") {
        notifyMsg(MSG_OPERATION_ATTR);
    } else if (sAction == "Search") {
        notifyMsg(MSG_OPERATION_FIND);
    } else if (sAction == "Full Screen") {
        onFullScreen();
    } else if (sAction == "Screening") {
        onScreening();
    } else if (sAction == "Larger") {
        notifyMsg(MSG_OPERATION_LARGER);
    } else if (sAction == "Smaller") {
        notifyMsg(MSG_OPERATION_SMALLER);
    }
}

void MainWindow::slotShowTips(const QString &contant)
{
    DMessageManager::instance()->sendMessage(this, QIcon(":/resources/light/pdfControl/ok.svg"), contant);
}

void MainWindow::sendMsg(const int &, const QString &)
{

}

void MainWindow::notifyMsg(const int &msgType, const QString &msgContent)
{
    if (m_pNotifySubject) {
        m_pNotifySubject->notifyMsg(msgType, msgContent);
    }
}

int MainWindow::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk();
    } else if (msgType == MSG_OPERATION_OPEN_FILE_TITLE) {
        emit sigSetAppTitle(msgContent);
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_OPERATION_EXIT) {
        emit sigAppExit();
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_esc) {          //  退出全屏模式
            emit sigAppShowState(1);
        } else if (msgContent == KeyStr::g_space) {
            if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
                qDebug() << __FUNCTION__ << "+++++++++++";
                emit sigSpacePressed();
            }
        }
    } else if (msgType == MSG_OPERATION_TEXT_SHOW_TIPS) {
        emit sigShowTips(msgContent);
        return ConstantMsg::g_effective_res;
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
