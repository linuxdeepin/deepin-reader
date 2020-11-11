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
#include "ut_utils.h"
#include "Utils.h"
#include "Global.h"

#include <gtest/gtest.h>
#include <QFontMetrics>

Ut_Utils::Ut_Utils()
{
}

void Ut_Utils::SetUp()
{
}

void Ut_Utils::TearDown()
{
}

#ifdef UT_UTILS_TEST
TEST_F(Ut_Utils, UtilsTest)
{
    QKeyEvent keyevent(QEvent::KeyPress, Qt::Key_1, Qt::ControlModifier);
    EXPECT_STREQ(Utils::getKeyshortcut(&keyevent).toStdString().c_str(), Dr::key_ctrl_1.toStdString().c_str());

    EXPECT_EQ(Utils::fileExists("/usr/bin/deepin-reader"), true);
    EXPECT_EQ(Utils::fileExists(""), false);
    EXPECT_EQ(Utils::fileExists("/usr/bin"), false);

    EXPECT_EQ(Utils::dropShadow(QPixmap(), 0, Qt::red), QImage());

    EXPECT_STREQ(Utils::getInputDataSize(0).toStdString().c_str(), "0 B");
    EXPECT_STREQ(Utils::getInputDataSize(1024).toStdString().c_str(), "1.0 KB");
    EXPECT_STREQ(Utils::getInputDataSize(1024 * 1024).toStdString().c_str(), "1.0 MB");
    EXPECT_STREQ(Utils::getInputDataSize(1024 * 1024 * 1024).toStdString().c_str(), "1.0 GB");

    QPixmap pixmap;
    EXPECT_EQ(Utils::roundQPixmap(pixmap, 0).isNull(), true);

    EXPECT_EQ(Utils::getElidedText(QFontMetrics(QFont("Times", 10, QFont::Bold)), QSize(0, 0), "123", Qt::AlignCenter), "...");

    EXPECT_EQ(Utils::copyFile("/home/leiyu/1", ""), false);

    QImage image;
    EXPECT_EQ(Utils::copyImage(image, 0, 0, 0, 0).isNull(), true);

    EXPECT_EQ(Utils::renderSVG("/1/2/3", QSize(12, 12)).isNull(), true);

    EXPECT_EQ(Utils::getHiglightColorList().size(), 8);
}
#endif
