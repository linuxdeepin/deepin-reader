
#ifndef UT_DEFINES_TEST_H
#define UT_DEFINES_TEST_H

#include <QStandardPaths>

#define UT_FILE_TEST_FILE   QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0).append("/Desktop/1.pdf")

#define UT_FILE_TEST_FILE_2 QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0).append("/Desktop/2.pdf")

#define UT_FILE_TEST_FILE_3 QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0).append("/Desktop/3.pdf")

#define UT_FILE_TEST_FILE_4 QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0).append("/Desktop/4.pdf")

#define UT_FILE_TEST_FILE_5 QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0).append("/Desktop/5_broken.pdf")

#define UT_FILE_TEST_FILE_6 QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0).append("/Desktop/6_djvu.pdf")

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
