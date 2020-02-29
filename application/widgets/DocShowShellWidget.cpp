#include "DocShowShellWidget.h"

#include <DDialogCloseButton>
#include <QDesktopWidget>
#include <QStackedLayout>

#include "FileViewWidget.h"
#include "FindWidget.h"
#include "FileAttrWidget.h"
#include "PlayControlWidget.h"

#include "MsgModel.h"

#include "docview/docummentproxy.h"
#include "business/FileDataManager.h"
#include "utils/PublicFunction.h"

#include "pdfControl/note/NoteViewWidget.h"

DocShowShellWidget::DocShowShellWidget(CustomWidget *parent)
    : CustomWidget("DocShowShellWidget", parent),
      m_pctrlwidget(new  PlayControlWidget(this))
{
    m_pctrlwidget->hide();

    initWidget();
    initConnections();

    m_pMsgList = {MSG_OPERATION_ATTR, MSG_OPERATION_TEXT_ADD_ANNOTATION,
                  MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, MSG_NOTE_PAGE_SHOW_NOTEWIDGET
                 };

    dApp->m_pModelService->addObserver(this);
}

DocShowShellWidget::~DocShowShellWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

void DocShowShellWidget::resizeEvent(QResizeEvent *event)
{
    int nState = dApp->m_pAppInfo->qGetCurShowState();
    if (nState == FILE_NORMAL || nState == FILE_FULLSCREEN) {
        auto findWidget = this->findChild<FindWidget *>();
        if (findWidget && findWidget->isVisible()) {
            int nParentWidth = this->width();
            findWidget->showPosition(nParentWidth);
        }
    }
    CustomWidget::resizeEvent(event);
}

//  全屏 \ 幻灯片放映, 显示 关闭按钮
void DocShowShellWidget::slotShowCloseBtn(const int &iFlag)
{
    DIconButton *closeBtn = nullptr;

    auto iconBtnList = this->findChildren<DIconButton *>();
    foreach (auto btn, iconBtnList) {
        if (/*btn->objectName() == "slider" ||*/ btn->objectName() == "fullscreen") {
            closeBtn = btn;
            break;
        }
    }

    if (closeBtn == nullptr) {
        closeBtn = new DIconButton(this);

        connect(closeBtn, &DIconButton::clicked, this, &DocShowShellWidget::slotBtnCloseClicked);

        closeBtn->setIconSize(QSize(50, 50));
        closeBtn->setFixedSize(QSize(50, 50));
    }

    /* if (iFlag == 2) {
         QString sIcon = PF::getImagePath("exit_slider", Pri::g_actions);
         closeBtn->setIcon(QIcon(sIcon));
         closeBtn->setObjectName("slider");
     } else */{
        QString sIcon = PF::getImagePath("exit_fullscreen", Pri::g_actions);
        closeBtn->setIcon(QIcon(sIcon));
        closeBtn->setObjectName("fullscreen");
    }

    int nScreenWidth = qApp->desktop()->geometry().width();
    closeBtn->move(nScreenWidth - 50, 0);

    closeBtn->show();
    closeBtn->raise();
}

void DocShowShellWidget::slotHideCloseBtn()
{
    DIconButton *closeBtn = nullptr;

    auto iconBtnList = this->findChildren<DIconButton *>();
    foreach (auto btn, iconBtnList) {
        if (/*btn->objectName() == "slider" || */btn->objectName() == "fullscreen") {
            closeBtn = btn;
            break;
        }
    }

    if (closeBtn != nullptr && closeBtn->isVisible()) {
        closeBtn->hide();
    }
}

//  获取文件的基本数据，　进行展示
void DocShowShellWidget::onShowFileAttr()
{
    auto pFileAttrWidget = new FileAttrWidget;
    pFileAttrWidget->showScreenCenter();
}

//  搜索框
void DocShowShellWidget::slotShowFindWidget()
{
    auto findWidget = this->findChild<FindWidget *>();
    if (!findWidget) {
        findWidget = new FindWidget(this);
    }

    int nParentWidth = this->width();
    findWidget->showPosition(nParentWidth);
    findWidget->setSearchEditFocus();
}

