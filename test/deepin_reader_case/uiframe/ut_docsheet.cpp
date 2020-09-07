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
#include "ut_docsheet.h"

#define private public
#define protected public

#include "MainWindow.h"
#include "SheetSidebar.h"
#include "CentralDocPage.h"

#undef private
#undef protected

#include "Central.h"

#include <QUuid>

Ut_DocSheet::Ut_DocSheet()
{
}

void Ut_DocSheet::SetUp()
{
}

void Ut_DocSheet::TearDown()
{
}

#ifdef UT_DOCSHEET_TEST
TEST_F(Ut_DocSheet, DocSheetTest)
{
    const QString &path = UT_FILE_TEST_FILE;
    MainWindow *mainWindow = MainWindow::createWindow(QStringList() << path);
    mainWindow->resize(600, 800);
    ASSERT_TRUE(mainWindow->m_central);

    CentralDocPage *docpage = mainWindow->m_central->docPage();
    //Central
    ASSERT_TRUE(docpage);
    ASSERT_TRUE(mainWindow->m_central->titleMenu());
    ASSERT_TRUE(mainWindow->m_central->titleWidget());

    mainWindow->m_central->openFiles(QStringList() << path);
    mainWindow->m_central->addSheet(0);
    mainWindow->m_central->hasSheet(0);
    mainWindow->m_central->showSheet(0);
    mainWindow->m_central->handleShortcut(Dr::key_esc);
    mainWindow->m_central->doOpenFile(path);
    mainWindow->m_central->zoomIn();
    mainWindow->m_central->zoomOut();
    mainWindow->m_central->onSheetCountChanged(0);
    mainWindow->m_central->onSheetCountChanged(1);
    mainWindow->m_central->onMenuTriggered("Save");
    mainWindow->m_central->onMenuTriggered("Magnifer");
    mainWindow->m_central->onMenuTriggered("Display in file manager");
    mainWindow->m_central->onMenuTriggered("Search");
    mainWindow->m_central->onNeedActivateWindow();
    mainWindow->m_central->onShowTips("const QString & text, int iconIndex = 0");

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

    docpage->openFile(path);
    docpage->saveAll();
    docpage->addSheet(0);
    docpage->enterSheet(0);
    docpage->leaveSheet(0);
    docpage->hasSheet(0);
    docpage->showSheet(0);
    docpage->showSheet(sheet);

    sheet->openSlide();
    docpage->handleShortcut(Dr::key_esc);

    sheet->openMagnifier();
    docpage->handleShortcut(Dr::key_esc);

    sheet->openFullScreen();
    docpage->handleShortcut(Dr::key_esc);
    docpage->handleShortcut(Dr::key_ctrl_s);
    docpage->handleShortcut(Dr::key_f5);
    sheet->closeSlide();
    docpage->handleShortcut(Dr::key_alt_z);
    sheet->closeMagnifier();
    docpage->handleShortcut(Dr::key_alt_1);
    docpage->handleShortcut(Dr::key_alt_2);
    docpage->handleShortcut(Dr::key_ctrl_1);
    docpage->handleShortcut(Dr::key_ctrl_m);
    docpage->handleShortcut(Dr::key_ctrl_2);
    docpage->handleShortcut(Dr::key_ctrl_3);
    docpage->handleShortcut(Dr::key_ctrl_r);
    docpage->handleShortcut(Dr::key_ctrl_shift_r);
    docpage->handleShortcut(Dr::key_alt_harger);
    docpage->handleShortcut(Dr::key_ctrl_equal);
    docpage->handleShortcut(Dr::key_ctrl_smaller);
    docpage->handleShortcut(Dr::key_ctrl_d);
    docpage->handleShortcut(Dr::key_ctrl_f);
    docpage->handleShortcut(Dr::key_ctrl_c);
    docpage->handleShortcut(Dr::key_alt_h);
    docpage->handleShortcut(Dr::key_alt_a);
    docpage->handleShortcut(Dr::key_left);
    docpage->handleShortcut(Dr::key_right);
    docpage->handleShortcut(Dr::key_f11);
    sheet->closeFullScreen();

    docpage->showTips("Test", 0);
    docpage->showTips("Test", 1);
    docpage->openMagnifer();
    docpage->quitMagnifer();
    docpage->openSlide();
    docpage->quitSlide();
    docpage->handleSearch();

    docpage->zoomIn();
    docpage->zoomOut();

    docpage->onTabChanged(0);
    docpage->onTabChanged(sheet);
    docpage->onTabMoveIn(0);
    docpage->onTabMoveIn(sheet);

    docpage->onTabClosed(0);
    docpage->onTabMoveOut(0);
    docpage->onCentralMoveIn(0);
    docpage->onSheetFileChanged(0);
    docpage->onSheetOperationChanged(0);

    docpage->onSheetCountChanged(0);
    docpage->onSheetCountChanged(1);
    docpage->getDocTabbarText(0);
    docpage->getDocTabbarText(1);
    docpage->getDocTabbarText(-1);
    docpage->getDocTabbarText(10000);

    docpage->onOpened(0, true);
    docpage->onOpened(0, false);
    sheet->saveData();
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

    sheet->jumpToOutline(0, 0, 0);
    sheet->jumpToOutline(0, 0, -1);
    sheet->jumpToOutline(0, 0, 10000);

    sheet->jumpToHighLight(0, 0);
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
    sheet->removeAnnotation(0);
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
    sheet->sideBarVisible();

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

    sheet->getPageLableIndex("1");

    sheet->operationRef();
    sheet->showTips("test", 0);
    sheet->onFindContentComming(deepin_reader::SearchResult());
    sheet->onFindFinished();

    sheet->label2pagenum("1");
    sheet->haslabel();

    deepin_reader::FileInfo tFileInfo;
    sheet->docBasicInfo(tFileInfo);

    sheet->pagenum2label(0);
    sheet->pagenum2label(-1);
    sheet->pagenum2label(10000);

    sheet->onBrowserPageChanged(0);
    sheet->onBrowserPageChanged(-1);
    sheet->onBrowserPageChanged(10000);

    sheet->onBrowserPageFirst();
    sheet->onBrowserPagePrev();
    sheet->onBrowserPageNext();
    sheet->onBrowserPageLast();

    sheet->onBrowserOperaAnnotation(0, 0, 0);
    sheet->onBrowserOperaAnnotation(1, 0, 0);
    sheet->onBrowserOperaAnnotation(2, 0, 0);
    sheet->onExtractPassword("123");

    sheet->saveData();
    mainWindow->close();
}
#endif
