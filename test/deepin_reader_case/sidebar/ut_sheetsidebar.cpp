
/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ut_sheetsidebar.h"

#define private public
#define protected public
#include "MainWindow.h"
#include "SheetSidebar.h"
#include "CentralDocPage.h"

#include "ImageListview.h"
#include "ImageViewModel.h"
#include "sidebar/note/NotesWidget.h"
#include "sidebar/search/SearchResWidget.h"
#include "sidebar/bookmark/BookMarkWidget.h"
#include "sidebar/note/NoteViewWidget.h"
#include "sidebar/thumbnail/ThumbnailWidget.h"
#include "sidebar/catalog/CatalogWidget.h"
#include "sidebar/catalog/CatalogTreeView.h"
#include "sidebar/note/TransparentTextEdit.h"

#include "menu/BookMarkMenu.h"
#include "menu/NoteMenu.h"

#include "widgets/SlideWidget.h"
#undef private
#undef protected

#include "Central.h"
#include "DocSheet.h"
#include "widgets/AttrScrollWidget.h"
#include "widgets/PrintManager.h"

#include <QUuid>
#include <DApplication>
#include <QClipboard>

Ut_SheetSidebar::Ut_SheetSidebar()
{
}

void Ut_SheetSidebar::SetUp()
{
}

void Ut_SheetSidebar::TearDown()
{
}

