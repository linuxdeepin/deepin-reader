// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ScaleMenu.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"
#include "stub.h"
#include "addr_pri.h"
#include <gtest/gtest.h>
#include <QTest>
#include <QList>

class TestScaleMenu : public ::testing::Test
{
public:
    TestScaleMenu(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }

        m_tester = new ScaleMenu();
        m_tester->readCurDocParam(new DocSheet(Dr::FileType::PDF, filePath, m_tester));
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ScaleMenu *m_tester;
};

int indexOf_stub(void *, const QAction &, int)
{
    return 1;
}
QList<qreal> scaleFactorList_stub()
{
    QList<qreal> factorList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};
    return  factorList;
}

TEST_F(TestScaleMenu, initTest)
{

}

ACCESS_PRIVATE_FIELD(ScaleMenu, DocSheet *, m_sheet);
ACCESS_PRIVATE_FIELD(DocSheet, SheetOperation, m_operation);
ACCESS_PRIVATE_FUN(ScaleMenu, void(), onTwoPage);

TEST_F(TestScaleMenu, testonTwoPage)
{
    access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).layoutMode = Dr::TwoPagesMode;
    access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode = Dr::FitToPageDefaultMode;
    call_private_fun::ScaleMenuonTwoPage(*m_tester);
    EXPECT_TRUE(access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).layoutMode == Dr::SinglePageMode);
    EXPECT_TRUE(access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode == Dr::FitToPageWidthMode);
}

ACCESS_PRIVATE_FUN(ScaleMenu, void(), onFiteH);
TEST_F(TestScaleMenu, testonFiteH)
{
    access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode = Dr::FitToPageDefaultMode;
    call_private_fun::ScaleMenuonFiteH(*m_tester);
    EXPECT_TRUE(access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode == Dr::FitToPageHeightMode);
}

ACCESS_PRIVATE_FUN(ScaleMenu, void(), onFiteW);
TEST_F(TestScaleMenu, testonFiteW)
{
    access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode = Dr::FitToPageDefaultMode;
    call_private_fun::ScaleMenuonFiteW(*m_tester);
    EXPECT_TRUE(access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode == Dr::FitToPageWidthMode);
}

ACCESS_PRIVATE_FUN(ScaleMenu, void(), onDefaultPage);
TEST_F(TestScaleMenu, testonDefaultPage)
{
    access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode = Dr::FitToPageWidthMode;
    call_private_fun::ScaleMenuonDefaultPage(*m_tester);
    EXPECT_TRUE(access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode == Dr::FitToPageDefaultMode);
}

ACCESS_PRIVATE_FUN(ScaleMenu, void(), onFitPage);
TEST_F(TestScaleMenu, testonFitPage)
{
    access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode = Dr::FitToPageDefaultMode;
    call_private_fun::ScaleMenuonFitPage(*m_tester);
    EXPECT_TRUE(access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode == Dr::FitToPageWorHMode);
}

ACCESS_PRIVATE_FUN(ScaleMenu, void(), onScaleFactor);
TEST_F(TestScaleMenu, testonScaleFactor)
{
    Stub s;
    s.set(ADDR(QList<QAction *>, indexOf), indexOf_stub);
    s.set(ADDR(DocSheet, scaleFactorList), scaleFactorList_stub);
    access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode = Dr::FitToPageWidthMode;
    call_private_fun::ScaleMenuonScaleFactor(*m_tester);
    //EXPECT_TRUE(access_private_field::DocSheetm_operation(*access_private_field::ScaleMenum_sheet(*m_tester)).scaleMode == Dr::ScaleFactorMode);
}
