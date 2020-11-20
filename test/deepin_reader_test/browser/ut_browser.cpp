#include "ut_browser.h"

#define private public
#define protected public

#include "MainWindow.h"
#include "ut_defines.h"
#include "DocSheet.h"
#include "Central.h"
#include "CentralDocPage.h"
#include "BrowserPage.h"
#include "SheetBrowser.h"
#include "sidebar/SheetSidebar.h"

#undef private
#undef protected

ut_browser::ut_browser()
{

}

void ut_browser::SetUp()
{
}

void ut_browser::TearDown()
{
}

#ifdef UT_BROWSER_TEST
TEST_F(ut_browser, SheetBrowserTest)
{
    MainWindow *window = MainWindow::createWindow(QStringList() << UT_FILE_PDF);

    DocSheet *sheet = window->m_central->m_docPage->getSheet(UT_FILE_PDF);
    EXPECT_TRUE(sheet);
    sheet->setSidebarVisible(true);
    sheet->m_sidebar->onBtnClicked(1);

    DocSheet *sheet2 = new DocSheet(Dr::PDF, UT_FILE_PDF);

    sheet2->openFileExec("");

    EXPECT_TRUE(sheet->m_browser->isUnLocked());

    sheet->m_browser->loadPages(sheet->m_operation, sheet->m_bookmarks);

    sheet->m_browser->getClickAnnot(sheet->m_browser->m_items.at(0), QPointF(0, 0), false);

    sheet->m_browser->moveIconAnnot(sheet->m_browser->m_items.at(0), QPointF(0, 0));

    sheet->m_browser->m_items.at(0)->getWords();


}
#endif
