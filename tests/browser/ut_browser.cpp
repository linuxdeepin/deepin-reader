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
#include "SheetRenderer.h"
#include "PageRenderThread.h"
#include "SideBarImageViewModel.h"
#include "PageSearchThread.h"

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

TEST_F(ut_browser, BrowserAnnotationTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "BrowserAnnotationTest"));

    EXPECT_TRUE(sheet->openFileExec(""));

    BrowserPage *page = sheet->m_browser->pages().value(0);

    EXPECT_TRUE(page);

    BrowserAnnotation *annot = new BrowserAnnotation(page, QRectF(20, 20, 20, 20), sheet->annotations().value(0), 1);

    annot->setScaleFactor(1);

    annot->annotationText();

    annot->annotationType();

    annot->boundingRect();

    annot->setDrawSelectRect(true);

    annot->annotation();

    annot->isSame(nullptr);

    QPixmap pixmap(200, 200);

    QPainter painter(&pixmap);

    QStyleOptionGraphicsItem option;

    annot->paint(&painter, &option, nullptr);

    delete sheet;
}


TEST_F(ut_browser, BrowserMagniFierTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "BrowserMagniFierTest"));

    EXPECT_TRUE(sheet->openFileExec(""));

    BrowserMagniFier *magnifier = new BrowserMagniFier(sheet->m_browser);

    magnifier->setMagniFierImage(QImage());

    magnifier->showMagnigierImage(QPoint(10, 10), QPoint(20, 20), 1);

    magnifier->updateImage();

    MagnifierInfo_t task;

    ReadMagnifierManager *manager = new ReadMagnifierManager(magnifier);

    manager->addTask(task);

    manager->wait();

    delete manager;

    delete magnifier;

    delete sheet;
}


TEST_F(ut_browser, BrowserMenuTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "BrowserMenuTest"));

    ///BrowserMenu
    BrowserMenu *menu = new BrowserMenu;
    menu->initActions(sheet, 0, SheetMenuType_e::DOC_MENU_DEFAULT);
    menu->createAction("test displayname", "test object");
    menu->onSetHighLight();
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

    delete sheet;
}

TEST_F(ut_browser, BrowserPageTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "BrowserPageTest"));

    EXPECT_TRUE(sheet->openFileExec(""));

    BrowserPage *page = sheet->m_browser->pages().value(0);

    EXPECT_TRUE(page);

    page->bookmarkMouseRect();

    page->updateBookmarkState();

    page->renderRect(QRectF(0, 0, 1, 1));

    page->renderViewPort();

    page->handleWordLoaded(QList<Word>());

    page->getImagePoint(1, QPoint(0, 0));

    page->loadWords();

    page->setDrawMoveIconRect(true);

    page->selectedWords();

    page->clearSelectSearchHighlightRects();

    page->getBrowserAnnotation(QPointF(0, 0));

    page->getBrowserWord(QPointF(0, 0));

    page->getNorotateRect(QRectF(0, 0, 1, 1));

    page->setSelectIconRect(true, nullptr);

    page->setPageBookMark(QPointF(0, 0));

    Annotation *anno = page->addIconAnnotation(QRectF(0, 0, 1, 1), "test");
    if (anno) delete anno;

    deepin_reader::Annotation *annotation = page->addHighlightAnnotation("text", QColor(Qt::red));

    page->updateAnnotation(annotation, "test", QColor(Qt::red));

    page->removeAnnotation(annotation);

    page->removeAllAnnotation();

    page->scaleWords(false);

    page->scaleWords(true);

    page->getCurrentImage(200, 200);

    page->clearSearchHighlightRects();

    QTimer::singleShot(10, [ = ]() {
        delete sheet;
    });

    exec();
}

TEST_F(ut_browser, BrowserWordTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "BrowserWordTest"));

    EXPECT_TRUE(sheet->openFileExec(""));

    BrowserPage *page = sheet->m_browser->pages().value(0);

    EXPECT_TRUE(page);

    page->loadWords();

    QList<Word> words = sheet->renderer()->getWords(page->itemIndex());

    if (words.count() <= 0)
        GTEST_FAIL();

    BrowserWord *word = new BrowserWord(page, words[0]);

    word->setScaleFactor(1);

    word->setSelectable(true);

    word->boundingBox();

    word->boundingRect();

    word->text();

    QPixmap pixmap(200, 200);

    QPainter painter(&pixmap);

    QStyleOptionGraphicsItem item;

    word->paint(&painter, &item);

    QGraphicsSceneMouseEvent *gsMouseEvent = new QGraphicsSceneMouseEvent;

    word->mousePressEvent(gsMouseEvent);

    word->mouseReleaseEvent(gsMouseEvent);

    delete gsMouseEvent;

    QTimer::singleShot(10, [ = ]() {
        delete sheet;
    });

    exec();
}


