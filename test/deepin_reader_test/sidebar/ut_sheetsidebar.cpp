
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
#include "Application.h"

#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"
#include "sidebar/note/NotesWidget.h"
#include "sidebar/search/SearchResWidget.h"
#include "sidebar/bookmark/BookMarkWidget.h"
#include "sidebar/note/NoteViewWidget.h"
#include "sidebar/thumbnail/ThumbnailWidget.h"
#include "sidebar/catalog/CatalogWidget.h"
#include "sidebar/catalog/CatalogTreeView.h"
#include "sidebar/note/TransparentTextEdit.h"
#include "sidebar/thumbnail/PagingWidget.h"
#include "sidebar/SideBarImageViewModel.h"

#undef private
#undef protected

#include "Central.h"
#include "DocSheet.h"
#include "MsgHeader.h"

#include <QUuid>
#include <DApplication>
#include <QClipboard>

ut_sidebar::ut_sidebar()
{
}

void ut_sidebar::SetUp()
{
    ut_application::SetUp();
}

void ut_sidebar::TearDown()
{
}

#ifdef UT_SHEETSIDEBAR_TEST
TEST_F(ut_sidebar, SidebarTest)
{
    QString path = UT_FILE_TEST_FILE;
    MainWindow *mainWindow = MainWindow::createWindow(QStringList() << path);
    ASSERT_TRUE(mainWindow->m_central);
    mainWindow->showDefaultSize();
    mainWindow->show();

    CentralDocPage *docpage = mainWindow->m_central->docPage();
    //Central
    ASSERT_TRUE(docpage);

    DocSheet *sheet = docpage->getSheet(path);
    ASSERT_TRUE(sheet);

    sheet->setSidebarVisible(true);
    sheet->m_sidebar->onBtnClicked(2);

    SheetSidebar sideBar(sheet, PreviewWidgesFlag::PREVIEW_THUMBNAIL | PreviewWidgesFlag::PREVIEW_CATALOG | PreviewWidgesFlag::PREVIEW_BOOKMARK | PreviewWidgesFlag::PREVIEW_NOTE);
    sideBar.show();
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

    sideBar.handleAnntationMsg(0, -1, nullptr);
    sideBar.handleAnntationMsg(1, -1, nullptr);
    sideBar.handleAnntationMsg(2, -1, nullptr);

    sideBar.adaptWindowSize(1.0);
    sideBar.onBtnClicked(0);
    sideBar.onUpdateWidgetTheme();

    sideBar.onHandWidgetDocOpenSuccess();
    sideBar.onHandleOpenSuccessDelay();

    sideBar.dealWithPressKey(Dr::key_up);
    sideBar.dealWithPressKey(Dr::key_down);
    sideBar.dealWithPressKey(Dr::key_delete);
    sideBar.onJumpToPrevPage();
    sideBar.onJumpToPageDown();
    sideBar.onJumpToPageUp();
    sideBar.deleteItemByKey();
    sideBar.onJumpToNextPage();
    sideBar.showMenu();

    QResizeEvent sidebarresizeEvent(QSize(100, 400), QSize(100, 600));
    QCoreApplication::sendEvent(&sideBar, &sidebarresizeEvent);

    QKeyEvent sideekeyRevent(QEvent::KeyPress, Qt::Key_Right, Qt::ControlModifier);
    QCoreApplication::sendEvent(&sideBar, &sideekeyRevent);

    QKeyEvent sideekeyDevent(QEvent::KeyPress, Qt::Key_Delete, Qt::ControlModifier);
    QCoreApplication::sendEvent(&sideBar, &sideekeyDevent);

    //ThumbnailWidget
    ThumbnailWidget thumbnailWidget(sheet);
    thumbnailWidget.show();
    thumbnailWidget.handleOpenSuccess();
    thumbnailWidget.handleOpenSuccess();
    thumbnailWidget.m_pImageListView->scrollToIndex(0);
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
    thumbnailWidget.pageDown();
    thumbnailWidget.pageUp();

    thumbnailWidget.adaptWindowSize(1.0);
    thumbnailWidget.updateThumbnail(-1);
    thumbnailWidget.updateThumbnail(0);
    thumbnailWidget.updateThumbnail(10000);
    thumbnailWidget.scrollToCurrentPage();
    thumbnailWidget.repaint();

    QList<QWidget *> tabWidgetlst;
    thumbnailWidget.setTabOrderWidget(tabWidgetlst);

    thumbnailWidget.m_pPageWidget->slotUpdateTheme();
    thumbnailWidget.m_pPageWidget->onEditFinished();
    thumbnailWidget.m_pPageWidget->normalChangePage();
    thumbnailWidget.m_pPageWidget->pageNumberJump();
    thumbnailWidget.m_pPageWidget->slotPrePageBtnClicked();
    thumbnailWidget.m_pPageWidget->slotNextPageBtnClicked();
    thumbnailWidget.m_pPageWidget->setTabOrderWidget(tabWidgetlst);
    thumbnailWidget.m_pPageWidget->SlotJumpPageLineEditReturnPressed();
    thumbnailWidget.m_pPageWidget->handleOpenSuccess();
    thumbnailWidget.m_pPageWidget->setIndex(0);
    thumbnailWidget.m_pPageWidget->setBtnState(0, sheet->pagesNumber());

    thumbnailWidget.m_pImageListView->onSetThumbnailListSlideGesture();
    thumbnailWidget.m_pImageListView->onRemoveThumbnailListSlideGesture();
    thumbnailWidget.m_pImageListView->model()->setData(QModelIndex(), "123", Qt::UserRole);

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
    catalogWidget.nextPage();
    catalogWidget.prevPage();
    catalogWidget.m_strTheme = "Test";
    catalogWidget.m_sheet = nullptr;
    catalogWidget.handleOpenSuccess();
    catalogWidget.pageUp();
    catalogWidget.pageDown();

    QResizeEvent resizeEvent(QSize(100, 400), QSize(100, 600));
    QCoreApplication::sendEvent(&catalogWidget, &resizeEvent);

    EXPECT_TRUE(catalogWidget.m_pTree);
    catalogWidget.m_pTree->slotExpanded(catalogWidget.m_pTree->model()->index(0, 0));
    catalogWidget.m_pTree->slotCollapsed(catalogWidget.m_pTree->model()->index(0, 0));
    catalogWidget.m_pTree->onItemClicked(catalogWidget.m_pTree->model()->index(0, 0));
    catalogWidget.m_pTree->currentChanged(catalogWidget.m_pTree->model()->index(0, 0), catalogWidget.m_pTree->model()->index(1, 0));
    QMouseEvent mouseevent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::RightButton, Qt::NoButton, Qt::NoModifier);
    QCoreApplication::sendEvent(catalogWidget.m_pTree, &mouseevent);

    QKeyEvent keyevent(QEvent::KeyPress, Qt::Key_1, Qt::ControlModifier);
    QCoreApplication::sendEvent(catalogWidget.m_pTree, &keyevent);

    catalogWidget.m_pTree->currentChanged(catalogWidget.m_pTree->model()->index(0, 0), catalogWidget.m_pTree->model()->index(1, 0));

    QCoreApplication::sendEvent(catalogWidget.m_pTree, &resizeEvent);

    catalogWidget.m_pTree->pageDownPage();
    catalogWidget.m_pTree->prevPage();
    catalogWidget.m_pTree->nextPage();
    catalogWidget.m_pTree->pageUpPage();
    catalogWidget.m_pTree->scrollToIndex(catalogWidget.m_pTree->indexAt(QPoint(0, 0)));

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
    bookwidget.deleteItemByKey();
    bookwidget.handlePage(0);
    bookwidget.handleBookMark(0, 1);
    bookwidget.prevPage();
    bookwidget.handleBookMark(0, 0);
    bookwidget.prevPage();
    bookwidget.onUpdateTheme();
    bookwidget.onAddBookMarkClicked();
    bookwidget.onListMenuClick(E_BOOKMARK_DELETE);
    bookwidget.deleteAllItem();
    bookwidget.pageUp();
    bookwidget.pageDown();
    bookwidget.setTabOrderWidget(tabWidgetlst);
    bookwidget.m_sheet = nullptr;
    bookwidget.prevPage();
    bookwidget.addBtnCheckEnter();

    //NotesWidget
    NotesWidget noteWidget(sheet);
    noteWidget.prevPage();
    noteWidget.nextPage();
    noteWidget.deleteItemByKey();
    noteWidget.handleOpenSuccess();
    noteWidget.handleAnntationMsg(-1, nullptr);
    noteWidget.handleAnntationMsg(0, nullptr);
    noteWidget.handleAnntationMsg(1, nullptr);
    noteWidget.handleAnntationMsg(2, nullptr);
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
    noteWidget.addNoteItem(nullptr);
    noteWidget.deleteNoteItem(nullptr);
    noteWidget.deleteAllItem();
    noteWidget.m_sheet = nullptr;
    noteWidget.prevPage();
    noteWidget.nextPage();
    noteWidget.handleOpenSuccess();
    noteWidget.pageUp();
    noteWidget.pageDown();
    noteWidget.addBtnCheckEnter();
    noteWidget.setTabOrderWidget(tabWidgetlst);
    noteWidget.m_pImageListView = nullptr;
    noteWidget.showMenu();

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

    ImagePageInfo_t imageInfo1(1);
    ImagePageInfo_t imageInfo2(2);
    ImagePageInfo_t imageInfo3(imageInfo1);

    EXPECT_FALSE(imageInfo1 == imageInfo2);
    EXPECT_TRUE(imageInfo1 == imageInfo3);
    EXPECT_TRUE(imageInfo1 < imageInfo2);
    EXPECT_FALSE(imageInfo1 > imageInfo2);

    mainWindow->closeWithoutSave();

    a->quit();
}

