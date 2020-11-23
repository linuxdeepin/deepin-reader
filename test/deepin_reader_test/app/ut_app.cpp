#include "ut_app.h"
#define private public
#define protected public

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

#undef private
#undef protected

ut_app::ut_app()
{

}

void ut_app::SetUp()
{
    ut_application::SetUp();
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

TEST_F(ut_app, UtilsTest)
{
    QKeyEvent keyevent(QEvent::KeyPress, Qt::Key_1, Qt::ControlModifier);
    EXPECT_STREQ(Utils::getKeyshortcut(&keyevent).toStdString().c_str(), Dr::key_ctrl_1.toStdString().c_str());

    QPixmap pixmap(UT_FILE_PNG);
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

    EXPECT_EQ(Utils::getHiglightColorList().size(), 8);
    EXPECT_EQ(Utils::isWayland(), false);
}
#endif