#ifdef UT_SHEETSIDEBAR_TEST
TEST_F(Ut_SheetSidebar, SidebarTest)
{
    const QString &path = "/home/leiyu/Desktop/1.pdf";
    MainWindow *mainWindow = MainWindow::createWindow(QStringList() << path);
    mainWindow->show();
    ASSERT_TRUE(mainWindow->m_central);

    CentralDocPage *docpage = mainWindow->m_central->docPage();
    //Central
    ASSERT_TRUE(docpage);
    ASSERT_TRUE(mainWindow->m_central->titleMenu());
    ASSERT_TRUE(mainWindow->m_central->titleWidget());

    DocSheet *sheet = docpage->getSheet(path);
    ASSERT_TRUE(sheet);
    EXPECT_TRUE(mainWindow->m_central->hasSheet(sheet));
    EXPECT_TRUE(mainWindow->m_central->saveAll());

    //CentralDocPage
    sheet->setBookMark(0, !sheet->hasBookMark(0));
    EXPECT_FALSE(docpage->firstThumbnail(path, "/home/leiyu/Desktop/1.png"));
    EXPECT_TRUE(docpage->saveCurrent());
    EXPECT_EQ(docpage->getCurSheet(), sheet);
    EXPECT_FALSE(docpage->isFullScreen());
    EXPECT_FALSE(docpage->quitFullScreen());
    EXPECT_TRUE(docpage->getTitleLabel());
    EXPECT_TRUE(!docpage->getDocTabbarText(0).isEmpty());

    //DocSheet
    EXPECT_FALSE(sheet->firstThumbnail(path).isNull());
    EXPECT_FALSE(sheet->existFileChanged());
    const QString &uuid = DocSheet::getUuid(sheet).toString();
    EXPECT_EQ(DocSheet::getSheet(uuid), sheet);
    EXPECT_FALSE(sheet->isOpen());
    EXPECT_TRUE(sheet->pagesNumber() > 0);
    EXPECT_TRUE(sheet->currentPage() > 0 && sheet->currentPage() <= sheet->pagesNumber());
    EXPECT_TRUE(sheet->currentIndex() >= 0 && sheet->currentIndex() < sheet->pagesNumber());

    sheet->jumpToIndex(0);
    EXPECT_TRUE(sheet->currentIndex() == 0);
    EXPECT_TRUE(sheet->currentPage() == 1);

    sheet->jumpToPage(1);
    EXPECT_TRUE(sheet->currentIndex() == 0);
    EXPECT_TRUE(sheet->currentPage() == 1);

    sheet->jumpToFirstPage();
    EXPECT_TRUE(sheet->currentIndex() == 0);
    EXPECT_TRUE(sheet->currentPage() == 1);

    sheet->jumpToLastPage();
    EXPECT_TRUE(sheet->currentIndex() == sheet->pagesNumber() - 1);
    EXPECT_TRUE(sheet->currentPage() == sheet->pagesNumber());

    sheet->jumpToIndex(0);
    sheet->jumpToNextPage();
    EXPECT_TRUE(sheet->currentIndex() == 1);
    EXPECT_TRUE(sheet->currentPage() == 2);

    sheet->jumpToLastPage();
    sheet->jumpToNextPage();
    EXPECT_TRUE(sheet->currentIndex() == sheet->pagesNumber() - 1);
    EXPECT_TRUE(sheet->currentPage() == sheet->pagesNumber());

    sheet->jumpToPrevPage();
    EXPECT_TRUE(sheet->currentIndex() == sheet->pagesNumber() - 2);
    EXPECT_TRUE(sheet->currentPage() == sheet->pagesNumber() - 1);

    sheet->jumpToIndex(0);
    sheet->jumpToPrevPage();
    EXPECT_TRUE(sheet->currentIndex() == 0);
    EXPECT_TRUE(sheet->currentPage() == 1);

    EXPECT_TRUE(sheet->outline().size() >= 0);

    sheet->openMagnifier();
    EXPECT_TRUE(sheet->magnifierOpened());
    sheet->closeMagnifier();

    sheet->setBookMark(0, !sheet->hasBookMark(0));
    EXPECT_TRUE(sheet->fileChanged());
    EXPECT_TRUE(sheet->saveData());

    QImage imageTest;
    EXPECT_TRUE(sheet->getImage(0, imageTest, 100, 100));
    EXPECT_FALSE(imageTest.isNull());

    EXPECT_TRUE(sheet->annotations().size() > 0);
    EXPECT_TRUE(sheet->scaleFactorList().size() > 0);

    EXPECT_TRUE(sheet->maxScaleFactor() >= 0.1 && sheet->maxScaleFactor() <= 5.0);
    EXPECT_TRUE(sheet->filter() == "Pdf File (*.pdf)");

    sheet->format();
    EXPECT_TRUE(sheet->getBookMarkList().size() > 0);

    sheet->setLayoutMode(Dr::SinglePageMode);
    EXPECT_TRUE(sheet->operation().layoutMode == Dr::SinglePageMode);
    sheet->setScaleMode(Dr::FitToPageDefaultMode);
    EXPECT_TRUE(sheet->operation().scaleMode == Dr::FitToPageDefaultMode);
    sheet->setScaleFactor(1.0);
    EXPECT_TRUE(sheet->operation().scaleFactor == 1.0);
    sheet->setMouseShape(Dr::MouseShapeHand);
    EXPECT_TRUE(sheet->operation().mouseShape == Dr::MouseShapeHand);
    int rotation = sheet->operation().rotation;
    sheet->rotateLeft();
    sheet->rotateRight();
    EXPECT_TRUE(rotation == sheet->operation().rotation);

    EXPECT_TRUE(sheet->fileType() == Dr::FileType::PDF);
    EXPECT_TRUE(sheet->filePath() == path);

    sheet->setBookMark(0, 0);
    EXPECT_FALSE(sheet->hasBookMark(0));

    sheet->openFullScreen();
    EXPECT_TRUE(sheet->isFullScreen());
    sheet->closeFullScreen();

    EXPECT_FALSE(sheet->needPassword());
    EXPECT_TRUE(sheet->isUnLocked());

    SheetSidebar sideBar(sheet, PreviewWidgesFlag::PREVIEW_THUMBNAIL | PreviewWidgesFlag::PREVIEW_CATALOG | PreviewWidgesFlag::PREVIEW_BOOKMARK | PreviewWidgesFlag::PREVIEW_NOTE);
    sideBar.resize(200, 600);
    DToolButton *btn = sideBar.createBtn("test", "test");
    EXPECT_TRUE(btn);

    //AttrScrollWidget
    AttrScrollWidget widgetTest(sheet);
    widgetTest.resize(100, 400);

    //PrintManager
    PrintManager printManager(sheet);

    //SlideWidget
    SlideWidget slidewidget(sheet);
    slidewidget.playImage();
    slidewidget.drawImage(QPixmap());

    slidewidget.onPreBtnClicked();
    slidewidget.onPlayBtnClicked();
    slidewidget.onNextBtnClicked();
    slidewidget.onExitBtnClicked();
    slidewidget.onImagevalueChanged(0);
    slidewidget.onImageShowTimeOut();

    slidewidget.onFetchImage(0);
    slidewidget.onUpdatePageImage(0);
    slidewidget.setWidgetState(false);
    slidewidget.setWidgetState(true);
    slidewidget.handleKeyPressEvent(Dr::key_space);
    slidewidget.handleKeyPressEvent(Dr::key_space);
    slidewidget.handleKeyPressEvent(Dr::key_left);
    slidewidget.handleKeyPressEvent(Dr::key_right);

    slidewidget.onParentDestroyed();
    slidewidget.setWidgetState(true);

    //ThumbnailWidget
    ThumbnailWidget thumbnailWidget(sheet);
    thumbnailWidget.handleOpenSuccess();
    thumbnailWidget.handleOpenSuccess();
    thumbnailWidget.handlePage(-1);
    thumbnailWidget.handlePage(0);
    thumbnailWidget.handlePage(100000);
    thumbnailWidget.handleRotate(0);
    thumbnailWidget.setBookMarkState(-1, 0);
    thumbnailWidget.setBookMarkState(0, 0);
    thumbnailWidget.setBookMarkState(10000, 0);

    thumbnailWidget.setBookMarkState(-1, 1);
    thumbnailWidget.setBookMarkState(0, 1);
    thumbnailWidget.setBookMarkState(10000, 1);

    thumbnailWidget.prevPage();
    thumbnailWidget.nextPage();

    thumbnailWidget.adaptWindowSize(1.0);
    thumbnailWidget.updateThumbnail(-1);
    thumbnailWidget.updateThumbnail(0);
    thumbnailWidget.updateThumbnail(10000);
    thumbnailWidget.scrollToCurrentPage();
    EXPECT_TRUE(thumbnailWidget.getLastFocusWidget());

    thumbnailWidget.m_sheet = nullptr;
    thumbnailWidget.prevPage();
    thumbnailWidget.nextPage();

    //CatalogWidget
    CatalogWidget catalogWidget(sheet);
    catalogWidget.resize(100, 600);
    catalogWidget.initWidget();
    catalogWidget.handleOpenSuccess();
    catalogWidget.handleOpenSuccess();
    catalogWidget.handlePage(0);
    catalogWidget.handlePage(1);
    catalogWidget.setTitleTheme();
    catalogWidget.m_strTheme = "Test";
    catalogWidget.m_sheet = nullptr;
    catalogWidget.handleOpenSuccess();

    EXPECT_TRUE(catalogWidget.m_pTree);
    catalogWidget.m_pTree->SlotExpanded(catalogWidget.m_pTree->model()->index(0, 0));
    catalogWidget.m_pTree->SlotCollapsed(catalogWidget.m_pTree->model()->index(0, 0));
    catalogWidget.m_pTree->onItemClicked(catalogWidget.m_pTree->model()->index(0, 0));
    catalogWidget.m_pTree->currentChanged(catalogWidget.m_pTree->model()->index(0, 0), catalogWidget.m_pTree->model()->index(1, 0));
    QMouseEvent mouseevent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::RightButton, Qt::NoButton, Qt::NoModifier);
    QCoreApplication::sendEvent(catalogWidget.m_pTree, &mouseevent);
    QKeyEvent keyevent(QEvent::KeyPress, Qt::Key_1, Qt::ControlModifier);
    QCoreApplication::sendEvent(catalogWidget.m_pTree, &keyevent);
    catalogWidget.m_pTree->currentChanged(catalogWidget.m_pTree->model()->index(0, 0), catalogWidget.m_pTree->model()->index(1, 0));
    catalogWidget.m_pTree->m_sheet = nullptr;
    catalogWidget.m_pTree->currentChanged(catalogWidget.m_pTree->model()->index(0, 0), catalogWidget.m_pTree->model()->index(1, 0));
    catalogWidget.m_pTree->handleOpenSuccess();

    //BookMarkWidget
    BookMarkWidget bookwidget(sheet);
    bookwidget.initWidget();
    bookwidget.handleOpenSuccess();
    bookwidget.adaptWindowSize(1.0);
    bookwidget.updateThumbnail(-1);
    bookwidget.updateThumbnail(10000);
    bookwidget.nextPage();
    bookwidget.DeleteItemByKey();
    bookwidget.handlePage(0);
    bookwidget.handleBookMark(0, 1);
    bookwidget.prevPage();
    bookwidget.handleBookMark(0, 0);
    bookwidget.prevPage();
    bookwidget.onUpdateTheme();
    bookwidget.onAddBookMarkClicked();
    bookwidget.onListMenuClick(E_BOOKMARK_DELETE);
    bookwidget.deleteAllItem();
    EXPECT_TRUE(bookwidget.getAddBtn());
    bookwidget.m_sheet = nullptr;
    bookwidget.prevPage();

    //NotesWidget
    NotesWidget noteWidget(sheet);
    noteWidget.prevPage();
    noteWidget.nextPage();
    noteWidget.DeleteItemByKey();
    noteWidget.handleOpenSuccess();
    noteWidget.handleAnntationMsg(-1, 0);
    noteWidget.handleAnntationMsg(0, 0);
    noteWidget.handleAnntationMsg(1, 0);
    noteWidget.handleAnntationMsg(2, 0);
    noteWidget.adaptWindowSize(1.0);
    noteWidget.updateThumbnail(-1);
    noteWidget.updateThumbnail(100000);
    noteWidget.changeResetModelData();
    noteWidget.onListMenuClick(-1);
    noteWidget.onListMenuClick(4);
    noteWidget.onListMenuClick(5);
    noteWidget.onListItemClicked(-1);
    noteWidget.onListItemClicked(0);
    noteWidget.onListItemClicked(10000);
    noteWidget.onAddAnnotation();
    noteWidget.copyNoteContent();
    noteWidget.addNoteItem(0);
    noteWidget.deleteNoteItem(0);
    noteWidget.deleteAllItem();
    EXPECT_TRUE(noteWidget.getAddBtn());
    noteWidget.m_sheet = nullptr;
    noteWidget.prevPage();
    noteWidget.nextPage();
    noteWidget.handleOpenSuccess();

    //SearchResWidget
    SearchResWidget searchWidget(sheet);
    searchWidget.clearFindResult();
    searchWidget.adaptWindowSize(1.0);
    searchWidget.updateThumbnail(-1);
    searchWidget.updateThumbnail(10000);
    EXPECT_EQ(searchWidget.handleFindFinished(), 0);
    searchWidget.addSearchsItem(0, "test", 5);
    searchWidget.addSearchsItem(-1, "test", -1);
    searchWidget.handFindContentComming(deepin_reader::SearchResult());
    EXPECT_EQ(searchWidget.handleFindFinished(), 2);

    searchWidget.m_sheet = nullptr;
    searchWidget.addSearchsItem(0, "test", 5);
    EXPECT_EQ(searchWidget.handleFindFinished(), 2);
}

TEST(Ut_BookMarkMenu, BookMarkMenuTest)
{
    BookMarkMenu booksMenu;
    booksMenu.slotDelete();
    booksMenu.slotDeleteAll();
}

TEST(Ut_NoteMenu, NoteMenuTest)
{
    NoteMenu noteMenu;
    noteMenu.slotCopy();
    noteMenu.slotDelete();
    noteMenu.slotAllDelete();
}

TEST(Ut_TransparentTextEdit, TransparentTextEditTest)
{
    TransparentTextEdit textedit;
    textedit.slotTextEditMaxContantNum();
    textedit.m_nMaxContantLen = 10;
    textedit.setText("1111111111111111111111111111111111");
    textedit.slotTextEditMaxContantNum();
    textedit.update();
    QClipboard *data = DApplication::clipboard();
    textedit.insertFromMimeData(data->mimeData());
}

TEST(Ut_NoteViewWidget, NoteViewWidgetTest)
{
    NoteShadowViewWidget shadowView(nullptr);
    NoteViewWidget noteviewWidget(&shadowView);
    shadowView.showWidget(QPoint(0, 0));

    noteviewWidget.setEditText("const QString & note");
    noteviewWidget.setEditText("");
    noteviewWidget.setAnnotation(0);
}
#endif
