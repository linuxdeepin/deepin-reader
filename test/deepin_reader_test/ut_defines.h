
#ifndef UT_DEFINES_TEST_H
#define UT_DEFINES_TEST_H

#include <QStandardPaths>
#include <QCoreApplication>

#define UT_FILE_TEST_FILE   QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/1.pdf")

#define UT_FILE_TEST_FILE_2 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/2.pdf")

#define UT_FILE_TEST_FILE_3 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/3.pdf")

#define UT_FILE_TEST_FILE_4 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/4.pdf")

#define UT_FILE_TEST_FILE_5 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/5_broken.pdf")

#define UT_FILE_TEST_FILE_6 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/6_djvu.pdf")

#define UT_FILE_TXT QCoreApplication::applicationDirPath() + "/files/1.txt"

#define UT_FILE_COPYTXT QCoreApplication::applicationDirPath() + "/files/copy.txt"

#define UT_FILE_PNG QCoreApplication::applicationDirPath() + "/files/1.png"

#define UT_FILE_EMPTYPNG QCoreApplication::applicationDirPath() + "/files/empty.png"

#define UT_FILE_PDF QCoreApplication::applicationDirPath() + "/files/1.pdf"

#define UT_FILE_DJVU QCoreApplication::applicationDirPath() + "/files/1.djvu"

#define UT_FILE_DOC QCoreApplication::applicationDirPath() + "/files/1.doc"

#define UT_FILE_DOCX QCoreApplication::applicationDirPath() + "/files/1.docx"

#define UT_FILE_PPTX QCoreApplication::applicationDirPath() + "/files/1.pptx"

#define UT_FILE_NONE QCoreApplication::applicationDirPath() + "/files/none"

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
