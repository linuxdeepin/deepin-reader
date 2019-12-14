#include "DocShowShellWidget.h"
#include <DDialogCloseButton>
#include <QVBoxLayout>
#include "DocummentFileHelper.h"
#include "FileViewWidget.h"
#include "application.h"
#include "controller/DataManager.h"
#include "mainShow/FindWidget.h"
#include "mainShow/FileAttrWidget.h"
#include "pdfControl/fileViewNote/FileViewNoteWidget.h"
#include "utils/PublicFunction.h"
#include "PlayControlWidget.h"

DocShowShellWidget::DocShowShellWidget(CustomWidget *parent)
    : CustomWidget("DocShowShellWidget", parent)
    , m_pctrlwidget(new PlayControlWidget(this))
{
    initWidget();
    initConnections();

    m_pMsgList = {MSG_OPERATION_ATTR, MSG_OPERATION_TEXT_ADD_ANNOTATION,
                  MSG_OPERATION_TEXT_SHOW_NOTEWIDGET
                 };
}

DocShowShellWidget::~DocShowShellWidget() {}

void DocShowShellWidget::resizeEvent(QResizeEvent *event)
{
    int nState = DataManager::instance()->CurShowState();
    if (nState == FILE_NORMAL) {
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
}

//  注释窗口
void DocShowShellWidget::onOpenNoteWidget(const QString &msgContent)
{
    auto pWidget = this->findChild<FileViewNoteWidget *>();
    if (pWidget == nullptr) {
        pWidget = new FileViewNoteWidget(this);
    }
    pWidget->setEditText("");
    pWidget->setPointAndPage(msgContent);
    pWidget->setNoteUuid("");
    pWidget->setNotePage("");

    QPoint point;
    bool t_bHigh = false;
    DataManager::instance()->setSmallNoteWidgetSize(pWidget->size());
    DataManager::instance()->mousePressLocal(t_bHigh, point);
    pWidget->showWidget(point);
}

//  显示 当前 注释
void DocShowShellWidget::onShowNoteWidget(const QString &contant)
{
    QStringList t_strList = contant.split(QString("%"), QString::SkipEmptyParts);
    if (t_strList.count() == 3) {
        QString t_strUUid = t_strList.at(0);
        QString t_page = t_strList.at(2);

        QString sContant = "";

        auto pHelper = DocummentFileHelper::instance();
        if (pHelper) {
            pHelper->getAnnotationText(t_strUUid, sContant, t_page.toInt());
        }

        auto pWidget = this->findChild<FileViewNoteWidget *>();
        if (pWidget == nullptr) {
            pWidget = new FileViewNoteWidget(this);
        }
        pWidget->setNoteUuid(t_strUUid);
        pWidget->setNotePage(t_page);
        pWidget->setEditText(sContant);
        pWidget->setPointAndPage("");
        DataManager::instance()->setSmallNoteWidgetSize(pWidget->size());

        bool t_bHigh = false;  // 点击位置是否是高亮
        QPoint point;          // = this->mapToGlobal(rrect.bottomRight());// 鼠标点击位置

        DataManager::instance()->mousePressLocal(t_bHigh, point);
        if (t_bHigh) {
            pWidget->showWidget(point);
        }
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

void DocShowShellWidget::slotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_ATTR) { //  打开该文件的属性信息
        onShowFileAttr();
    } else if (msgType == MSG_OPERATION_TEXT_ADD_ANNOTATION) {   //  添加注释
        onOpenNoteWidget(msgContent);
    } else if (msgType ==  MSG_OPERATION_TEXT_SHOW_NOTEWIDGET) { //  显示注释窗口
        onShowNoteWidget(msgContent);
    }
}

void DocShowShellWidget::initConnections()
{
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
    if (KeyStr::g_ctrl_f == msgContent) {  //  搜索
        emit sigShowFileFind();
        return ConstantMsg::g_effective_res;
    }

    if (KeyStr::g_f11 == msgContent && DataManager::instance()->CurShowState() != FILE_SLIDE) { //  全屏
        if (DataManager::instance()->CurShowState() == FILE_FULLSCREEN)
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
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto m_pfileviwewidget = new FileViewWidget(this);
    connect(m_pfileviwewidget, SIGNAL(sigShowPlayCtrl(bool)), this, SLOT(slotChangePlayCtrlShow(bool)));

    layout->addWidget(m_pfileviwewidget);

    this->setLayout(layout);
}
