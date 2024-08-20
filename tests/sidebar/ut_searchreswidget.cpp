// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SearchResWidget.h"
#include "DocSheet.h"
#include "CatalogTreeView.h"
#include "SideBarImageListview.h"

#include "stub.h"
#include "addr_pri.h"

#include <QStackedLayout>
#include <gtest/gtest.h>

class UT_SearchResWidget : public ::testing::Test
{
public:
    UT_SearchResWidget() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new SearchResWidget(m_sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    SearchResWidget *m_tester = nullptr;
};

TEST_F(UT_SearchResWidget, initTest)
{

}

ACCESS_PRIVATE_FIELD(SearchResWidget, DocSheet *, m_sheet);
TEST_F(UT_SearchResWidget, UT_SearchResWidget_handleSearchResultComming)
{
    deepin_reader::PageLine line;
    line.text = "123";

    deepin_reader::PageSection section;
    section << line;

    deepin_reader::SearchResult search;
    search.sections << section;

    m_tester->handleSearchResultComming(search);
    EXPECT_TRUE(access_private_field::SearchResWidgetm_sheet(*m_tester) != nullptr);
}

ACCESS_PRIVATE_FIELD(SearchResWidget, QStackedLayout *, m_stackLayout);
TEST_F(UT_SearchResWidget, UT_SearchResWidget_handleFindFinished)
{
    m_tester->handleFindFinished();
    EXPECT_TRUE(access_private_field::SearchResWidgetm_stackLayout(*m_tester)->currentIndex() == 1);
}

ACCESS_PRIVATE_FIELD(SearchResWidget, QString, m_searchKey);
TEST_F(UT_SearchResWidget, UT_SearchResWidget_clearFindResult)
{
    m_tester->clearFindResult();
    EXPECT_TRUE(access_private_field::SearchResWidgetm_stackLayout(*m_tester)->currentIndex() == 0);
    EXPECT_TRUE(access_private_field::SearchResWidgetm_searchKey(*m_tester).count() == 0);
}

TEST_F(UT_SearchResWidget, UT_SearchResWidget_searchKey)
{
    m_tester->searchKey("123");
    EXPECT_TRUE(access_private_field::SearchResWidgetm_searchKey(*m_tester) == "123");
}

ACCESS_PRIVATE_FUN(SearchResWidget, void(const int &pageIndex, const QString &text, const int &resultNum), addSearchsItem);
TEST_F(UT_SearchResWidget, UT_SearchResWidget_addSearchsItem)
{
    call_private_fun::SearchResWidgetaddSearchsItem(*m_tester, 0, "123", 1);
    EXPECT_TRUE(access_private_field::SearchResWidgetm_stackLayout(*m_tester)->currentIndex() == 0);
}

ACCESS_PRIVATE_FIELD(SearchResWidget, SideBarImageListView *, m_pImageListView);
TEST_F(UT_SearchResWidget, UT_SearchResWidget_adaptWindowSize)
{
    m_tester->adaptWindowSize(1);
    EXPECT_TRUE(access_private_field::SearchResWidgetm_pImageListView(*m_tester)->property("adaptScale") == 1);
}
