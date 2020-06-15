#include "DefaultOperationMenu.h"
#include "app/AppInfo.h"
#include "pdfControl/docview/DocummentProxy.h"
#include "pdfControl/DocSheetPDF.h"
#include "ModuleHeader.h"
#include "MsgHeader.h"

DefaultOperationMenu::DefaultOperationMenu(DWidget *parent)
    : CustomMenu(parent)
{
    initActions();
}

DefaultOperationMenu::~DefaultOperationMenu()
{
    if (nullptr != m_pSearch)
        delete m_pSearch;
    if (nullptr != m_pBookMark)
        delete m_pBookMark;
    if (nullptr != m_pAddIconNote)
        delete m_pAddIconNote;
    if (nullptr != m_pFirstPage)
        delete m_pFirstPage;
    if (nullptr != m_pPrevPage)
        delete m_pPrevPage;
    if (nullptr != m_pNextPage)
        delete m_pNextPage;
    if (nullptr != m_pEndPage)
        delete m_pEndPage;
}

void DefaultOperationMenu::execMenu(DocSheetPDF *sheet, const QPoint &showPoint, const int &nClickPage)
{
    m_sheet = sheet;

    if (m_sheet.isNull())
        return;

    m_showPoint = showPoint;

    m_nRightPageNumber = nClickPage;

    QString sCurPath = m_sheet->filePath();

    QSet<int> pageList = m_sheet->getBookMarkList();

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

    if (!m_sheet->isOpen()) {
        m_pFirstPage->setEnabled(false);
        m_pPrevPage->setEnabled(false);
        m_pNextPage->setEnabled(false);
        m_pEndPage->setEnabled(false);
        return;
    } else {
        int currentPage = 0;
        int pageSum = 0;
        bool isSinglePage = false;//文档总页数是否是单页

        pageSum = m_sheet->pagesNumber();
        currentPage = m_sheet->currentIndex();
        isSinglePage = static_cast<bool>(pageSum % 2);

        if (currentPage == 0) { //  首页
            m_pFirstPage->setEnabled(false);
            m_pPrevPage->setEnabled(false);
        } else {
            pageSum--;
            if ((!m_sheet->isDoubleShow()) ? (currentPage == pageSum) : (isSinglePage ? (currentPage >= pageSum) : (currentPage >= (--pageSum)))) { //  最后一页
                m_pNextPage->setEnabled(false);
                m_pEndPage->setEnabled(false);
            }
        }
        if (m_sheet->pagesNumber() == 1) {
            m_pFirstPage->setEnabled(false);
            m_pPrevPage->setEnabled(false);
            m_pNextPage->setEnabled(false);
            m_pEndPage->setEnabled(false);
        }

        m_pSearch->setVisible(true);

        m_pAddIconNote->setVisible(true);

        this->exec(showPoint);
    }
}

void DefaultOperationMenu::initActions()
{
    m_pSearch = createAction(tr("Search"), SLOT(slotSearchClicked()));

    this->addSeparator();

    m_pBookMark = createAction(tr("Add bookmark"), SLOT(slotBookMarkClicked()));

    m_pAddIconNote = createAction(tr("Add annotation"), SLOT(slotAddIconNote()));

    this->addSeparator();

    m_pFirstPage = createAction(tr("First page"), SLOT(slotFirstPageClicked()));

    m_pPrevPage = createAction(tr("Previous page"), SLOT(slotPrevPageClicked()));

    m_pNextPage = createAction(tr("Next page"), SLOT(slotNextPageClicked()));

    m_pEndPage = createAction(tr("Last page"), SLOT(slotEndPageClicked()));
}

QAction *DefaultOperationMenu::createAction(const QString &name, const char *member)
{
    auto action = new QAction(name);

    connect(action, SIGNAL(triggered()), member);

    this->addAction(action);

    return action;
}

void DefaultOperationMenu::slotSearchClicked()
{
    m_sheet->handleSearch();
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
    m_sheet->jumpToFirstPage();
}

void DefaultOperationMenu::slotPrevPageClicked()
{
    m_sheet->jumpToPrevPage();
}

void DefaultOperationMenu::slotNextPageClicked()
{
    m_sheet->jumpToNextPage();
}

void DefaultOperationMenu::slotEndPageClicked()
{
    m_sheet->jumpToLastPage();
}

void DefaultOperationMenu::slotAddIconNote()
{
    if (m_sheet.isNull())
        return;

    QString sUuid = m_sheet->addIconAnnotation(m_pointclicked);        //  添加注释图标成功
    if (sUuid != "") {
        int page = m_sheet->pointInWhichPage(m_pointclicked);
        m_sheet->showNoteWidget(page, sUuid, NOTE_ICON);

    }
}