TEST_F(ut_browser, PageRenderThreadTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "PageRenderThreadTest"));

    EXPECT_TRUE(sheet->openFileExec(""));

    BrowserPage *page = sheet->m_browser->pages().value(0);

    EXPECT_TRUE(page);

    PageRenderThread::instance();

    PageRenderThread *thread = new PageRenderThread;

    //DocPageNormalImageTask
    DocPageNormalImageTask docPageNormalImageTask;

    docPageNormalImageTask.sheet = sheet;

    docPageNormalImageTask.page = page;

    docPageNormalImageTask.rect = QRect(0, 0, 200, 200);

    docPageNormalImageTask.pixmapId = 0;

    thread->appendTask(docPageNormalImageTask);

    //DocPageSliceImageTask
    DocPageSliceImageTask docPageSliceImageTask;

    docPageSliceImageTask.page = page;

    docPageSliceImageTask.sheet = sheet;

    docPageSliceImageTask.slice = QRect(0, 0, 200, 200);

    docPageSliceImageTask.whole = page->boundingRect().toRect();

    docPageSliceImageTask.pixmapId = 1;

    thread->appendTask(docPageSliceImageTask);

    //DocPageBigImageTask
    DocPageBigImageTask docPageBigImageTask;

    docPageBigImageTask.page = page;

    docPageBigImageTask.sheet = sheet;

    docPageBigImageTask.rect = page->boundingRect().toRect();

    docPageBigImageTask.pixmapId = 2;

    thread->appendTask(docPageSliceImageTask);

    //DocPageWordTask
    DocPageWordTask docPageWordTask;

    docPageWordTask.page = page;

    docPageWordTask.sheet = sheet;

    thread->appendTask(docPageWordTask);

    //DocPageAnnotationTask
    DocPageAnnotationTask docPageAnnotationTask;

    docPageAnnotationTask.page = page;

    docPageAnnotationTask.sheet = sheet;

    thread->appendTask(docPageAnnotationTask);

    //DocPageThumbnailTask
    DocPageThumbnailTask docPageThumbnailTask;

    docPageThumbnailTask.index = 0;

    docPageThumbnailTask.sheet = sheet;

    SideBarImageViewModel *model = new SideBarImageViewModel(sheet);

    docPageThumbnailTask.model = model;

    thread->appendTask(docPageThumbnailTask);

    //DocOpenTask
    DocOpenTask docOpenTask;

    docOpenTask.sheet = sheet;

    docOpenTask.password = "";

    SheetRenderer *render = new SheetRenderer(sheet);

    docOpenTask.renderer = render;

    thread->appendTask(docOpenTask);

    //DocCloseTask
    DocCloseTask docCloseTask;

    docCloseTask.pages = QList<deepin_reader::Page *>();

    docCloseTask.document = nullptr;

    thread->appendTask(docCloseTask);

    delete thread;

    delete render;

    delete model;

    QTimer::singleShot(10, [ = ]() {
        delete sheet;
    });

    exec();
}

TEST_F(ut_browser, PageSearchThreadTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "PageSearchThreadTest"));

    EXPECT_TRUE(sheet->openFileExec(""));

    PageSearchThread *thread = new PageSearchThread;

    thread->startSearch(sheet, "test");

    thread->stopSearch();

    delete thread;

    QTimer::singleShot(10, [ = ]() {
        delete sheet;
    });

    exec();
}

