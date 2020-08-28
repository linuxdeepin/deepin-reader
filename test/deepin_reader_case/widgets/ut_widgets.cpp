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
#include "widgets/AttrScrollWidget.h"
#include "widgets/BookMarkButton.h"
#include "widgets/ColorWidgetAction.h"
#include "widgets/CustomClickLabel.h"
#include "widgets/CustemPushButton.h"
#include "widgets/CustomMenu.h"
#include "widgets/CustomWidget.h"
#include "widgets/EncryptionPage.h"
#include "widgets/FileAttrWidget.h"
#include "widgets/FindWidget.h"
#include "widgets/HandleMenu.h"
#include "widgets/PrintManager.h"
#include "widgets/RoundColorWidget.h"
#include "widgets/SaveDialog.h"
#include "widgets/ScaleMenu.h"
#include "widgets/ScaleWidget.h"
#include "widgets/ShortCutShow.h"
#include "widgets/SlidePlayWidget.h"
#include "widgets/SlideWidget.h"
#include "widgets/SpinnerWidget.h"
#include "widgets/TipsWidget.h"
#include "widgets/WordWrapLabel.h"

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
TEST(Ut_Widgets, BookMarkButtonTest)
{
    BookMarkButton bkBtn;
}

TEST(Ut_Widgets, BColorWidgetActionTest)
{
    ColorWidgetAction colorAction;
}

TEST(Ut_Widgets, CustemPushButtonTest)
{
    CustemPushButton cusBtn;
}

TEST(Ut_Widgets, CustomClickLabelTest)
{
    CustomClickLabel clickLabel;
}

TEST(Ut_Widgets, CustomMenuTest)
{
    CustomMenu menu;
    EXPECT_TRUE(menu.createAction("test", nullptr, true));
}

TEST(Ut_Widgets, CustomWidgetTest)
{
    CustomWidget cusWidget;
}

TEST(Ut_Widgets, EncryptionPageTest)
{
    EncryptionPage encryPage;
}

TEST(Ut_Widgets, FileAttrWidgetTest)
{
    FileAttrWidget encryPage;
}

TEST(Ut_Widgets, FindWidgetTest)
{
    FindWidget findWidget;
}

TEST(Ut_Widgets, HandleMenuTest)
{
    HandleMenu handleMenu;
}

TEST(Ut_Widgets, SaveDialogTest)
{
    SaveDialog saveDialog;
}

TEST(Ut_Widgets, ScaleMenuTest)
{
    ScaleMenu scaleMenu;
}

TEST(Ut_Widgets, RoundColorWidgetTest)
{
    RoundColorWidget roundColorWidget(Qt::red);
    roundColorWidget.setSelected(true);
    roundColorWidget.setAllClickNotify(false);
    EXPECT_EQ(roundColorWidget.isSelected(), true);
}

TEST(Ut_Widgets, ShortCutShowTest)
{
    ShortCutShow shortCutDialog;
}

TEST(Ut_Widgets, SlidePlayWidgetTest)
{
    SlidePlayWidget slidePlaywidget;
}

TEST(Ut_Widgets, SpinnerWidgetTest)
{
    SpinnerWidget spinnerWidget;
}

TEST(Ut_Widgets, ScaleWidgetTest)
{
    ScaleWidget scaleWidget;
}

TEST(Ut_Widgets, TipsWidgetTest)
{
    TipsWidget tipsWidget;
}

TEST(Ut_Widgets, WordWrapLabelTest)
{
    WordWrapLabel wordLabel;
}
#endif
