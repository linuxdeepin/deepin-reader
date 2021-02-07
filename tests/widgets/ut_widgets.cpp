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
#include "ut_widgets.h"

#define private public
#define protected public

#include "widgets/ScaleMenu.h"
#include "widgets/FindWidget.h"
#include "widgets/BaseWidget.h"
#include "widgets/EncryptionPage.h"
#include "widgets/ColorWidgetAction.h"
#include "widgets/HandleMenu.h"
#include "widgets/ShortCutShow.h"
#include "widgets/SlidePlayWidget.h"
#include "widgets/SlideWidget.h"
#include "widgets/ScaleWidget.h"
#include "widgets/RoundColorWidget.h"
#include "widgets/SaveDialog.h"
#include "widgets/AttrScrollWidget.h"

#undef private
#undef protected

#define private public
#define protected public
#include "MainWindow.h"
#include "CentralDocPage.h"
#include "Application.h"

#include "widgets/TipsWidget.h"
#include "widgets/WordWrapLabel.h"
#include "widgets/FileAttrWidget.h"

#include "sidebar/SheetSidebar.h"
#undef private
#undef protected

#include "Central.h"
#include "DocSheet.h"

Ut_Widgets::Ut_Widgets()
{
}

void Ut_Widgets::SetUp()
{
    ut_application::SetUp();
}

void Ut_Widgets::TearDown()
{
}

TEST_F(Ut_Widgets, SheetWidgetTest)
{
    QString path = filePath(UT_FILE_TEST_FILE_1, "SheetWidgetTest");

    MainWindow *mainWindow = MainWindow::createWindow(QStringList() << path);

    ASSERT_TRUE(mainWindow->m_central);

    DocSheet *sheet = new DocSheet(Dr::PDF, filePath(UT_FILE_PDF, "SheetWidgetTest"));

    ASSERT_TRUE(sheet->openFileExec(""));

    mainWindow->show();

    mainWindow->addSheet(sheet);

    CentralDocPage *docpage = mainWindow->m_central->docPage();

    ASSERT_TRUE(docpage);

    ASSERT_TRUE(sheet);

    sheet->setSidebarVisible(true);

    sheet->m_sidebar->onBtnClicked(4);

    sheet->startSearch("12");

    //ScaleMenu
    {
        ScaleMenu scaleMenu;
        scaleMenu.readCurDocParam(nullptr);
        scaleMenu.onTwoPage();
        scaleMenu.onFiteH();
        scaleMenu.onFiteW();
        scaleMenu.onDefaultPage();
        scaleMenu.onFitPage();
        scaleMenu.onScaleFactor();

        scaleMenu.readCurDocParam(sheet);
        scaleMenu.onTwoPage();
        scaleMenu.onFiteH();
        scaleMenu.onFiteW();
        scaleMenu.onDefaultPage();
        scaleMenu.onFitPage();
        scaleMenu.onScaleFactor();
    }

    //FileAttrWidget
    {
        class ImageWidget : public DWidget
        {
        };

        FileAttrWidget *filewidget = new FileAttrWidget();
        filewidget->setAttribute(Qt::WA_ShowModal, false);
        filewidget->setFileAttr(nullptr);
        filewidget->setFileAttr(sheet);
        filewidget->addTitleFrame("");
        filewidget->resize(600, 600);
        filewidget->showScreenCenter();

        QPaintEvent paintevent(QRect(0, 0, 100, 100));
        filewidget->paintEvent(&paintevent);

        delete filewidget;
    }

    //SlideWidget
    {
        SlideWidget *slidewidget = new SlideWidget(sheet);
        slidewidget->resize(600, 400);
        slidewidget->show();
        slidewidget->playImage();
        slidewidget->drawImage(QPixmap());

        slidewidget->onImagevalueChanged(0);
        slidewidget->onImageShowTimeOut();

        slidewidget->onFetchImage(0);
        slidewidget->onUpdatePageImage(0);
        slidewidget->onPlayBtnClicked();
        slidewidget->handleKeyPressEvent(Dr::key_space);
        slidewidget->handleKeyPressEvent(Dr::key_left);
        slidewidget->handleKeyPressEvent(Dr::key_right);

        slidewidget->onParentDestroyed();
        slidewidget->onPreBtnClicked();
        slidewidget->onPlayBtnClicked();
        slidewidget->onNextBtnClicked();
        slidewidget->onExitBtnClicked();

        slidewidget->setWidgetState(true);
        slidewidget->onParentDestroyed();
        slidewidget->setWidgetState(false);
        slidewidget->onImageAniFinished();

        QMouseEvent mousemoveevent(QEvent::MouseMove, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QCoreApplication::sendEvent(slidewidget, &mousemoveevent);

        QMouseEvent mouseLPevent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        QCoreApplication::sendEvent(slidewidget, &mouseLPevent);

        slidewidget->repaint();

        delete slidewidget;
    }

    //HandleMenu
    {
        HandleMenu handleMenu;
        handleMenu.readCurDocParam(nullptr);
        handleMenu.onHandTool();
        handleMenu.onSelectText();

        handleMenu.readCurDocParam(sheet);
        handleMenu.onHandTool();
        handleMenu.onSelectText();
    }

    //ScaleWidget
    {
        ScaleWidget scaleWidget;
        scaleWidget.setSheet(nullptr);
        scaleWidget.clear();
        scaleWidget.onPrevScale();
        scaleWidget.onNextScale();
        scaleWidget.onReturnPressed();
        scaleWidget.onEditFinished();
        //        scaleWidget.onArrowBtnlicked();       //会等待

        scaleWidget.setSheet(sheet);
        scaleWidget.clear();
        scaleWidget.onPrevScale();
        scaleWidget.onNextScale();
        scaleWidget.onReturnPressed();
        scaleWidget.onEditFinished();
    }

    //FindWidget
    {
        FindWidget findWidget;
        findWidget.setDocSheet(nullptr);
        findWidget.showPosition(0);
        findWidget.setSearchEditFocus();
        findWidget.setEditAlert(true);
        findWidget.setEditAlert(false);
        findWidget.onSearchStart();
        findWidget.slotFindNextBtnClicked();
        findWidget.slotFindPrevBtnClicked();
        findWidget.onSearchStop();
        findWidget.slotClearContent();
        findWidget.onCloseBtnClicked();

        findWidget.setDocSheet(sheet);
        findWidget.slotFindNextBtnClicked();
        findWidget.slotFindPrevBtnClicked();
        findWidget.onSearchStop();
        findWidget.slotClearContent();
        findWidget.onCloseBtnClicked();

        QKeyEvent sidekeyLevent(QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier);
        QCoreApplication::sendEvent(&findWidget, &sidekeyLevent);

        QKeyEvent sidekeyUpLevent(QEvent::KeyPress, Qt::Key_Up, Qt::ControlModifier);
        QCoreApplication::sendEvent(&findWidget, &sidekeyUpLevent);
    }

    sheet->saveData();

    exec();
}

TEST_F(Ut_Widgets, BColorWidgetActionTest)
{
    ColorWidgetAction colorAction;
    colorAction.slotBtnClicked(0);
}

TEST_F(Ut_Widgets, BaseWidgetTest)
{
    BaseWidget cusWidget(nullptr);
    cusWidget.adaptWindowSize(1.0);
    cusWidget.updateWidgetTheme();
}

TEST_F(Ut_Widgets, EncryptionPageTest)
{
    EncryptionPage encryPage;
    encryPage.nextbuttonClicked();
    encryPage.wrongPassWordSlot();
    encryPage.onPasswordChanged();
    encryPage.onUpdateTheme();
}

TEST_F(Ut_Widgets, SaveDialogTest)
{
    //    a->exec();
    SaveDialog saveDialog;
    //    saveDialog.showTipDialog("1111111");
    //    saveDialog.showExitDialog();
}

TEST_F(Ut_Widgets, RoundColorWidgetTest)
{
    RoundColorWidget roundColorWidget(Qt::red);
    roundColorWidget.show();
    roundColorWidget.setSelected(false);
    roundColorWidget.setSelected(true);
    roundColorWidget.setAllClickNotify(true);
    roundColorWidget.resize(600, 600);
    roundColorWidget.repaint();
    QMouseEvent mouseLPevent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    roundColorWidget.mousePressEvent(&mouseLPevent);

    QPaintEvent paintevent(QRect(0, 0, 100, 100));
    roundColorWidget.paintEvent(&paintevent);
}

TEST_F(Ut_Widgets, ShortCutShowTest)
{
    ShortCutShow shortCutDialog;
    shortCutDialog.setSheet(nullptr);
    shortCutDialog.initDJVU();
    //shortCutDialog.show();
}

TEST_F(Ut_Widgets, SlidePlayWidgetTest)
{
    SlidePlayWidget slidePlaywidget;
    slidePlaywidget.showControl();
    slidePlaywidget.setPlayStatus(true);
    EXPECT_TRUE(slidePlaywidget.getPlayStatus());
    slidePlaywidget.setPlayStatus(false);
    EXPECT_FALSE(slidePlaywidget.getPlayStatus());
    EXPECT_TRUE(slidePlaywidget.createBtn("test"));
    slidePlaywidget.playStatusChanged();
    slidePlaywidget.onTimerout();
    slidePlaywidget.onPreClicked();
    slidePlaywidget.onPlayClicked();
    slidePlaywidget.onNextClicked();
    slidePlaywidget.onExitClicked();

    QMouseEvent mouseLPevent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&slidePlaywidget, &mouseLPevent);

    QMouseEvent mouseRPevent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::RightButton, Qt::NoButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&slidePlaywidget, &mouseLPevent);

    QEvent hoverE(QEvent::Enter);
    QCoreApplication::sendEvent(&slidePlaywidget, &hoverE);

    QEvent hoverL(QEvent::Leave);
    QCoreApplication::sendEvent(&slidePlaywidget, &hoverL);
}