//  注释窗口
void DocShowShellWidget::onOpenNoteWidget(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 3) {

        QString sPage = sList.at(0);
//        QString sX = sList.at(1);
//        QString sY = sList.at(2);

        auto pWidget = this->findChild<NoteViewWidget *>();
        if (pWidget == nullptr) {
            pWidget = new NoteViewWidget(this);
        }
        pWidget->setEditText("");
        pWidget->setNoteUuid("");
        pWidget->setNotePage(sPage);

        QPoint point;
        bool t_bHigh = false;
        dApp->m_pAppInfo->setSmallNoteWidgetSize(pWidget->size());
        dApp->m_pAppInfo->mousePressLocal(t_bHigh, point);
        pWidget->showWidget(point);
    }
}

//  显示 当前 注释
void DocShowShellWidget::onShowNoteWidget(const QString &contant)
{
    QStringList t_strList = contant.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (t_strList.count() == 2) {
        QString t_strUUid = t_strList.at(0);
        QString t_page = t_strList.at(1);

        QString sContant = "";

        auto pHelper = DocummentProxy::instance();
        if (pHelper) {
            pHelper->getAnnotationText(t_strUUid, sContant, t_page.toInt());
        }

        auto pWidget = this->findChild<NoteViewWidget *>();
        if (pWidget == nullptr) {
            pWidget = new NoteViewWidget(this);
        }
        pWidget->setNoteUuid(t_strUUid);
        pWidget->setNotePage(t_page);
        pWidget->setEditText(sContant);
//        pWidget->setPointAndPage("");
        dApp->m_pAppInfo->setSmallNoteWidgetSize(pWidget->size());

        bool t_bHigh = false;  // 点击位置是否是高亮
        QPoint point;          // = this->mapToGlobal(rrect.bottomRight());// 鼠标点击位置

        dApp->m_pAppInfo->mousePressLocal(t_bHigh, point);
        pWidget->showWidget(point);
    }
}

void DocShowShellWidget::__ShowPageNoteWidget(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 4) {
        QString sUuid = sList.at(0);
        QString sPage = sList.at(1);
        QString sX = sList.at(2);
        QString sY = sList.at(3);

        QString sContant = "";

        auto pHelper = DocummentProxy::instance();
        if (pHelper) {
            pHelper->getAnnotationText(sUuid, sContant, sPage.toInt());
        }
        auto pWidget = this->findChild<NoteViewWidget *>();
        if (pWidget == nullptr) {
            pWidget = new NoteViewWidget(this);
        }
        pWidget->setEditText(sContant);
        pWidget->setNoteUuid(sUuid);
        pWidget->setNotePage(sPage);
        pWidget->setWidgetType(NOTE_ICON);
        pWidget->showWidget(QPoint(sX.toInt(), sY.toInt()));
    }
}

void DocShowShellWidget::InitSpinner()
{
    m_pSpinerWidget = new DWidget(this);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addStretch();

    m_pSpiner = new DSpinner(m_pSpinerWidget);
    m_pSpiner->setFixedSize(QSize(36, 36));
    m_pSpiner->start();

    vLayout->addWidget(m_pSpiner);
    vLayout->addStretch();

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addItem(vLayout);
    hLayout->addStretch();

    m_pSpinerWidget->setLayout(hLayout);
}

void DocShowShellWidget::OnDocProxyMsg(const QString &msgContent)
{
    MsgModel mm;
    mm.fromJson(msgContent);

    int nMsg = mm.getMsgType();
    if (nMsg == MSG_OPERATION_OPEN_FILE_OK) {
        QString sPath = mm.getPath();
        slotOpenFileOk(sPath);
    }
}

void DocShowShellWidget::slotBtnCloseClicked()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
}

void DocShowShellWidget::slotUpdateTheme()
{
    auto closeBtn = this->findChild<DIconButton *>();
    if (!closeBtn) {
        if (closeBtn->objectName() == "fullscreen") {
            QString sIcon = PF::getImagePath("exit_fullscreen", Pri::g_actions);
            closeBtn->setIcon(QIcon(sIcon));
        }
    }
}

