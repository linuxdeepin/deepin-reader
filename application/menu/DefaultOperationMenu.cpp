#include "DefaultOperationMenu.h"
#include <DPushButton>
#include "application.h"
#include <DFontSizeManager>

#include "docview/docummentproxy.h"

#include "controller/DataManager.h"
#include "controller/NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include "subjectObserver/ModuleHeader.h"

DefaultOperationMenu::DefaultOperationMenu(DWidget *parent)
    : DMenu(parent)
{
    initMenu();
}

void DefaultOperationMenu::execMenu(const QPoint &showPoint, const int &nClickPage)
{
    m_nRightPageNumber = nClickPage;

    QList<int> pageList = dApp->m_pDBService->getBookMarkList();
    bool bBookState = pageList.contains(m_nRightPageNumber);
    if (bBookState) {
        m_pBookMark->setProperty("data", 0);
        m_pBookMark->setText(tr("Remove bookmark"));
    } else {
        m_pBookMark->setProperty("data", 1);
        m_pBookMark->setText(tr("Add bookmark"));
    }

    if (m_nRightPageNumber == 0) {    //  首页
        m_pFirstPage->setEnabled(false);
        m_pPrevPage->setEnabled(false);
    } else {
        DocummentProxy *_proxy = DocummentProxy::instance();
        if (_proxy) {
            int nPageNum = _proxy->getPageSNum();
            nPageNum--;

            if (m_nRightPageNumber == nPageNum) { //  最后一页
                m_pNextPage->setEnabled(false);
                m_pEndPage->setEnabled(false);
            }
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

    m_pBookMark = createAction(tr("Add bookmark"), SLOT(slotBookMarkClicked()));

    m_pFirstPage = createAction(tr("First page"), SLOT(slotFirstPageClicked()));

    m_pPrevPage = createAction(tr("Previous page"), SLOT(slotPrevPageClicked()));

    m_pNextPage = createAction(tr("Next page"), SLOT(slotNextPageClicked()));

    m_pEndPage = createAction(tr("Last page"), SLOT(slotEndPageClicked()));

    m_pExitFullScreen = createAction(tr("Exit fullscreen"), SLOT(slotExitFullScreenClicked()));
}

QAction *DefaultOperationMenu::createAction(const QString &name, const char *member)
{
    auto action = new QAction(name);

    connect(action, SIGNAL(triggered()), member);

    this->addAction(action);
    this->addSeparator();

    return action;
}

void DefaultOperationMenu::notifyMsgToFrame(const int &msgType, const QString &msgContent)
{
    g_NotifySubject::getInstance()->notifyMsg(msgType, msgContent);
}

void DefaultOperationMenu::slotSearchClicked()
{
    notifyMsgToFrame(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_f);
}

void DefaultOperationMenu::slotBookMarkClicked()
{
    int nData = m_pBookMark->property("data").toInt();
    if (nData == 0) {
        notifyMsgToFrame(MSG_OPERATION_DELETE_BOOKMARK, QString("%1").arg(m_nRightPageNumber));
    } else {
        notifyMsgToFrame(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(m_nRightPageNumber));
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
