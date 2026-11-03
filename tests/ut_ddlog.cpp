// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ddlog.h"

#include <gtest/gtest.h>
#include <QLoggingCategory>
#include <QDebug>

// dpfLog is defined in the deepin-pdfium-reader library (dpdfglobal.cpp).
// Its private header dpdflog.h lives under 3rdparty/deepin-pdfium/src/ which
// is not on the test include path, so we declare the category here to verify
// that the library exports the symbol correctly.
Q_DECLARE_LOGGING_CATEGORY(dpfLog)

class UT_DdLog : public ::testing::Test
{
protected:
    void TearDown() override
    {
        QLoggingCategory::setFilterRules(QString());
    }
};

// --- ddlog.h is a pure Qt header ---
// The fact that this test file compiles and links without any DTK include
// or dependency verifies that ddlog.h no longer pulls in DLog/DConfig.

TEST_F(UT_DdLog, appLogCategoryName)
{
    EXPECT_STREQ(appLog().categoryName(), "org.deepin.reader");
}

TEST_F(UT_DdLog, batchprintLogCategoryName)
{
    EXPECT_STREQ(batchprintLog().categoryName(), "org.deepin.reader.batchprint");
}

TEST_F(UT_DdLog, htmltopdfLogCategoryName)
{
    EXPECT_STREQ(htmltopdfLog().categoryName(), "org.deepin.reader.htmltopdf");
}

TEST_F(UT_DdLog, dpfLogCategoryName)
{
    EXPECT_STREQ(dpfLog().categoryName(), "org.deepin.reader.pdfium");
}

TEST_F(UT_DdLog, appLogDebugEnabledByFilterRules)
{
    QLoggingCategory::setFilterRules("org.deepin.reader.debug=true");
    EXPECT_TRUE(appLog().isDebugEnabled());
}

TEST_F(UT_DdLog, appLogDebugDisabledByFilterRules)
{
    QLoggingCategory::setFilterRules("org.deepin.reader.debug=false");
    EXPECT_FALSE(appLog().isDebugEnabled());
}

TEST_F(UT_DdLog, batchprintLogWarningEnabledByFilterRules)
{
    QLoggingCategory::setFilterRules("org.deepin.reader.batchprint.warning=true");
    EXPECT_TRUE(batchprintLog().isWarningEnabled());
}

TEST_F(UT_DdLog, htmltopdfLogInfoEnabledByFilterRules)
{
    QLoggingCategory::setFilterRules("org.deepin.reader.htmltopdf.info=true");
    EXPECT_TRUE(htmltopdfLog().isInfoEnabled());
}

TEST_F(UT_DdLog, dpfLogInfoEnabledByFilterRules)
{
    QLoggingCategory::setFilterRules("org.deepin.reader.pdfium.info=true");
    EXPECT_TRUE(dpfLog().isInfoEnabled());
}

// The core feature: a single wildcard rule "org.deepin.reader*.debug=true"
// can enable debug for all module categories simultaneously, allowing
// DConfig (deepin-log-viewer) to filter every process's logs uniformly.
// The pattern "org.deepin.reader*" matches "org.deepin.reader" as well as
// "org.deepin.reader.batchprint", "org.deepin.reader.htmltopdf", etc.
TEST_F(UT_DdLog, wildcardFilterRuleEnablesAllModuleCategories)
{
    QLoggingCategory::setFilterRules("org.deepin.reader*.debug=true");
    EXPECT_TRUE(appLog().isDebugEnabled());
    EXPECT_TRUE(batchprintLog().isDebugEnabled());
    EXPECT_TRUE(htmltopdfLog().isDebugEnabled());
    EXPECT_TRUE(dpfLog().isDebugEnabled());
}

TEST_F(UT_DdLog, wildcardFilterRuleDisablesAllModuleCategories)
{
    QLoggingCategory::setFilterRules("org.deepin.reader*=false");
    EXPECT_FALSE(appLog().isDebugEnabled());
    EXPECT_FALSE(batchprintLog().isDebugEnabled());
    EXPECT_FALSE(htmltopdfLog().isDebugEnabled());
    EXPECT_FALSE(dpfLog().isDebugEnabled());
}

// Each category can be individually controlled without affecting siblings.
// Qt processes filter rules so that the last matching rule takes precedence.
TEST_F(UT_DdLog, individualCategoryControlDoesNotAffectOthers)
{
    QLoggingCategory::setFilterRules("org.deepin.reader*=false\n"
                                    "org.deepin.reader.batchprint.debug=true");
    EXPECT_FALSE(appLog().isDebugEnabled());
    EXPECT_TRUE(batchprintLog().isDebugEnabled());
    EXPECT_FALSE(htmltopdfLog().isDebugEnabled());
}
