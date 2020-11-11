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
#include "widgets/CustomMenu.h"
#include "widgets/FindWidget.h"
#include "widgets/CustomWidget.h"
#include "widgets/EncryptionPage.h"
#include "widgets/ColorWidgetAction.h"
#include "widgets/HandleMenu.h"
#include "widgets/ShortCutShow.h"
#include "widgets/SlidePlayWidget.h"
#include "widgets/SlideWidget.h"
#include "widgets/ScaleWidget.h"
#include "widgets/RoundColorWidget.h"
#include "widgets/SaveDialog.h"
#include "widgets/SpinnerWidget.h"
#include "widgets/SlideWidget.h"
#include "widgets/AttrScrollWidget.h"

#undef private
#undef protected

#define private public
#include "MainWindow.h"
#include "CentralDocPage.h"

#include "widgets/TipsWidget.h"
#include "widgets/WordWrapLabel.h"
#include "widgets/FileAttrWidget.h"
#undef private

#include "Central.h"
#include "DocSheet.h"

Ut_Widgets::Ut_Widgets()
{
}

void Ut_Widgets::SetUp()
{
}

void Ut_Widgets::TearDown()
{
}

#ifdef UT_WIDGETS_TEST
TEST(Ut_Widgets, SheetWidgetTest)
{
    QString path = UT_FILE_TEST_FILE;
    MainWindow *mainWindow = MainWindow::createWindow(QStringList() << path);
    ASSERT_TRUE(mainWindow->m_central);
    mainWindow->show();

    CentralDocPage *docpage = mainWindow->m_central->docPage();
    //Central
    ASSERT_TRUE(docpage);

    DocSheet *sheet = docpage->getSheet(path);
    ASSERT_TRUE(sheet);

    //ScaleMenu
    {
        ScaleMenu scaleMenu;
        scaleMenu.readCurDocParam(0);
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

    //    //FileAttrWidget
    {
        FileAttrWidget filewidget;
        filewidget.setAttribute(Qt::WA_ShowModal, false);
        filewidget.setFileAttr(0);
        filewidget.setFileAttr(sheet);
        filewidget.addTitleFrame("");
        filewidget.resize(600, 600);
        filewidget.showScreenCenter();
    }

    //PrintManager
    //PrintManager printManager(sheet);

    //SlideWidget
    {
        SlideWidget *slidewidget = new SlideWidget(sheet);
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

        slidewidget->onPreBtnClicked();
        slidewidget->onPlayBtnClicked();
        slidewidget->onNextBtnClicked();
        slidewidget->onExitBtnClicked();

        slidewidget->onParentDestroyed();
        slidewidget->close();
    }

    //HandleMenu
    {
        HandleMenu handleMenu;
        handleMenu.readCurDocParam(0);
        handleMenu.onHandTool();
        handleMenu.onSelectText();

        handleMenu.readCurDocParam(sheet);
        handleMenu.onHandTool();
        handleMenu.onSelectText();
    }

    //ScaleWidget
    {
        ScaleWidget scaleWidget;
        scaleWidget.setSheet(0);
        scaleWidget.clear();
        scaleWidget.onPrevScale();
        scaleWidget.onNextScale();
        scaleWidget.onReturnPressed();
        scaleWidget.onEditFinished();
        scaleWidget.onArrowBtnlicked();

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
        findWidget.setDocSheet(0);
        findWidget.showPosition(0);
        findWidget.setSearchEditFocus();
        findWidget.setEditAlert(true);
        findWidget.setEditAlert(false);
        findWidget.handleContentChanged();
        findWidget.slotFindNextBtnClicked();
        findWidget.slotFindPrevBtnClicked();
        findWidget.slotEditAborted();
        findWidget.slotClearContent();
        findWidget.stopSearch();
        findWidget.findCancel();

        findWidget.setDocSheet(sheet);
        findWidget.slotFindNextBtnClicked();
        findWidget.slotFindPrevBtnClicked();
        findWidget.slotEditAborted();
        findWidget.slotClearContent();
        findWidget.stopSearch();
        findWidget.findCancel();

        QKeyEvent sidekeyLevent(QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier);
        QCoreApplication::sendEvent(&findWidget, &sidekeyLevent);

        QKeyEvent sidekeyUpLevent(QEvent::KeyPress, Qt::Key_Up, Qt::ControlModifier);
        QCoreApplication::sendEvent(&findWidget, &sidekeyUpLevent);
    }

    sheet->saveData();
}

TEST(Ut_Widgets, BColorWidgetActionTest)
{
    ColorWidgetAction colorAction;
    colorAction.slotBtnClicked(0);
}

TEST(Ut_Widgets, CustomMenuTest)
{
    CustomMenu menu;
    EXPECT_TRUE(menu.createAction("test", nullptr, true));
}

TEST(Ut_Widgets, CustomWidgetTest)
{
    CustomWidget cusWidget;
    cusWidget.adaptWindowSize(1.0);
    cusWidget.updateThumbnail(-1);
    cusWidget.updateThumbnail(0);
    cusWidget.updateThumbnail(10000);
    cusWidget.showMenu();
    cusWidget.initWidget();
    cusWidget.updateWidgetTheme();
}

TEST(Ut_Widgets, EncryptionPageTest)
{
    EncryptionPage encryPage;
    encryPage.nextbuttonClicked();
    encryPage.wrongPassWordSlot();
    encryPage.onPasswordChanged();
    encryPage.onUpdateTheme();
}

TEST(Ut_Widgets, SaveDialogTest)
{
    SaveDialog saveDialog;
    saveDialog.showTipDialog("1111111");
    saveDialog.showExitDialog();
}

TEST(Ut_Widgets, RoundColorWidgetTest)
{
    RoundColorWidget roundColorWidget(Qt::red);
    roundColorWidget.setSelected(false);
    roundColorWidget.setSelected(true);
    roundColorWidget.setAllClickNotify(true);
    roundColorWidget.resize(600, 600);
    roundColorWidget.repaint();
    roundColorWidget.show();

    QMouseEvent mouseLPevent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&roundColorWidget, &mouseLPevent);
}

TEST(Ut_Widgets, ShortCutShowTest)
{
    ShortCutShow shortCutDialog;
    shortCutDialog.setSheet(0);
    shortCutDialog.initDJVU();
    shortCutDialog.show();
}

TEST(Ut_Widgets, SlidePlayWidgetTest)
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

TEST(Ut_Widgets, SpinnerWidgetTest)
{
    SpinnerWidget spinnerWidget;
    spinnerWidget.startSpinner();
    spinnerWidget.stopSpinner();
    spinnerWidget.setSpinnerSize(QSize(100, 100));
}

TEST(Ut_Widgets, TipsWidgetTest)
{
    TipsWidget tipsWidget;
    tipsWidget.show();
    tipsWidget.setText("test");
    tipsWidget.setAutoChecked(true);
    tipsWidget.setAlignment(Qt::AlignCenter);
    tipsWidget.setLeftRightMargin(-1);
    tipsWidget.setTopBottomMargin(-1);
    tipsWidget.setMaxLineCount(0);
    tipsWidget.adjustContent("const QString & text");
    tipsWidget.onUpdateTheme();
    tipsWidget.onTimeOut();
}

TEST(Ut_Widgets, WordWrapLabelTest)
{
    WordWrapLabel wordLabel;
    wordLabel.show();
    wordLabel.setFixedWidth(200);
    wordLabel.resize(200, 100);
    wordLabel.setText("test_111111111111111111111111111111111");
    wordLabel.setMargin(0);
    wordLabel.adjustContent();
}
#endif
