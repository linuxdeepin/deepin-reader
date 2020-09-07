
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
#include "MsgHeader.h"

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
    const QString &path = UT_FILE_TEST_FILE;
    MainWindow *mainWindow = MainWindow::createWindow(QStringList() << path);
    ASSERT_TRUE(mainWindow->m_central);

    CentralDocPage *docpage = mainWindow->m_central->docPage();
    //Central
    ASSERT_TRUE(docpage);

    DocSheet *sheet = docpage->getSheet(path);
    ASSERT_TRUE(sheet);

    SheetSidebar sideBar(sheet, PreviewWidgesFlag::PREVIEW_THUMBNAIL | PreviewWidgesFlag::PREVIEW_CATALOG | PreviewWidgesFlag::PREVIEW_BOOKMARK | PreviewWidgesFlag::PREVIEW_NOTE);
    sideBar.resize(200, 600);
    DToolButton *btn = sideBar.createBtn("test", "test");
    EXPECT_TRUE(btn);
    sideBar.handleOpenSuccess();
    sideBar.setBookMark(0, 0);
    sideBar.setBookMark(-1, 0);
    sideBar.setBookMark(10000, 0);
    sideBar.setBookMark(0, 1);
    sideBar.setBookMark(-1, 1);
    sideBar.setBookMark(10000, 1);

    sideBar.setCurrentPage(-1);
    sideBar.setCurrentPage(0);
    sideBar.setCurrentPage(10000);

    sideBar.handleFindOperation(E_FIND_CONTENT);
    sideBar.handleFindOperation(E_FIND_EXIT);

    sideBar.handleFindContentComming(deepin_reader::SearchResult());
    sideBar.handleFindFinished();
    sideBar.handleRotate();
    sideBar.changeResetModelData();
    sideBar.handleUpdateThumbnail(-1);
    sideBar.handleUpdateThumbnail(0);
    sideBar.handleUpdateThumbnail(10000);

    sideBar.handleUpdatePartThumbnail(0);
    sideBar.handleUpdatePartThumbnail(-1);
    sideBar.handleUpdatePartThumbnail(10000);

    sideBar.handleAnntationMsg(0, -1, 0);
    sideBar.handleAnntationMsg(1, -1, 0);
    sideBar.handleAnntationMsg(2, -1, 0);

    sideBar.adaptWindowSize(1.0);
    sideBar.onBtnClicked(0);
    sideBar.onUpdateWidgetTheme();

    sideBar.onHandWidgetDocOpenSuccess();
    sideBar.onHandleOpenSuccessDelay();

    sideBar.dealWithPressKey(Dr::key_up);
    sideBar.dealWithPressKey(Dr::key_down);
    sideBar.dealWithPressKey(Dr::key_delete);
    sideBar.onJumpToPrevPage();
    sideBar.onJumpToNextPage();
    sideBar.deleteItemByKey();

    QKeyEvent sidekeyLevent(QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier);
    QCoreApplication::sendEvent(&sideBar, &sidekeyLevent);

    QKeyEvent sideekeyRevent(QEvent::KeyPress, Qt::Key_Right, Qt::ControlModifier);
    QCoreApplication::sendEvent(&sideBar, &sideekeyRevent);

    QKeyEvent sideekeyDevent(QEvent::KeyPress, Qt::Key_Delete, Qt::ControlModifier);
    QCoreApplication::sendEvent(&sideBar, &sideekeyDevent);

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
    thumbnailWidget.handleRotate();
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

    thumbnailWidget.m_sheet = nullptr;
    thumbnailWidget.prevPage();
    thumbnailWidget.nextPage();

    //CatalogWidget
    CatalogWidget catalogWidget(sheet);
    catalogWidget.resize(100, 600);
    catalogWidget.handleOpenSuccess();
    catalogWidget.handleOpenSuccess();
    catalogWidget.handlePage(0);
    catalogWidget.handlePage(10000);
    catalogWidget.handlePage(-1);
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
    deepin_reader::SearchResult searchRes;
    searchRes.words << deepin_reader::Word();
    searchWidget.handFindContentComming(searchRes);
    EXPECT_EQ(searchWidget.handleFindFinished(), 2);

    searchWidget.m_sheet = nullptr;
    searchWidget.addSearchsItem(0, "test", 5);
    EXPECT_EQ(searchWidget.handleFindFinished(), 2);

    EXPECT_TRUE(searchWidget.m_pImageListView);
    searchWidget.m_pImageListView->handleOpenSuccess();
    searchWidget.m_pImageListView->scrollToIndex(0);
    searchWidget.m_pImageListView->scrollToIndex(-1);
    searchWidget.m_pImageListView->scrollToIndex(10000);
    searchWidget.m_pImageListView->scrollToModelInexPage(searchWidget.m_pImageListView->getImageModel()->index(0, 0));
    searchWidget.m_pImageListView->getModelIndexForPageIndex(0);
    searchWidget.m_pImageListView->getPageIndexForModelIndex(0);
    EXPECT_LE(searchWidget.m_pImageListView->getModelIndexForPageIndex(-1), 0);
    EXPECT_TRUE(searchWidget.m_pImageListView->getImageModel());
    searchWidget.m_pImageListView->onUpdatePageImage(0);
    searchWidget.m_pImageListView->onUpdatePageImage(-1);
    searchWidget.m_pImageListView->onItemClicked(searchWidget.m_pImageListView->getImageModel()->index(0, 0));
    searchWidget.m_pImageListView->onItemClicked(QModelIndex());
    QMouseEvent mouserevent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::RightButton, Qt::NoButton, Qt::NoModifier);
    QCoreApplication::sendEvent(searchWidget.m_pImageListView, &mouserevent);

    sheet->saveData();
    mainWindow->close();
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
