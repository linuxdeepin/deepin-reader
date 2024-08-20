// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CatalogWidget.h"
#include "DocSheet.h"
#include "CatalogTreeView.h"

#include "stub.h"
#include "addr_pri.h"
#include <gtest/gtest.h>

class UT_CatalogWidget : public ::testing::Test
{
public:
    UT_CatalogWidget() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new CatalogWidget(m_sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    CatalogWidget *m_tester = nullptr;
};

TEST_F(UT_CatalogWidget, initTest)
{

}

ACCESS_PRIVATE_FIELD(CatalogWidget, QString, m_strTheme);
ACCESS_PRIVATE_FIELD(CatalogWidget, DLabel *, titleLabel);
ACCESS_PRIVATE_FUN(CatalogWidget, void(QResizeEvent *event), resizeEvent);
TEST_F(UT_CatalogWidget, UT_CatalogWidget_resizeEvent)
{
    access_private_field::CatalogWidgetm_strTheme(*m_tester) = "123";
    QResizeEvent *event = new QResizeEvent(QSize(100, 100), QSize(80, 80));
    call_private_fun::CatalogWidgetresizeEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(access_private_field::CatalogWidgettitleLabel(*m_tester)->text() == "123");
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_handleOpenSuccess)
{
    access_private_field::CatalogWidgetm_strTheme(*m_tester) = "123";
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(access_private_field::CatalogWidgettitleLabel(*m_tester)->text() == "");
}

ACCESS_PRIVATE_FIELD(CatalogWidget, CatalogTreeView *, m_pTree);
ACCESS_PRIVATE_FIELD(CatalogTreeView, bool, rightnotifypagechanged);
TEST_F(UT_CatalogWidget, UT_CatalogWidget_handlePage)
{
    m_tester->handlePage(1);

    EXPECT_TRUE(access_private_field::CatalogTreeViewrightnotifypagechanged(*access_private_field::CatalogWidgetm_pTree(*m_tester)) == true);
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_nextPage)
{
    m_tester->nextPage();
    EXPECT_TRUE(access_private_field::CatalogWidgetm_pTree(*m_tester) != nullptr);
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_prevPage)
{
    m_tester->prevPage();
    EXPECT_TRUE(access_private_field::CatalogWidgetm_pTree(*m_tester) != nullptr);
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_pageDown)
{
    m_tester->pageDown();
    EXPECT_TRUE(access_private_field::CatalogWidgetm_pTree(*m_tester) != nullptr);
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_pageUp)
{
    m_tester->pageUp();
    EXPECT_TRUE(access_private_field::CatalogWidgetm_pTree(*m_tester) != nullptr);
}