TEST_F(ut_browser, SheetBrowserTest)
{
    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "SheetBrowserTest"));

    ASSERT_TRUE(sheet->openFileExec(""));

    SheetBrowser *browser = new SheetBrowser(sheet);

    browser->init(sheet->m_operation, sheet->m_bookmarks);

    browser->onInit();

    browser->getClickAnnot(browser->m_items.at(0), QPointF(0, 0), false);

    browser->moveIconAnnot(browser->m_items.at(0), QPointF(0, 0));

    QRectF iconRect = QRectF(0, 0, 1, 1.0);

    browser->calcIconAnnotRect(browser->m_items.at(0), QPointF(0, 0), iconRect);

    QPoint showPoint = QPoint(1, 1);

    Annotation *hightLightAnnot = browser->addHighLightAnnotation("123", QColor(Qt::red), showPoint);

    browser->jumpToHighLight(hightLightAnnot, 0);

    browser->onAddHighLightAnnot(nullptr, "123", QColor(Qt::red));

    browser->setMouseShape(Dr::MouseShapeHand);

    browser->setBookMark(0, 1);

    browser->setAnnotationInserting(false);

    browser->onVerticalScrollBarValueChanged(1);

    browser->onHorizontalScrollBarValueChanged(1);

    browser->beginViewportChange();

    browser->hideSubTipsWidget();

    browser->onViewportChanged();

    browser->showNoteEditWidget(hightLightAnnot, QPoint(0, 0));

    browser->jump2PagePos(browser->m_items.at(0), 1, 2);

    browser->jumpToOutline(0, 0, 0);

    browser->onRemoveAnnotation(hightLightAnnot, true);

    browser->hasLoaded();

    browser->openMagnifier();

    browser->magnifierOpened();

    browser->closeMagnifier();

    browser->maxWidth();

    browser->maxHeight();

    browser->needBookmark(0, 1);

    browser->handleSearchStart();

    browser->jumpToNextSearchResult();

    browser->jumpToPrevSearchResult();

    browser->handleSearchResultComming(deepin_reader::SearchResult());

    browser->handleFindFinished(123);

    browser->handleSearchStop();

    browser->setDocTapGestrue(QPoint(0, 0));

    browser->clearSelectIconAnnotAfterMenu();

    browser->isLink(QPoint(0, 0));

    browser->setIconAnnotSelect(true);

    browser->onSetDocSlideGesture();

    browser->onRemoveIconAnnotSelect();

    browser->curpageChanged(0);

    browser->setCurrentPage(0);

    browser->jump2Link(QPoint(0, 0));

    QImage image;

    browser->getExistImage(0, image, 200, 200);

    browser->currentPage();

    browser->allPages();

    browser->translate2Local(QPointF(0, 0));

    QPointF pagePoint = QPointF(0, 0);

    browser->getBrowserPageForPoint(pagePoint);

    browser->currentScrollValueForPage();

    int fromIndex = 0;

    int toIndex = 1;

    browser->currentIndexRange(fromIndex, toIndex);

    browser->selectedWordsText();

    //browser->showMenu();     //阻塞

    browser->deform(sheet->m_operation);

    QShowEvent showEvent;

    browser->showEvent(&showEvent);

    QGestureEvent gestureEvent((QList<QGesture *>()));

    browser->gestureEvent(&gestureEvent);

    QTimerEvent timerEvent(5);

    browser->timerEvent(&timerEvent);

    QEvent event(QEvent::Timer);

    browser->event(&event);

    QMouseEvent pressMouseEvent(QEvent::MouseButtonPress, QPointF(0, 0), QPoint(0, 0), QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    browser->mousePressEvent(&pressMouseEvent);

    QMouseEvent releaseMouseEvent(QEvent::MouseButtonRelease, QPointF(0, 0), QPoint(0, 0), QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    browser->mouseReleaseEvent(&releaseMouseEvent);

    QMouseEvent *mouseEvent = new QMouseEvent(QEvent::Move, QPointF(0, 0), QPointF(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    browser->mouseMoveEvent(mouseEvent);

    delete mouseEvent;

    QDragEnterEvent dragEnterEvent(QPoint(0, 0), Qt::CopyAction, nullptr, Qt::LeftButton, Qt::NoModifier);

    browser->dragEnterEvent(&dragEnterEvent);

    QResizeEvent resizeEvent(QSize(800, 600), QSize(400, 300));

    browser->resizeEvent(&resizeEvent);

    QPinchGesture *pinchGesture = new QPinchGesture;

    browser->pinchTriggered(pinchGesture);

    delete pinchGesture;

    QWheelEvent *wheelEvent = new QWheelEvent(QPointF(0, 0), 1, Qt::LeftButton, Qt::NoModifier);

    browser->wheelEvent(wheelEvent);

    delete wheelEvent;

    delete browser;

    QTimer::singleShot(10, [ = ]() {
        delete sheet;
    });

    exec();
}
