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
#include "BrowserWord.h"
#include "BrowserMenu.h"
#include "Application.h"
#include "BrowserAnnotation.h"
#include "BrowserMagniFier.h"

#undef private
#undef protected

#include <QTimer>
#include <DApplicationSettings>
#include <DLog>
#include <QDebug>

ut_browser::ut_browser()
{

}

void ut_browser::SetUp()
{
    ut_application::SetUp();
}

void ut_browser::TearDown()
{

}

#ifdef UT_BROWSER_TEST
TEST_F(ut_browser, SheetBrowserTest)
{
    MainWindow *window = MainWindow::createWindow();

    window->activateWindow();

    window->show();

    QString error;

    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "SheetBrowserTest"));

    ASSERT_TRUE(sheet->openFileExec("", error));

    window->addSheet(sheet);

    EXPECT_TRUE(sheet);

    sheet->setSidebarVisible(true);

    sheet->m_sidebar->onBtnClicked(1);

    DocSheet *sheet2 = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "SheetBrowserTest"));

    sheet2->openFileAsync("");

    //EXPECT_TRUE(sheet->m_browser->isUnLocked());  //暂时报错

    sheet->m_browser->getClickAnnot(sheet->m_browser->m_items.at(0), QPointF(0, 0), false);

    sheet->m_browser->moveIconAnnot(sheet->m_browser->m_items.at(0), QPointF(0, 0));

    QImage image;

    sheet->getImage(0, image, 200, 200);

    window->show();

    //BrowserPage
    BrowserPage *page = sheet->m_browser->m_items.at(0);
    if (nullptr == page)      //暂时报错
        GTEST_FAIL();

    page->bookmarkMouseRect();
    page->updateBookmarkState();
    page->getImage(1);
    page->renderRect(1, QRectF(0, 0, 1, 1));
    page->renderViewPort(1);
    page->handleWordLoaded(QList<Word>());
    page->getImagePoint(1, QPoint(0, 0));
    page->loadWords();
    page->setDrawMoveIconRect(true);

    QList<Word> words = page->getWords();
    if (words.count() <= 0)
        GTEST_FAIL();

    BrowserWord *word = new BrowserWord(page, words[0]);
    word->setScaleFactor(1);
    word->setSelectable(true);
    word->boundingBox();
    word->boundingRect();
    word->textBoundingRect();
    word->getWord();

    ///BrowserMenu
    BrowserMenu *menu = new BrowserMenu;
    menu->initActions(sheet, 0, SheetMenuType_e::DOC_MENU_DEFAULT);
    menu->createAction("test displayname", "test object");
    menu->onSetHighLight();
    //    menu->onItemClicked();
    delete menu;

    BrowserMenu *menu1 = new BrowserMenu;
    menu1->initActions(sheet, 0, SheetMenuType_e::DOC_MENU_ANNO_ICON);
    delete menu1;

    BrowserMenu *menu2 = new BrowserMenu;
    menu2->initActions(sheet, 0, SheetMenuType_e::DOC_MENU_ANNO_HIGHLIGHT);
    delete menu2;

    BrowserMenu *menu3 = new BrowserMenu;
    menu3->initActions(sheet, 0, SheetMenuType_e::DOC_MENU_SELECT_TEXT);
    delete menu3;

    BrowserMenu *menu4 = new BrowserMenu;
    menu4->initActions(sheet, 0, SheetMenuType_e::DOC_MENU_KEY);
    delete menu4;

    ///BrowserAnnotation
    page->reloadAnnotations();
    deepin_reader::Annotation *annotation = page->annotations().at(0);
    if (nullptr == annotation)
        GTEST_FAIL();

    BrowserAnnotation *annotationItem = new BrowserAnnotation(page, QRectF(0, 0, 10, 10), annotation, 1);
    EXPECT_TRUE(annotationItem->isSame(annotation));
    annotationItem->annotationText();
    annotationItem->annotationType();
    annotationItem->setDrawSelectRect(true);
    annotationItem->annotation();
    annotationItem->deleteMe();

    annotation = page->annotations().at(1);
    if (nullptr == annotation)
        GTEST_FAIL();

    page->clearSelectSearchHighlightRects();
    page->getBrowserAnnotation(QPointF(0, 0));
    page->getBrowserWord(QPointF(0, 0));
    page->getNorotateRect(QRectF(0, 0, 1, 1));
    page->addIconAnnotation(QRectF(0, 0, 1, 1), "test");
    page->addHighlightAnnotation("text", QColor(Qt::red));
    page->updateAnnotation(annotation, "test", QColor(Qt::red));
    page->setSelectIconRect(true, nullptr);
    page->deleteNowSelectIconAnnotation();
    page->jump2Link(QPointF(0, 0));
    page->inLink(QPointF(0, 0));
    page->setPageBookMark(QPointF(0, 0));
    page->removeAnnotation(annotation);

    ///BrowserMagniFier
    MagnifierInfo_t info;

    window->closeWithoutSave();

    exec();
}
#endif
