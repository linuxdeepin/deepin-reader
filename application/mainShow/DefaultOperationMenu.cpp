#include "DefaultOperationMenu.h"
#include <DPushButton>
#include "controller/DataManager.h"
#include "controller/MsgSubject.h"
#include "subjectObserver/MsgHeader.h"
#include "docview/docummentproxy.h"
#include "application.h"

DefaultOperationMenu::DefaultOperationMenu(DWidget *parent)
    : DMenu (parent)
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
        int nPageNum = DocummentProxy::instance()->getPageSNum();
        nPageNum--;

        if (m_nRightPageNumber == nPageNum) { //  最后一页
            m_pNextPage->setEnabled(false);
            m_pEndPage->setEnabled(false);
        }
    }

    this->exec(showPoint);
}

void DefaultOperationMenu::initMenu()
{
    createAction(tr("Search"), SLOT(slotSearchClicked()));
    m_pBookMark = createAction(tr("add bookmark"), SLOT(slotBookMarkClicked()));
    m_pFirstPage = createAction(tr("first page"), SLOT(slotFirstPageClicked()));
    m_pPrevPage = createAction(tr("prev page"), SLOT(slotPrevPageClicked()));
    m_pNextPage = createAction(tr("next page"), SLOT(slotNextPageClicked()));
    m_pEndPage = createAction(tr("end page"), SLOT(slotEndPageClicked()));
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
    MsgSubject::getInstance()->sendMsg(nullptr, msgType, msgContent);
}

void DefaultOperationMenu::slotSearchClicked()
{
    sendMsgToFrame(MSG_OPERATION_FIND);
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
    sendMsgToFrame(MSG_OPERATION_FIRST_PAGE);
}

void DefaultOperationMenu::slotPrevPageClicked()
{
    sendMsgToFrame(MSG_OPERATION_PREV_PAGE);
}

void DefaultOperationMenu::slotNextPageClicked()
{
    sendMsgToFrame(MSG_OPERATION_NEXT_PAGE);
}

void DefaultOperationMenu::slotEndPageClicked()
{
    sendMsgToFrame(MSG_OPERATION_END_PAGE);
}