TEST_F(Ut_Widgets, TipsWidgetTest)
{
    TipsWidget tipsWidget;
    tipsWidget.show();
    tipsWidget.setText("test");
    tipsWidget.resize(200, 100);
    tipsWidget.setAutoChecked(true);
    tipsWidget.setAlignment(Qt::AlignCenter);
    tipsWidget.setLeftRightMargin(-1);
    tipsWidget.setTopBottomMargin(-1);
    tipsWidget.setMaxLineCount(0);
    tipsWidget.adjustContent("const QString & text");
    tipsWidget.onUpdateTheme();
    tipsWidget.onTimeOut();

    QPaintEvent paintevent(QRect(0, 0, 100, 100));
    tipsWidget.paintEvent(&paintevent);

    QHideEvent hideEvent;
    QCoreApplication::sendEvent(&tipsWidget, &hideEvent);

    QShowEvent showEvent;
    QCoreApplication::sendEvent(&tipsWidget, &showEvent);
}

TEST_F(Ut_Widgets, WordWrapLabelTest)
{
    WordWrapLabel wordLabel;
    wordLabel.show();
    wordLabel.setFixedWidth(200);
    wordLabel.resize(200, 100);
    wordLabel.setText("test_111111111111111111111111111111111");
    wordLabel.setMargin(0);
    wordLabel.adjustContent();

    QPaintEvent paintevent(QRect(0, 0, 100, 100));
    wordLabel.paintEvent(&paintevent);
}