void DocShowShellWidget::slotChangePlayCtrlShow(bool bshow)
{
    if (bshow && m_pctrlwidget->canShow()) {
        int nScreenWidth = qApp->desktop()->geometry().width();
        int inScreenHeght = qApp->desktop()->geometry().height();
        m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - 100);
    } else {
        m_pctrlwidget->killshow();
    }
}

void DocShowShellWidget::slotOpenFileOk(const QString &sPath)
{
    if (/*this->isVisible()*/!sPath.isEmpty()) {
        dApp->m_pDataManager->qSetCurrentFilePath(sPath);

        MsgModel mm;
        mm.setMsgType(MSG_OPERATION_OPEN_FILE_OK);
        mm.setPath(sPath);

        notifyMsg(E_DOCPROXY_MSG_TYPE, mm.toJson());
    }

    m_playout->removeWidget(m_pSpinerWidget);

    delete  m_pSpinerWidget;
    m_pSpinerWidget = nullptr;
}

void DocShowShellWidget::slotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_ATTR) {                            //  打开该文件的属性信息
        onShowFileAttr();
    } else if (msgType == MSG_OPERATION_TEXT_ADD_ANNOTATION) {      //  添加注释
        onOpenNoteWidget(msgContent);
    } else if (msgType ==  MSG_OPERATION_TEXT_SHOW_NOTEWIDGET) {    //  显示注释窗口
        onShowNoteWidget(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_SHOW_NOTEWIDGET) {          //  显示注释窗口
        __ShowPageNoteWidget(msgContent);
    }
}

void DocShowShellWidget::initConnections()
{
    connect(this, SIGNAL(sigOpenFileOk(const QString &)), SLOT(slotOpenFileOk(const QString &)));
    connect(this, SIGNAL(sigShowFileFind()), SLOT(slotShowFindWidget()));
    connect(this, SIGNAL(sigShowCloseBtn(const int &)), SLOT(slotShowCloseBtn(const int &)));
    connect(this, SIGNAL(sigHideCloseBtn()), SLOT(slotHideCloseBtn()));
    connect(this, SIGNAL(sigChangePlayCtrlShow(bool)), SLOT(slotChangePlayCtrlShow(bool)));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)),
            SLOT(slotDealWithData(const int &, const QString &)));
}

//  集中处理 按键通知消息
int DocShowShellWidget::dealWithNotifyMsg(const QString &msgContent)
{
    if (KeyStr::g_ctrl_f == msgContent && dApp->m_pAppInfo->qGetCurShowState() != FILE_SLIDE) { //  搜索
        emit sigShowFileFind();
        return ConstantMsg::g_effective_res;
    }

    if (KeyStr::g_f11 == msgContent && dApp->m_pAppInfo->qGetCurShowState() != FILE_SLIDE) { //  全屏
        if (dApp->m_pAppInfo->qGetCurShowState() == FILE_FULLSCREEN)
            emit sigHideCloseBtn();
        else {
            emit sigShowCloseBtn(0);
        }
    }
    if (KeyStr::g_esc == msgContent) {  //  退出   幻灯片\全屏
        emit sigHideCloseBtn();
        m_pctrlwidget->setCanShow(false);
        emit sigChangePlayCtrlShow(false);
    }
    return 0;
}

int DocShowShellWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_OPERATION_SLIDE) {
        m_pctrlwidget->setCanShow(true);
        emit sigChangePlayCtrlShow(true);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        //  最后一个处理通知消息
        return dealWithNotifyMsg(msgContent);
    }
    return 0;
}

void DocShowShellWidget::initWidget()
{
    m_playout = new QStackedLayout;
    m_playout->setContentsMargins(0, 0, 0, 0);
    m_playout->setSpacing(0);

    auto pViewWidget = new FileViewWidget(this);
    connect(pViewWidget, SIGNAL(sigFileOpenOK(const QString &)), SLOT(slotOpenFileOk(const QString &)));
    connect(pViewWidget, SIGNAL(sigShowPlayCtrl(bool)), this, SLOT(slotChangePlayCtrlShow(bool)));

    InitSpinner();
    m_playout->addWidget(m_pSpinerWidget);

    m_playout->addWidget(pViewWidget);

    this->setLayout(m_playout);
}

