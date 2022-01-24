/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
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

#include "AttrScrollWidget.h"
#include "DocSheet.h"
#include "Global.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>

class TestAttrScrollWidget : public ::testing::Test
{
public:
    TestAttrScrollWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        sheet = new DocSheet(Dr::FileType::PDF, "1.pdf", m_tester);
        m_tester = new AttrScrollWidget(sheet, m_tester);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete sheet;
        delete m_tester;
    }

protected:
    DocSheet *sheet = nullptr;
    AttrScrollWidget *m_tester = nullptr;
};

TEST_F(TestAttrScrollWidget, initTest)
{

}
