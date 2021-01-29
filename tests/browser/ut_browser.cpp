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
#include <QGraphicsSceneMouseEvent>
#include <QRectF>
#include <QGestureEvent>
#include <QTimerEvent>

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

    sheet->m_browser->properties();

    sheet->m_browser->getClickAnnot(sheet->m_browser->m_items.at(0), QPointF(0, 0), false);

    sheet->m_browser->moveIconAnnot(sheet->m_browser->m_items.at(0), QPointF(0, 0));

    QRectF iconRect = QRectF(0, 0, 1, 1.0);

    sheet->m_browser->calcIconAnnotRect(sheet->m_browser->m_items.at(0), QPointF(0, 0), iconRect);

    QPoint showPoint = QPoint(1, 1);

    Annotation *hightLightAnnot = sheet->m_browser->addHighLightAnnotation("123", QColor(Qt::red), showPoint);

    sheet->m_browser->jumpToHighLight(hightLightAnnot, 0);

    sheet->m_browser->onAddHighLightAnnot(nullptr, "123", QColor(Qt::red));

    //firstThumbnail

    //save

    //aveas

    sheet->m_browser->setMouseShape(Dr::MouseShapeHand);

    sheet->m_browser->setBookMark(0, 1);

    sheet->m_browser->setAnnotationInserting(true);

    sheet->m_browser->onVerticalScrollBarValueChanged(1);

    sheet->m_browser->onHorizontalScrollBarValueChanged(1);

    sheet->m_browser->beginViewportChange();

    sheet->m_browser->hideSubTipsWidget();

    sheet->m_browser->onViewportChanged();

    sheet->m_browser->showNoteEditWidget(hightLightAnnot, QPoint(0, 0));

    sheet->m_browser->jump2PagePos(sheet->m_browser->m_items.at(0), 1, 2);

    sheet->m_browser->jumpToOutline(0, 0, 0);

    sheet->m_browser->onRemoveAnnotation(hightLightAnnot, true);

    QShowEvent *showEvent = new QShowEvent;

    sheet->m_browser->showEvent(showEvent);

    QGestureEvent *gestureEvent = new QGestureEvent(QList<QGesture *>());

    sheet->m_browser->gestureEvent(gestureEvent);

    QTimerEvent *timerEvent = new QTimerEvent(123);

    sheet->m_browser->timerEvent(timerEvent);

    sheet->m_browser->hasLoaded();

    sheet->m_browser->openMagnifier();

    sheet->m_browser->magnifierOpened();

    sheet->m_browser->closeMagnifier();

    sheet->m_browser->maxWidth();

    sheet->m_browser->maxHeight();

    sheet->m_browser->needBookmark(0, 1);

    sheet->m_browser->handleSearch();

    sheet->m_browser->handleFindNext();

    sheet->m_browser->handleFindPrev();

    sheet->m_browser->handleFindContent("aaaaaaaaaaaaaaaa");

    sheet->m_browser->handleFindExit();

    sheet->m_browser->handleFindFinished(123);

    sheet->m_browser->stopSearch();

    sheet->m_browser->setDocTapGestrue(QPoint(0, 0));

    sheet->m_browser->clearSelectIconAnnotAfterMenu();

    sheet->m_browser->isLink(QPoint(0, 0));

    sheet->m_browser->setIconAnnotSelect(true);

    sheet->m_browser->onSetDocSlideGesture();

    sheet->m_browser->onRemoveIconAnnotSelect();

    sheet->m_browser->curpageChanged(0);

    sheet->m_browser->outline();

    sheet->m_browser->loadPageLable();

    sheet->m_browser->pageLableIndex("1");

    sheet->m_browser->pageHasLable();

    sheet->m_browser->pageSizeByIndex(0);

    sheet->m_browser->pageNum2Lable(0);

    sheet->m_browser->setCurrentPage(0);

    sheet->m_browser->currentPage();

    sheet->m_browser->allPages();

    sheet->m_browser->translate2Local(QPointF(0, 0));

    QPointF pagePoint = QPointF(0, 0);

    sheet->m_browser->getBrowserPageForPoint(pagePoint);

    sheet->m_browser->viewPointInIndex(QPoint(0, 0));

    sheet->m_browser->currentScrollValueForPage();

    int fromIndex = 0;
    int toIndex = 1;
    sheet->m_browser->currentIndexRange(fromIndex, toIndex);

    sheet->m_browser->selectedWordsText();

    //sheet->m_browser->showMenu();     //阻塞

    sheet->m_browser->deform(sheet->m_operation);

    QEvent *event = new QEvent(QEvent::Timer);

    sheet->m_browser->event(event);

    QMouseEvent *pressMouseEvent = new QMouseEvent(QEvent::MouseButtonPress, QPointF(0, 0), QPoint(0, 0), QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    sheet->m_browser->mousePressEvent(pressMouseEvent);

    QMouseEvent *releaseMouseEvent = new QMouseEvent(QEvent::MouseButtonRelease, QPointF(0, 0), QPoint(0, 0), QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    sheet->m_browser->mouseReleaseEvent(releaseMouseEvent);

    QMouseEvent *mouseEvent = new QMouseEvent(QEvent::Move, QPointF(0, 0), QPointF(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    sheet->m_browser->mouseMoveEvent(mouseEvent);

    QDragEnterEvent *dragEnterEvent = new QDragEnterEvent(QPoint(0, 0), Qt::CopyAction, new QMimeData, Qt::LeftButton, Qt::NoModifier);

    sheet->m_browser->dragEnterEvent(dragEnterEvent);

    QResizeEvent *resizeEvent = new QResizeEvent(QSize(800, 600), QSize(400, 300));

    sheet->m_browser->resizeEvent(resizeEvent);

    QPinchGesture *pinchGesture = new QPinchGesture;

    sheet->m_browser->pinchTriggered(pinchGesture);

    QWheelEvent *wheelEvent = new QWheelEvent(QPointF(0, 0), 1, Qt::LeftButton, Qt::NoModifier);

    sheet->m_browser->wheelEvent(wheelEvent);

    window->show();

    //BrowserPage
    BrowserPage *page = sheet->m_browser->m_items.at(0);
    if (nullptr == page) //暂时报错
        GTEST_FAIL();

    page->bookmarkMouseRect();
    page->updateBookmarkState();
    page->renderRect(QRectF(0, 0, 1, 1));
    page->renderViewPort();
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

    QGraphicsSceneMouseEvent *gsMouseEvent = new QGraphicsSceneMouseEvent;

    word->mousePressEvent(gsMouseEvent);

    word->mouseReleaseEvent(gsMouseEvent);

    ///BrowserMenu
    BrowserMenu *menu = new BrowserMenu;
    menu->initActions(sheet, 0, SheetMenuType_e::DOC_MENU_DEFAULT);
    menu->createAction("test displayname", "test object");
    menu->onSetHighLight();
    menu->show();
    menu->hide();
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

    page->selectedWords();
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
    page->removeAllAnnotation();

    ///BrowserMagniFier
    MagnifierInfo_t info;

    window->closeWithoutSave();

    exec();
}