TEST_F(ut_sidebar, TransparentTextEditTest)
{
    TransparentTextEdit textedit;
    textedit.show();
    textedit.slotTextEditMaxContantNum();
    textedit.m_nMaxContantLen = 10;
    textedit.setText("1111111111111111111111111111111111");
    textedit.slotTextEditMaxContantNum();
    textedit.update();
    QClipboard *data = DApplication::clipboard();
    textedit.insertFromMimeData(data->mimeData());
    textedit.repaint();

    QKeyEvent keyevent(QEvent::KeyPress, Qt::Key_N, Qt::AltModifier);
    QCoreApplication::sendEvent(&textedit, &keyevent);
}

TEST_F(ut_sidebar, NoteViewWidgetTest)
{
    NoteShadowViewWidget shadowView(nullptr);
    shadowView.showWidget(QPoint(0, 0));
    shadowView.getNoteViewWidget()->setEditText("const QString & note");
    shadowView.getNoteViewWidget()->setEditText("");
    shadowView.getNoteViewWidget()->setAnnotation(nullptr);
    shadowView.getNoteViewWidget()->onBlurWindowChanged();
    shadowView.getNoteViewWidget()->repaint();
    shadowView.getNoteViewWidget()->show();

    QTimer::singleShot(1, [&shadowView]() {
        QHideEvent hideEvent;
        QCoreApplication::sendEvent(&shadowView, &hideEvent);

        shadowView.getNoteViewWidget()->hide();
        shadowView.getNoteViewWidget()->onShowMenu();
    });

    exec();
}
#endif
