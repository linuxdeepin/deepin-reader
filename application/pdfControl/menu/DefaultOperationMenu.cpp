#include "DefaultOperationMenu.h"

#include "business/AppInfo.h"
#include "docview/docummentproxy.h"

#include "CentralDocPage.h"

DefaultOperationMenu::DefaultOperationMenu(DWidget *parent)
    : CustomMenu(DEFAULT_OPERATION_MENU, parent)
{
    initActions();
}

void DefaultOperationMenu::execMenu(const QPoint &showPoint, const int &nClickPage)
{
    CentralDocPage *pMtwe = CentralDocPage::Instance();
    if (pMtwe) {

        m_showPoint = showPoint;

        m_nRightPageNumber = nClickPage;

        QString sCurPath = pMtwe->qGetCurPath();

        QList<int> pageList = dApp->m_pDBService->getBookMarkList(sCurPath);

        bool bBookState = pageList.contains(m_nRightPageNumber);

        if (bBookState) {
            m_pBookMark->setProperty("data", 0);
            m_pBookMark->setText(tr("Remove bookmark"));
        } else {
            m_pBookMark->setProperty("data", 1);
            m_pBookMark->setText(tr("Add bookmark"));
        }

        m_pFirstPage->setEnabled(true);
        m_pPrevPage->setEnabled(true);
        m_pNextPage->setEnabled(true);
        m_pEndPage->setEnabled(true);

        if ((m_isDoubleShow == false) ? (m_nRightPageNumber == 0) : (m_nRightPageNumber <= 1)) { //  首页
            m_pFirstPage->setEnabled(false);
            m_pPrevPage->setEnabled(false);
        } else {
            DocummentProxy *_proxy = pMtwe->getCurFileAndProxy(sCurPath);
            if (_proxy) {
                int nPageNum = _proxy->getPageSNum();
                nPageNum--;

                if ((m_isDoubleShow == false) ? (m_nRightPageNumber == nPageNum) : ((m_nRightPageNumber >= (--nPageNum)))) { //  最后一页
                    m_pNextPage->setEnabled(false);
                    m_pEndPage->setEnabled(false);
                }
            }
        }

        m_pExitFullScreen->setVisible(false);
        m_pSearch->setVisible(true);
        m_pAddIconNote->setVisible(true);

        this->exec(showPoint);
    }
}

void DefaultOperationMenu::setDoubleShow(const bool &isDoubleShow)
{
    m_isDoubleShow = isDoubleShow;
}

void DefaultOperationMenu::initActions()
{
    m_pSearch = createAction(tr("Search"), SLOT(slotSearchClicked()));

    m_pBookMark = createAction(tr("Add bookmark"), SLOT(slotBookMarkClicked()));

    m_pAddIconNote = createAction(tr("Add note"), SLOT(slotAddIconNote()));

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

void DefaultOperationMenu::slotSearchClicked()
{
    QJsonObject obj;
    obj.insert("type", "ShortCut");
    obj.insert("key", KeyStr::g_ctrl_f);

    QJsonDocument doc = QJsonDocument(obj);
    notifyMsg(E_APP_MSG_TYPE, doc.toJson(QJsonDocument::Compact));
}

void DefaultOperationMenu::slotBookMarkClicked()
{
    int nData = m_pBookMark->property("data").toInt();
    if (nData == 0) {
        emit sigActionTrigger(MSG_OPERATION_DELETE_BOOKMARK, QString("%1").arg(m_nRightPageNumber));
    } else {
        emit sigActionTrigger(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(m_nRightPageNumber));
    }
}

void DefaultOperationMenu::slotFirstPageClicked()
{
    CentralDocPage::Instance()->qDealWithData(MSG_OPERATION_FIRST_PAGE, "");
}

void DefaultOperationMenu::slotPrevPageClicked()
{
    CentralDocPage::Instance()->qDealWithData(MSG_OPERATION_PREV_PAGE, m_isDoubleShow ? "doubleshow" : "");
}

void DefaultOperationMenu::slotNextPageClicked()
{
    CentralDocPage::Instance()->qDealWithData(MSG_OPERATION_NEXT_PAGE,  m_isDoubleShow ? "doubleshow" : "");
}

void DefaultOperationMenu::slotEndPageClicked()
{
    CentralDocPage::Instance()->qDealWithData(MSG_OPERATION_END_PAGE, "");
}

void DefaultOperationMenu::slotExitFullScreenClicked()
{
    dApp->m_pModelService->notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
}

void DefaultOperationMenu::slotAddIconNote()
{
    DocummentProxy *_proxy = CentralDocPage::Instance()->getCurFileAndProxy();
    QString sUuid = _proxy->addIconAnnotation(m_pointclicked);        //  添加注释图标成功
    if (sUuid != "") {
        int nClickPage = _proxy->pointInWhichPage(m_pointclicked);
        QString strContent = sUuid.trimmed() + Constant::sQStringSep +
                             QString::number(nClickPage) + Constant::sQStringSep +
                             QString::number(m_showPoint.x()) + Constant::sQStringSep +
                             QString::number(m_showPoint.y());

        QJsonObject obj;
        obj.insert("content", strContent);
        obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

        QJsonDocument doc(obj);

        dApp->m_pModelService->notifyMsg(MSG_NOTE_PAGE_SHOW_NOTEWIDGET, doc.toJson(QJsonDocument::Compact));
    }
}
