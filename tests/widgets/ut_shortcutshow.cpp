// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ShortCutShow.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"
#include "stub.h"
#include "addr_pri.h"

#include "ut_common.h"

#include <gtest/gtest.h>
#include <QTest>

namespace {
class UT_ShortCutShow : public ::testing::Test
{
public:
    UT_ShortCutShow(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new ShortCutShow();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ShortCutShow *m_tester;
};

static QString g_funcname;
void initPDF_stub()
{
    g_funcname = __FUNCTION__;
}

void initDJVU_stub()
{
    g_funcname = __FUNCTION__;
}
}

TEST_F(UT_ShortCutShow, initTest)
{

}
ACCESS_PRIVATE_FUN(ShortCutShow, void(), initPDF);
ACCESS_PRIVATE_FUN(ShortCutShow, void(), initDJVU);

TEST_F(UT_ShortCutShow, UT_ShortCutShow_setSheet_001)
{
    Stub s;
    auto initPDF_temp = get_private_fun::ShortCutShowinitPDF();
    auto initDJVUtemp = get_private_fun::ShortCutShowinitDJVU();
    s.set(initPDF_temp, initPDF_stub);
    s.set(initDJVUtemp, initDJVU_stub);

    m_tester->setSheet(nullptr);
    EXPECT_TRUE(g_funcname == "initPDF_stub");

    s.reset(initPDF_temp);
    s.reset(initDJVUtemp);


}

TEST_F(UT_ShortCutShow, UT_ShortCutShow_setSheet_002)
{
    Stub s;
    auto initPDF_temp = get_private_fun::ShortCutShowinitPDF();
    auto initDJVUtemp = get_private_fun::ShortCutShowinitDJVU();
    s.set(initPDF_temp, initPDF_stub);
    s.set(initDJVUtemp, initDJVU_stub);

    DocSheet *sheet = new DocSheet(Dr::FileType::DJVU, "1.pdf");
    m_tester->setSheet(sheet);
    delete sheet;
    EXPECT_TRUE(g_funcname == "initDJVU_stub");

    s.reset(initPDF_temp);
    s.reset(initDJVUtemp);
}

TEST_F(UT_ShortCutShow, UT_ShortCutShow_setSheet_003)
{
    Stub s;
    auto initPDF_temp = get_private_fun::ShortCutShowinitPDF();
    auto initDJVUtemp = get_private_fun::ShortCutShowinitDJVU();
    s.set(initPDF_temp, initPDF_stub);
    s.set(initDJVUtemp, initDJVU_stub);

    DocSheet *sheet = new DocSheet(Dr::FileType::PDF, "1.pdf");
    m_tester->setSheet(sheet);
    delete sheet;
    EXPECT_TRUE(g_funcname == "initPDF_stub");

    s.reset(initPDF_temp);
    s.reset(initDJVUtemp);
}

ACCESS_PRIVATE_FIELD(ShortCutShow, QStringList, Editnames);
TEST_F(UT_ShortCutShow, UT_ShortCutShow_show)
{
    Stub stub;
    UTCommon::stub_QProcess_startDetached(stub);

    call_private_fun::ShortCutShowinitDJVU(*m_tester);
    m_tester->show();
    EXPECT_TRUE(m_tester->m_shortcutMap.count() > 0);
}

ACCESS_PRIVATE_FIELD(ShortCutShow, QStringList, windowKeymaps);
ACCESS_PRIVATE_FIELD(ShortCutShow, QStringList, shortcutnames);
ACCESS_PRIVATE_FIELD(ShortCutShow, QStringList, Filesnames);
ACCESS_PRIVATE_FIELD(ShortCutShow, QStringList, Displaynames);
ACCESS_PRIVATE_FIELD(ShortCutShow, QStringList, Toolsnames);
TEST_F(UT_ShortCutShow, UT_ShortCutShow_initDJVU)
{
    m_tester->initDJVU();
    EXPECT_TRUE(m_tester->m_shortcutMap.count() > 0);
}

TEST_F(UT_ShortCutShow, UT_ShortCutShow_initPDF)
{
    m_tester->initPDF();
    EXPECT_TRUE(m_tester->m_shortcutMap.count() > 0);
}

