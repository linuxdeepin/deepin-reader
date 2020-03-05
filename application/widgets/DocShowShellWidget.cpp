#include "DocShowShellWidget.h"

#include <DDialogCloseButton>
#include <QDesktopWidget>
#include <QStackedLayout>

#include "FileViewWidget.h"
#include "FindWidget.h"
#include "FileAttrWidget.h"

#include "MsgModel.h"

#include "docview/docummentproxy.h"
#include "utils/PublicFunction.h"

#include "pdfControl/note/NoteViewWidget.h"
#include "widgets/main/MainTabWidgetEx.h"

DocShowShellWidget::DocShowShellWidget(CustomWidget *parent)
    : CustomWidget("DocShowShellWidget", parent)
{

    initWidget();
    initConnections();

    m_pMsgList = { MSG_OPERATION_TEXT_ADD_ANNOTATION,
                   MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, MSG_NOTE_PAGE_SHOW_NOTEWIDGET
                 };

    dApp->m_pModelService->addObserver(this);
}

DocShowShellWidget::~DocShowShellWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

//  注释窗口
void DocShowShellWidget::onOpenNoteWidget(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 3) {

        QString sPage = sList.at(0);
//        QString sX = sList.at(1);
//        QString sY = sList.at(2);

        if (m_pNoteViewWidget == nullptr) {
            m_pNoteViewWidget = new NoteViewWidget(this);
        }
        m_pNoteViewWidget->setEditText("");
        m_pNoteViewWidget->setNoteUuid("");
        m_pNoteViewWidget->setNotePage(sPage);
        m_pNoteViewWidget->setWidgetType(NOTE_HIGHLIGHT);

        QPoint point;
        bool t_bHigh = false;
        dApp->m_pAppInfo->setSmallNoteWidgetSize(m_pNoteViewWidget->size());
        dApp->m_pAppInfo->mousePressLocal(t_bHigh, point);
        m_pNoteViewWidget->showWidget(point);
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

        auto pHelper = MainTabWidgetEx::Instance()->getCurFileAndProxy();
        if (pHelper) {
            pHelper->getAnnotationText(t_strUUid, sContant, t_page.toInt());
        }

        if (m_pNoteViewWidget == nullptr) {
            m_pNoteViewWidget = new NoteViewWidget(this);
        }
        m_pNoteViewWidget->setNoteUuid(t_strUUid);
        m_pNoteViewWidget->setNotePage(t_page);
        m_pNoteViewWidget->setEditText(sContant);
        m_pNoteViewWidget->setWidgetType(NOTE_HIGHLIGHT);
        dApp->m_pAppInfo->setSmallNoteWidgetSize(m_pNoteViewWidget->size());

        bool t_bHigh = false;  // 点击位置是否是高亮
        QPoint point;          // = this->mapToGlobal(rrect.bottomRight());// 鼠标点击位置

        dApp->m_pAppInfo->mousePressLocal(t_bHigh, point);
        m_pNoteViewWidget->showWidget(point);
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

        auto pHelper = MainTabWidgetEx::Instance()->getCurFileAndProxy();
        if (pHelper) {
            pHelper->getAnnotationText(sUuid, sContant, sPage.toInt());
        }
        if (m_pNoteViewWidget == nullptr) {
            m_pNoteViewWidget = new NoteViewWidget(this);
        }
        m_pNoteViewWidget->setEditText(sContant);
        m_pNoteViewWidget->setNoteUuid(sUuid);
        m_pNoteViewWidget->setNotePage(sPage);
        m_pNoteViewWidget->setWidgetType(NOTE_ICON);
        m_pNoteViewWidget->showWidget(QPoint(sX.toInt(), sY.toInt()));
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

void DocShowShellWidget::slotUpdateTheme()
{
}

void DocShowShellWidget::SlotOpenFileOk()
{
    m_playout->removeWidget(m_pSpinerWidget);

    delete  m_pSpinerWidget;
    m_pSpinerWidget = nullptr;

    emit sigOpenFileOk();
}

void DocShowShellWidget::SlotFindOperation(const int &iType, const QString &strFind)
{
    if (m_pFileViewWidget) {
        m_pFileViewWidget->SetFindOperation(iType, strFind);
    }
}

void DocShowShellWidget::initConnections()
{
}

//  集中处理 按键通知消息
int DocShowShellWidget::dealWithNotifyMsg(const QString &msgContent)
{
    return MSG_NO_OK;
}

int DocShowShellWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    int nRes = m_pFileViewWidget->dealWithData(msgType, msgContent);
    if (nRes != MSG_OK) {
        if (msgType == MSG_OPERATION_TEXT_ADD_ANNOTATION) {             //  添加注释
            onOpenNoteWidget(msgContent);
        } else if (msgType == MSG_OPERATION_TEXT_SHOW_NOTEWIDGET) {
            onShowNoteWidget(msgContent);
        } else if (msgType == MSG_NOTE_PAGE_SHOW_NOTEWIDGET) {          //  显示注释窗口
            __ShowPageNoteWidget(msgContent);
        } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
            slotUpdateTheme();
        } else if (msgType == MSG_NOTIFY_KEY_MSG) {
            //  最后一个处理通知消息
            return dealWithNotifyMsg(msgContent);
        }

        if (m_pMsgList.contains(msgType)) {
            return  MSG_OK;
        }
    }

    return nRes;
}

bool DocShowShellWidget::OpenFilePath(const QString &sPath)
{
    return m_pFileViewWidget->OpenFilePath(sPath);
}

void DocShowShellWidget::ShowFindWidget()
{
    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(this);
        connect(m_pFindWidget, SIGNAL(sigFindOperation(const int &, const QString &)), SLOT(SlotFindOperation(const int &, const QString &)));
    }

    int nParentWidth = this->width();
    m_pFindWidget->showPosition(nParentWidth);
    m_pFindWidget->setSearchEditFocus();
}

void DocShowShellWidget::initWidget()
{
    m_playout = new QStackedLayout;
    m_playout->setContentsMargins(0, 0, 0, 0);
    m_playout->setSpacing(0);

    m_pFileViewWidget = new FileViewWidget(this);
    connect(m_pFileViewWidget, SIGNAL(sigFileOpenOK()), SLOT(SlotOpenFileOk()));

    InitSpinner();
    m_playout->addWidget(m_pSpinerWidget);

    m_playout->addWidget(m_pFileViewWidget);

    this->setLayout(m_playout);
}

void DocShowShellWidget::resizeEvent(QResizeEvent *event)
{
    if (m_pFindWidget && m_pFindWidget->isVisible()) {
        int nParentWidth = this->width();
        m_pFindWidget->showPosition(nParentWidth);
    }

    CustomWidget::resizeEvent(event);
}

