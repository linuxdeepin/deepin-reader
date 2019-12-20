#include "DefaultOperationMenu.h"
#include <DPushButton>
#include "controller/DataManager.h"
#include "controller/MsgSubject.h"
#include "controller/NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include "application.h"
#include "frame/DocummentFileHelper.h"
#include <DFontSizeManager>
#include "subjectObserver/ModuleHeader.h"

DefaultOperationMenu::DefaultOperationMenu(DWidget *parent)
    : DMenu(parent)
{
    initMenu();
}

void DefaultOperationMenu::execMenu(const QPoint &showPoint, const int &nClickPage)
{
    m_nRightPageNumber = nClickPage;

    QList<int> pageList = dApp->dbM->getBookMarkList();
    bool bBookState = pageList.contains(m_nRightPageNumber);
    if (bBookState) {
        m_pBookMark->setProperty("data", 0);
        m_pBookMark->setText(tr("delete bookmark"));
    } else {
        m_pBookMark->setProperty("data", 1);
        m_pBookMark->setText(tr("add bookmark"));
    }

    if (m_nRightPageNumber == 0) {    //  首页
        m_pFirstPage->setEnabled(false);
        m_pPrevPage->setEnabled(false);
    } else {
        int nPageNum = DocummentFileHelper::instance()->getPageSNum();
        nPageNum--;

        if (m_nRightPageNumber == nPageNum) { //  最后一页
            m_pNextPage->setEnabled(false);
            m_pEndPage->setEnabled(false);
        }
    }

    //  当前显示状态状态
    int nState = DataManager::instance()->CurShowState();
    if (nState == FILE_FULLSCREEN) {
        m_pExitFullScreen->setVisible(true);
        m_pSearch->setVisible(false);
    } else {
        m_pExitFullScreen->setVisible(false);
        m_pSearch->setVisible(true);
    }
    this->exec(showPoint);
}

void DefaultOperationMenu::initMenu()
{
    DFontSizeManager::instance()->bind(this, DFontSizeManager::T6);
    m_pSearch = createAction(tr("Search"), SLOT(slotSearchClicked()));
    addSeparator();
    m_pBookMark = createAction(tr("add bookmark"), SLOT(slotBookMarkClicked()));
    addSeparator();
    m_pFirstPage = createAction(tr("first page"), SLOT(slotFirstPageClicked()));
    addSeparator();
    m_pPrevPage = createAction(tr("prev page"), SLOT(slotPrevPageClicked()));
    addSeparator();
    m_pNextPage = createAction(tr("next page"), SLOT(slotNextPageClicked()));
    addSeparator();
    m_pEndPage = createAction(tr("end page"), SLOT(slotEndPageClicked()));
    addSeparator();
    m_pExitFullScreen = createAction(tr("exit fullscreen"), SLOT(slotExitFullScreenClicked()));
}

QAction *DefaultOperationMenu::createAction(const QString &name, const char *member)
{
    auto action = new QAction(name);

    connect(action, SIGNAL(triggered()), member);

    this->addAction(action);

    return action;
}

void DefaultOperationMenu::sendMsgToFrame(const int &msgType, const QString &msgContent)
{
    MsgSubject::getInstance()->sendMsg(msgType, msgContent);
}

void DefaultOperationMenu::notifyMsgToFrame(const int &msgType, const QString &msgContent)
{
    NotifySubject::getInstance()->notifyMsg(msgType, msgContent);
}

void DefaultOperationMenu::slotSearchClicked()
{
    notifyMsgToFrame(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_f);
}

void DefaultOperationMenu::slotBookMarkClicked()
{
    int nData = m_pBookMark->property("data").toInt();
    if (nData == 0) {
        sendMsgToFrame(MSG_OPERATION_DELETE_BOOKMARK, QString("%1").arg(m_nRightPageNumber));
    } else {
        sendMsgToFrame(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(m_nRightPageNumber));
    }
}

void DefaultOperationMenu::slotFirstPageClicked()
{
    notifyMsgToFrame(MSG_OPERATION_FIRST_PAGE);
}

void DefaultOperationMenu::slotPrevPageClicked()
{
    notifyMsgToFrame(MSG_OPERATION_PREV_PAGE);
}

void DefaultOperationMenu::slotNextPageClicked()
{
    notifyMsgToFrame(MSG_OPERATION_NEXT_PAGE);
}

void DefaultOperationMenu::slotEndPageClicked()
{
    notifyMsgToFrame(MSG_OPERATION_END_PAGE);
}

void DefaultOperationMenu::slotExitFullScreenClicked()
{
    notifyMsgToFrame(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
}
