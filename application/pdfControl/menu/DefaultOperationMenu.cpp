#include "DefaultOperationMenu.h"

#include "business/AppInfo.h"
#include "docview/docummentproxy.h"
#include "DocSheet.h"
#include "ModuleHeader.h"
#include "MsgHeader.h"

DefaultOperationMenu::DefaultOperationMenu(DWidget *parent)
    : CustomMenu(DEFAULT_OPERATION_MENU, parent)
{
    initActions();
}

void DefaultOperationMenu::execMenu(DocSheet *sheet, const QPoint &showPoint, const int &nClickPage)
{
    m_sheet = sheet;

    if (m_sheet.isNull())
        return;

    m_showPoint = showPoint;

    m_nRightPageNumber = nClickPage;

    QString sCurPath = m_sheet->filePath();

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

    DocummentProxy *_proxy = m_sheet->getDocProxy();
    if(_proxy == nullptr){
        m_pFirstPage->setEnabled(false);
        m_pPrevPage->setEnabled(false);
        m_pNextPage->setEnabled(false);
        m_pEndPage->setEnabled(false);
        return;
    }else {
        int currentPage = 0;
        int pageSum = 0;
        bool isSinglePage = false;//文档总页数是否是单页

        pageSum = _proxy->getPageSNum();
        currentPage = _proxy->currentPageNo();
        isSinglePage = static_cast<bool>(pageSum%2);

        if (currentPage == 0/*(!m_sheet->isDoubleShow()) ? (currentPage == 0) : (currentPage <= 1)*/) { //  首页
            m_pFirstPage->setEnabled(false);
            m_pPrevPage->setEnabled(false);
        } else {
            if (_proxy) {
                pageSum--;

                if ((!m_sheet->isDoubleShow()) ? (currentPage == pageSum) : (isSinglePage ? (currentPage >= pageSum) : (currentPage >= (--pageSum)))) { //  最后一页
                    m_pNextPage->setEnabled(false);
                    m_pEndPage->setEnabled(false);
                }
            }
        }
        if(_proxy->getPageSNum() == 1){
            m_pFirstPage->setEnabled(false);
            m_pPrevPage->setEnabled(false);
            m_pNextPage->setEnabled(false);
            m_pEndPage->setEnabled(false);
        }

        m_pExitFullScreen->setVisible(false);
        m_pSearch->setVisible(true);
        m_pAddIconNote->setVisible(true);

        this->exec(showPoint);
    }
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
    m_sheet->handleShortcut(KeyStr::g_ctrl_f);
}

void DefaultOperationMenu::slotBookMarkClicked()
{
    int nData = m_pBookMark->property("data").toInt();
    if (nData == 0) {
        m_sheet->setBookMark(m_nRightPageNumber, false);
    } else {
        m_sheet->setBookMark(m_nRightPageNumber, true);
    }
}

void DefaultOperationMenu::slotFirstPageClicked()
{
    m_sheet->pageFirst();
}

void DefaultOperationMenu::slotPrevPageClicked()
{
    m_sheet->pagePrev();
}

void DefaultOperationMenu::slotNextPageClicked()
{
    m_sheet->pageNext();
}

void DefaultOperationMenu::slotEndPageClicked()
{
    m_sheet->pageLast();
}

void DefaultOperationMenu::slotExitFullScreenClicked()
{
    dApp->m_pModelService->notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
}

void DefaultOperationMenu::slotAddIconNote()
{
    if (m_sheet.isNull())
        return;

    DocummentProxy *_proxy = m_sheet->getDocProxy();

    QString sUuid = _proxy->addIconAnnotation(m_pointclicked);        //  添加注释图标成功

    if (sUuid != "") {
        int page = _proxy->pointInWhichPage(m_pointclicked);

        m_sheet->showNoteWidget(page,sUuid,NOTE_ICON);

    }
}
