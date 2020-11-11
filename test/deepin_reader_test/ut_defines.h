
#ifndef UT_DEFINES_TEST_H
#define UT_DEFINES_TEST_H

#include <QStandardPaths>

#define UT_FILE_TEST_FILE   QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/1.pdf")

#define UT_FILE_TEST_FILE_2 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/2.pdf")

#define UT_FILE_TEST_FILE_3 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/3.pdf")

#define UT_FILE_TEST_FILE_4 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/4.pdf")

#define UT_FILE_TEST_FILE_5 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/5_broken.pdf")

#define UT_FILE_TEST_FILE_6 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/6_djvu.pdf")

#define UT_JSON_TEST

#define UT_UTILS_TEST

#define UT_WIDGETS_TEST

#define UT_UIFRAME_TEST

#define UT_DOCSHEET_TEST

#define UT_PDFMODEL_TEST

#define UT_DJVUMODEL_TEST

#define UT_MAINWINDOW_TEST

#define UT_SHEETSIDEBAR_TEST

#endif // UT_DEFINES_TEST_H
