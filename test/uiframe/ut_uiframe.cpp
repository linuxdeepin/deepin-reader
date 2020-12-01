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

#include "ut_uiframe.h"
#include "MsgHeader.h"

#define private public
#define protected public

#include "MainWindow.h"
#include "SheetSidebar.h"
#include "Central.h"
#include "CentralDocPage.h"
#include "CentralNavPage.h"
#include "DocTabBar.h"
#include "TitleWidget.h"

#undef private
#undef protected

#include <QUuid>
#include <QMimeData>
#include <QPrinter>
#include <DPrintPreviewDialog>

#define CentralDocPage1 mainWindow1->m_central->docPage()
#define CentralDocPage2 mainWindow2->m_central->docPage()

Ut_UiFrame::Ut_UiFrame()
{
}

void Ut_UiFrame::SetUp()
{
    ut_application::SetUp();
}

void Ut_UiFrame::TearDown()
{
}

#ifdef UT_UIFRAME_TEST
TEST_F(Ut_UiFrame, UiFrameTest)
{
    MainWindow *mainWindow1 = MainWindow::createWindow(QStringList() << filePath(UT_FILE_TEST_FILE_1, "UiFrameTest") << filePath(UT_FILE_TEST_FILE_2, "UiFrameTest") << filePath(UT_FILE_TEST_FILE_3, "UiFrameTest"));
    MainWindow *mainWindow2 = MainWindow::createWindow(QStringList() << filePath(UT_FILE_TEST_FILE_4, "UiFrameTest") << filePath(UT_FILE_TEST_FILE_5, "UiFrameTest") << filePath(UT_FILE_TEST_FILE_6, "UiFrameTest"));

    //Central
    ASSERT_TRUE(CentralDocPage1);
    ASSERT_TRUE(mainWindow1->m_central->titleWidget());

    mainWindow1->m_central->titleWidget()->setBtnDisable(true);
    mainWindow1->m_central->titleWidget()->onThumbnailBtnClicked(true);
    mainWindow1->m_central->titleWidget()->onFindOperation(E_FIND_CONTENT);
    mainWindow1->m_central->titleWidget()->onFindOperation(E_FIND_EXIT);
    mainWindow1->m_central->titleWidget()->setControlEnabled(false);

    QStyleOptionTab optionTab;
    QMimeData *mimeData = CentralDocPage1->m_pTabBar->createMimeDataFromTab(0, optionTab);

    CentralDocPage1->m_pTabBar->insertFromMimeData(0, new QMimeData);
    CentralDocPage1->m_pTabBar->insertFromMimeDataOnDragEnter(0, new QMimeData);
    CentralDocPage1->m_pTabBar->insertFromMimeDataOnDragEnter(0, mimeData);

    DocSheet *sheet2 = CentralDocPage1->getSheet(filePath(UT_FILE_TEST_FILE_2, "UiFrameTest"));
    ASSERT_TRUE(sheet2);

    DocSheet *sheet3 = CentralDocPage2->getSheet(filePath(UT_FILE_TEST_FILE_5, "UiFrameTest"));
    ASSERT_TRUE(sheet3);
    sheet3->setSidebarVisible(true);
    sheet3->m_sidebar->onBtnClicked(1);

    DocSheet *sheet4 = CentralDocPage2->getSheet(filePath(UT_FILE_TEST_FILE_4, "UiFrameTest"));
    ASSERT_TRUE(sheet4);
    sheet4->setSidebarVisible(true);
    sheet4->m_sidebar->onBtnClicked(3);

    DocSheet *sheet5 = CentralDocPage1->getSheet(filePath(UT_FILE_TEST_FILE_3, "UiFrameTest"));
    ASSERT_TRUE(sheet5);
    sheet5->setSidebarVisible(true);
    sheet5->m_sidebar->onBtnClicked(2);

    sheet2->setSidebarVisible(true);
    sheet2->m_sidebar->onBtnClicked(4);
    sheet2->handleFindContent("1");

    CentralDocPage1->m_pTabBar->resize(600, 40);
    CentralDocPage1->m_pTabBar->onDragActionChanged(Qt::IgnoreAction);
    CentralDocPage1->m_pTabBar->onDragActionChanged(Qt::CopyAction);
    CentralDocPage1->m_pTabBar->onDragActionChanged(Qt::MoveAction);

    CentralDocPage1->m_pTabBar->onTabDroped(100, Qt::MoveAction, mainWindow2);
    CentralDocPage1->m_pTabBar->onTabDroped(0, Qt::MoveAction, mainWindow2);

    CentralDocPage2->m_pTabBar->canInsertFromMimeData(0, mimeData);
    CentralDocPage2->m_pTabBar->insertFromMimeData(0, mimeData);

    CentralDocPage2->m_pTabBar->onTabReleased(100);
    CentralDocPage2->m_pTabBar->onTabReleased(0);

    CentralDocPage2->m_pTabBar->onTabCloseRequested(100);
    CentralDocPage2->m_pTabBar->onTabCloseRequested(0);

    CentralDocPage2->m_pTabBar->onSetCurrentIndex();
    CentralDocPage2->m_pTabBar->onTabChanged(0);

    QDragEnterEvent dragEnterevent(QPoint(0, 0), Qt::MoveAction, new QMimeData, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(CentralDocPage2->m_pTabBar, &dragEnterevent);

    QResizeEvent resizeEvent(QSize(100, 100), QSize(200, 200));
    QCoreApplication::sendEvent(CentralDocPage2->m_pTabBar, &resizeEvent);

    mainWindow1->resize(600, 800);
    ASSERT_TRUE(mainWindow1->m_central);

    mainWindow1->m_central->addSheet(nullptr);
    mainWindow1->m_central->hasSheet(nullptr);
    mainWindow1->m_central->showSheet(nullptr);
    mainWindow1->m_central->handleShortcut(Dr::key_esc);
    mainWindow1->m_central->zoomIn();
    mainWindow1->m_central->zoomOut();
    mainWindow1->m_central->onSheetCountChanged(0);
    mainWindow1->m_central->onSheetCountChanged(1);
    mainWindow1->m_central->onMenuTriggered("Save");
    mainWindow1->m_central->onMenuTriggered("Magnifer");
    mainWindow1->m_central->onMenuTriggered("Search");
    mainWindow1->m_central->onNeedActivateWindow();
    mainWindow1->m_central->onShowTips("const QString & text, int iconIndex = 0");

    //mainWindow1->m_central->doOpenFile(path);     //崩溃
    //mainWindow1->m_central->openFiles(QStringList() << path); //崩溃
    //mainWindow1->m_central->m_navPage->onChooseButtonClicked(); //阻塞
    //mainWindow1->m_central->openFilesExec(); //阻塞
    //mainWindow1->m_central->onMenuTriggered("Display in file manager"); //打开外部文件夹
    //mainWindow1->m_central->onMenuTriggered("Save as");       //阻塞

    mimeData->setData("deepin_reader/tabbar", "0");
    QDropEvent dropEnterevent(QPoint(0, 0), Qt::MoveAction, mimeData, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(mainWindow1->m_central, &dropEnterevent);

    DocSheet *sheet = CentralDocPage1->getSheet(filePath(UT_FILE_TEST_FILE_3, "UiFrameTest"));
    ASSERT_TRUE(sheet);
    EXPECT_TRUE(mainWindow1->m_central->hasSheet(sheet));
    EXPECT_TRUE(mainWindow1->m_central->saveAll());
    sheet->saveAsData(filePath(UT_FILE_PDF, "UiFrameTest"));
    sheet->setSidebarVisible(true);
    sheet->m_sidebar->onBtnClicked(3);

    //CentralDocPage
    sheet->setBookMark(0, !sheet->hasBookMark(0));
    EXPECT_FALSE(CentralDocPage1->firstThumbnail(filePath(UT_FILE_PDF, "UiFrameTest"), "/home/samson/Desktop/1.png"));
    EXPECT_FALSE(CentralDocPage1->isFullScreen());
    EXPECT_FALSE(CentralDocPage1->quitFullScreen());
    EXPECT_TRUE(CentralDocPage1->getTitleLabel());

    CentralDocPage1->m_pTabBar->removeTab(0);
    CentralDocPage1->getCurSheet();
    CentralDocPage1->saveCurrent();
    CentralDocPage1->addSheet(nullptr);
    CentralDocPage1->enterSheet(nullptr);
    CentralDocPage1->leaveSheet(nullptr);
    CentralDocPage1->hasSheet(nullptr);
    CentralDocPage1->showSheet(nullptr);
    CentralDocPage1->showSheet(sheet);
    //CentralDocPage1->openFile(filePath(UT_FILE_PDF, "UiFrameTest"));   //崩溃
    //CentralDocPage1->openCurFileFolder();     //打开文件夹
    //CentralDocPage1->saveAll();               //卡住

    sheet->openSlide();
    CentralDocPage1->handleShortcut(Dr::key_esc);

    sheet->openMagnifier();
    CentralDocPage1->handleShortcut(Dr::key_esc);

    sheet->openFullScreen();
    CentralDocPage1->handleShortcut(Dr::key_esc);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_s);
    sheet->closeSlide();
    CentralDocPage1->handleShortcut(Dr::key_alt_z);
    sheet->closeMagnifier();
    CentralDocPage1->handleShortcut(Dr::key_f5);
    CentralDocPage1->handleShortcut(Dr::key_alt_2);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_m);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_2);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_3);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_r);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_shift_r);
    CentralDocPage1->handleShortcut(Dr::key_alt_harger);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_equal);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_smaller);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_d);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_f);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_c);
    CentralDocPage1->handleShortcut(Dr::key_alt_h);
    CentralDocPage1->handleShortcut(Dr::key_alt_a);
    CentralDocPage1->handleShortcut(Dr::key_left);
    CentralDocPage1->handleShortcut(Dr::key_right);
    CentralDocPage1->handleShortcut(Dr::key_f11);
    sheet->closeFullScreen();

    CentralDocPage1->handleShortcut(Dr::key_alt_1);
    CentralDocPage1->handleShortcut(Dr::key_ctrl_1);
    CentralDocPage1->showTips("Test", 0);
    CentralDocPage1->showTips("Test", 1);
    CentralDocPage1->openMagnifer();
    CentralDocPage1->quitMagnifer();
    CentralDocPage1->openSlide();
    CentralDocPage1->quitSlide();
    CentralDocPage1->handleSearch();
    CentralDocPage1->zoomIn();
    CentralDocPage1->zoomOut();
    CentralDocPage1->onTabChanged(nullptr);
    CentralDocPage1->onTabChanged(sheet);
    CentralDocPage1->onTabMoveIn(nullptr);
    CentralDocPage1->onTabMoveIn(sheet);
    CentralDocPage1->onTabClosed(nullptr);
    CentralDocPage1->onTabMoveOut(nullptr);
    CentralDocPage1->onCentralMoveIn(nullptr);
    CentralDocPage1->onSheetFileChanged(nullptr);
    CentralDocPage1->onSheetOperationChanged(nullptr);
    CentralDocPage1->onSheetCountChanged(0);
    CentralDocPage1->onSheetCountChanged(1);
    CentralDocPage1->onOpened(nullptr, true);
    CentralDocPage1->onOpened(nullptr, false);

    sheet->saveData();
    sheet->existFileChanged();

    //DocSheet
    EXPECT_FALSE(sheet->firstThumbnail(filePath(UT_FILE_PDF, "UiFrameTest")).isNull());

    const QString &uuid = DocSheet::getUuid(sheet).toString();
    EXPECT_EQ(DocSheet::getSheet(uuid), sheet);
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
    if (sheet->operation().layoutMode == Dr::SinglePageMode) {
        EXPECT_TRUE(sheet->currentIndex() == 1);
        EXPECT_TRUE(sheet->currentPage() == 2);
    } else {
        EXPECT_TRUE(sheet->currentIndex() == 2);
        EXPECT_TRUE(sheet->currentPage() == 3);
    }

    sheet->jumpToLastPage();
    sheet->jumpToNextPage();
    EXPECT_TRUE(sheet->currentIndex() == sheet->pagesNumber() - 1);
    EXPECT_TRUE(sheet->currentPage() == sheet->pagesNumber());

    sheet->jumpToPrevPage();

    DPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    sheet->onPrintRequested(&printer);
    if (sheet->operation().layoutMode == Dr::SinglePageMode) {
        EXPECT_TRUE(sheet->currentIndex() == sheet->pagesNumber() - 2);
        EXPECT_TRUE(sheet->currentPage() == sheet->pagesNumber() - 1);
    } else {
        EXPECT_TRUE(sheet->currentIndex() == sheet->pagesNumber() - 3);
        EXPECT_TRUE(sheet->currentPage() == sheet->pagesNumber() - 2);
    }

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
    //EXPECT_TRUE(sheet->saveData());       //阻塞

    QImage imageTest;
    EXPECT_TRUE(sheet->getImage(0, imageTest, 100, 100));
    EXPECT_FALSE(imageTest.isNull());

    EXPECT_TRUE(sheet->annotations().size() >= 0);
    EXPECT_TRUE(sheet->scaleFactorList().size() > 0);

    EXPECT_TRUE(sheet->maxScaleFactor() >= 0.1 && sheet->maxScaleFactor() <= 5.0);
    EXPECT_TRUE(sheet->filter() == "Pdf File (*.pdf)");

    sheet->format();
    sheet->getBookMarkList().size();
    sheet->setLayoutMode(Dr::SinglePageMode);
    sheet->operation();
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
    EXPECT_TRUE(sheet->filePath() == filePath(UT_FILE_TEST_FILE_3, "UiFrameTest"));

    sheet->setBookMark(0, 0);
    EXPECT_FALSE(sheet->hasBookMark(0));

    sheet->openFullScreen();
    EXPECT_TRUE(sheet->isFullScreen());
    sheet->closeFullScreen();

    EXPECT_FALSE(sheet->needPassword());
    EXPECT_TRUE(sheet->isUnLocked());

    sheet->jumpToOutline(0, 0, 0);
    sheet->jumpToOutline(0, 0, -1);
    sheet->jumpToOutline(0, 0, 10000);

    sheet->jumpToHighLight(nullptr, 0);
    sheet->rotateLeft();
    sheet->rotateRight();

    sheet->setBookMarks(QList<int>(), 1);
    sheet->setAnnotationInserting(true);
    sheet->openMagnifier();
    sheet->closeMagnifier();
    sheet->handleSearch();
    sheet->stopSearch();
    sheet->copySelectedText();
    sheet->highlightSelectedText();
    sheet->addSelectedTextHightlightAnnotation();
    sheet->defaultFocus();
    sheet->removeAnnotation(nullptr);
    sheet->removeAllAnnotation();
    sheet->scaleFactorList();
    sheet->maxScaleFactor();
    sheet->filter();
    sheet->format();
    sheet->getBookMarkList();
    sheet->operation();
    sheet->fileType();
    sheet->filePath();
    sheet->hasBookMark(0);
    sheet->hasBookMark(-1);
    sheet->hasBookMark(10000);
    sheet->zoomin();
    sheet->zoomout();
    sheet->setSidebarVisible(true);
    sheet->setSidebarVisible(false);
    sheet->handleOpenSuccess();
    sheet->openSlide();
    sheet->closeSlide();
    sheet->isFullScreen();
    sheet->openFullScreen();
    sheet->closeFullScreen();
    sheet->setDocumentChanged(false);
    sheet->setBookmarkChanged(false);
    sheet->setOperationChanged();
    sheet->handleFindNext();
    sheet->handleFindPrev();
    sheet->handleFindContent("const QString & strFind");
    sheet->handleFindExit();
    sheet->showEncryPage();
    sheet->tryPassword("");
    sheet->needPassword();
    sheet->isUnLocked();
    sheet->operationRef();
    sheet->showTips("test", 0);
    sheet->onFindContentComming(deepin_reader::SearchResult());
    sheet->onFindFinished();
    sheet->haslabel();

    deepin_reader::FileInfo tFileInfo;
    sheet->docBasicInfo(tFileInfo);

    sheet->onBrowserPageChanged(0);
    sheet->onBrowserPageChanged(-1);
    sheet->onBrowserPageChanged(10000);

    sheet->onBrowserPageFirst();
    sheet->onBrowserPagePrev();
    sheet->onBrowserPageNext();
    sheet->onBrowserPageLast();

    sheet->onBrowserOperaAnnotation(0, 0, nullptr);
    sheet->onBrowserOperaAnnotation(1, 0, nullptr);
    sheet->onBrowserOperaAnnotation(2, 0, nullptr);

    sheet->onSideAniFinished();
    sheet->getPageLabelByIndex(0);
    sheet->getIndexByPageLable("0");
    sheet->pageSizeByIndex(0);

    QChildEvent childEvent(QEvent::ChildRemoved, sheet);
    QCoreApplication::sendEvent(sheet, &childEvent);

    mainWindow1->closeWithoutSave();
    mainWindow2->closeWithoutSave();

    exec();
}
#endif
