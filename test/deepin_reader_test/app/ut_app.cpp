#include "ut_app.h"
#define private public
#define protected public

#include "app/Json.h"
#include <QStringList>
#include <QJsonArray>
#include <QDebug>
#include "Utils.h"
#include "Global.h"
#include <gtest/gtest.h>
#include <QFontMetrics>
#include "app/DebugTimeManager.h"
#include "Database.h"
#include "DocSheet.h"
#include "TimeElapsedUtils.h"

#undef private
#undef protected

ut_app::ut_app()
{

}

void ut_app::SetUp()
{
}

void ut_app::TearDown()
{
}

#ifdef UT_APP_TEST
TEST_F(ut_app, DatabaseTest)
{
    Database::instance()->prepareBookmark();
    Database::instance()->prepareOperation();

    QSet<int> bookmarks;
    Database::instance()->readBookmarks(UT_FILE_PDF, bookmarks);
    bookmarks << 0;
    Database::instance()->saveBookmarks(UT_FILE_PDF, bookmarks);

    DocSheet *sheet = new DocSheet(Dr::PDF, UT_FILE_PDF, nullptr);
    Database::instance()->readOperation(sheet);
    Database::instance()->saveOperation(sheet);
}

TEST_F(ut_app, TimeElapsedUtilsTest)
{
    TimeElapsedUtils te("test te");
}

TEST_F(ut_app, DebugTimeManagerTest)
{
    DebugTimeManager::getInstance()->beginPointLinux("0", "1");
    DebugTimeManager::getInstance()->endPointLinux("0", "1");
    DebugTimeManager::getInstance()->beginPointQt("1", "1");
    DebugTimeManager::getInstance()->endPointQt("1");
    DebugTimeManager::getInstance()->clear();
}

TEST_F(ut_app, GlobalTest)
{
    EXPECT_EQ(Dr::fileType(UT_FILE_PDF), Dr::PDF);
    EXPECT_EQ(Dr::fileType(UT_FILE_PPTX), Dr::PPTX);
    EXPECT_EQ(Dr::fileType(UT_FILE_DJVU), Dr::DJVU);
    EXPECT_EQ(Dr::fileType(UT_FILE_DOC), Dr::DOC);
    EXPECT_EQ(Dr::fileType(UT_FILE_DOCX), Dr::DOCX);
    EXPECT_EQ(Dr::fileType(UT_FILE_NONE), Dr::Unknown);
}

TEST_F(ut_app, JsonTest)
{
    Json json("{\"test0_key\" : \"test0_value\", \"test1_key\" : 1, \"test2_key\" : true, \"test3_key\" : 520.1314, \"test4_key\" : [\"1\", \"2\",\"3\"], \"test5_key\" : {\"child0\" : 1}}");
    EXPECT_STREQ(json.getString("test0_key").toStdString().c_str(), "test0_value");
    EXPECT_EQ(json.getInt("test1_key"), 1);
    EXPECT_EQ(json.getBool("test2_key"), true);
    EXPECT_DOUBLE_EQ(json.getDouble("test3_key"), 520.1314);
    EXPECT_EQ(json.getStringList("test4_key").join(","), "1,2,3");
    EXPECT_EQ(json.getJsonArray("test4_key"), QJsonArray() << "1" << "2" << "3");
    EXPECT_EQ(json.getJsonValue("test1_key").toInt(), 1);
    EXPECT_EQ(json.getJsonObject("test5_key").value("child0"), 1);

    EXPECT_EQ(json.getInt("test5_key.child0"), 1);

    json.set("test0_key", "111");
    EXPECT_STREQ(json.getString("test0_key").toStdString().c_str(), "111");
    json.set("test0_key", QStringList() << "1" << "2" << "3");
    EXPECT_STREQ(json.getStringList("test0_key").join(",").toStdString().c_str(), "1,2,3");
    json.set("test0_key", "test0_value");
    EXPECT_STREQ(json.toString(QJsonDocument::Compact).toStdString().c_str(), "{\"test0_key\":\"test0_value\",\"test1_key\":1,\"test2_key\":true,\"test3_key\":520.1314,\"test4_key\":[\"1\",\"2\",\"3\"],\"test5_key\":{\"child0\":1}}");
}

TEST_F(ut_app, UtilsTest)
{
    QKeyEvent keyevent(QEvent::KeyPress, Qt::Key_1, Qt::ControlModifier);
    EXPECT_STREQ(Utils::getKeyshortcut(&keyevent).toStdString().c_str(), Dr::key_ctrl_1.toStdString().c_str());

    EXPECT_EQ(Utils::fileExists("/usr/bin/deepin-reader"), true);
    EXPECT_EQ(Utils::fileExists(""), false);
    EXPECT_EQ(Utils::fileExists("/usr/bin"), false);

    QPixmap pixmap(UT_FILE_PNG);
    EXPECT_EQ(Utils::dropShadow(QPixmap(), 0, Qt::red), QImage());
    EXPECT_EQ(Utils::roundQPixmap(pixmap, 0).isNull(), false);
    EXPECT_EQ(Utils::roundQPixmap(QPixmap(), 0).isNull(), true);

    EXPECT_STREQ(Utils::getInputDataSize(0).toStdString().c_str(), "0 B");
    EXPECT_STREQ(Utils::getInputDataSize(1024).toStdString().c_str(), "1.0 KB");
    EXPECT_STREQ(Utils::getInputDataSize(1024 * 1024).toStdString().c_str(), "1.0 MB");
    EXPECT_STREQ(Utils::getInputDataSize(1024 * 1024 * 1024).toStdString().c_str(), "1.0 GB");

    EXPECT_EQ(Utils::getElidedText(QFontMetrics(QFont("Times", 10, QFont::Bold)), QSize(0, 0), "123", Qt::AlignCenter), "...");
    EXPECT_EQ(Utils::copyFile(UT_FILE_TXT, UT_FILE_COPYTXT), true);
    Utils::copyText("copyText");

    QImage image;
    EXPECT_EQ(Utils::copyImage(image, 0, 0, 0, 0).isNull(), true);
    image = pixmap.toImage();
    EXPECT_EQ(Utils::copyImage(image, 1, 1, 1, 1).isNull(), false);

    EXPECT_EQ(Utils::renderSVG(UT_FILE_EMPTYPNG, QSize(12, 12)).isNull(), true);
    EXPECT_EQ(Utils::renderSVG(UT_FILE_PNG, QSize(12, 12)).isNull(), false);
    EXPECT_EQ(Utils::getHiglightColorList().size(), 8);
    EXPECT_EQ(Utils::isWayland(), false);
    EXPECT_EQ(Utils::getUuid().isEmpty(), false);
}
#endif
