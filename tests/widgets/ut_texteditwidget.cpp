/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "TextEditWidget.h"
#include "DocSheet.h"
#include "SheetBrowser.h"
#include "TransparentTextEdit.h"
#include "Model.h"
#include "PDFModel.h"
#include "dpdfannot.h"
#include "ut_common.h"

#include <QSignalSpy>
#include <QDebug>

#include <gtest/gtest.h>

class UT_TextEditShadowWidgetayWidget : public ::testing::Test
{
public:
    UT_TextEditShadowWidgetayWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_pBrowser = new SheetBrowser(sheet);
        m_tester = new TextEditShadowWidget(m_pBrowser);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_pBrowser;
    }

protected:
    SheetBrowser *m_pBrowser;
    TextEditShadowWidget *m_tester;
};

TEST_F(UT_TextEditShadowWidgetayWidget, initTest)
{

}

TEST_F(UT_TextEditShadowWidgetayWidget, UT_TextEditShadowWidgetayWidget_getTextEditWidget)
{
    EXPECT_TRUE(m_tester->getTextEditWidget() == m_tester->m_TextEditWidget);
}

TEST_F(UT_TextEditShadowWidgetayWidget, UT_TextEditShadowWidgetayWidget_showWidget)
{
    m_tester->showWidget(QPoint());
    EXPECT_TRUE(m_tester->m_TextEditWidget != nullptr);
}

TEST_F(UT_TextEditShadowWidgetayWidget, UT_TextEditShadowWidgetayWidget_slotCloseNoteWidget)
{
    m_tester->showWidget(QPoint());
    EXPECT_TRUE(m_tester->m_TextEditWidget != nullptr);
}


class TestTextEditWidget : public ::testing::Test
{
public:
    TestTextEditWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_pBrowser = new SheetBrowser(sheet);
        m_pShadow = new TextEditShadowWidget(m_pBrowser);
        m_tester = new TextEditWidget(m_pShadow);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_pBrowser;
    }

protected:
    SheetBrowser *m_pBrowser;
    TextEditShadowWidget *m_pShadow;
    TextEditWidget *m_tester;
};

TEST_F(TestTextEditWidget, initTest)
{

}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_onShowMenu)
{
    Stub stub;
    UTCommon::stub_QWidget_isVisible(stub, true);
    UTCommon::stub_DMenu_exec(stub);
    m_tester->onShowMenu();
    EXPECT_TRUE(m_tester->m_pTextEdit != nullptr);
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_setEditText)
{
    m_tester->setEditText("123");
    EXPECT_TRUE(m_tester->m_pTextEdit->toPlainText() == "123");
    EXPECT_TRUE(m_tester->m_strNote == "123");
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_setAnnotation)
{
    m_tester->setAnnotation(nullptr);
    EXPECT_TRUE(m_tester->m_annotation == nullptr);
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_setEditFocus)
{
    m_tester->setEditFocus();
    EXPECT_TRUE(m_tester->m_pTextEdit != nullptr);
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_getTextEdit)
{
    EXPECT_TRUE(m_tester->getTextEdit() == m_tester->m_pTextEdit);
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_hideEvent_001)
{
    DPdfTextAnnot *pDPdfAnnot  = new DPdfTextAnnot();
    deepin_reader::PDFAnnotation *p = new deepin_reader::PDFAnnotation(pDPdfAnnot);

    m_tester->m_annotation = p;
    QHideEvent *event = new QHideEvent();
    QSignalSpy spy(m_tester, SIGNAL(sigRemoveAnnotation(deepin_reader::Annotation *, bool)));
    m_tester->hideEvent(event);
    EXPECT_TRUE(spy.count() == 1);
    delete p;
    delete pDPdfAnnot;
    delete event;
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_hideEvent_002)
{
    DPdfTextAnnot *pDPdfAnnot  = new DPdfTextAnnot();
    deepin_reader::PDFAnnotation *p = new deepin_reader::PDFAnnotation(pDPdfAnnot);

    m_tester->m_pTextEdit->setPlainText("123");
    m_tester->m_annotation = p;
    QHideEvent *event = new QHideEvent();
    QSignalSpy spy(m_tester, SIGNAL(sigUpdateAnnotation(deepin_reader::Annotation *, const QString &)));
    m_tester->hideEvent(event);
    EXPECT_TRUE(spy.count() == 1);
    delete p;
    delete pDPdfAnnot;
    delete event;
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_onTouchPadEvent_001)
{
    m_tester->onTouchPadEvent("", "up", 0);
}

static QString g_funcname;
void qTimer_start_stub()
{
    g_funcname = __FUNCTION__;
    return;
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_onTouchPadEvent_002)
{
    Stub stub;
    stub.set((void(QTimer::*)())ADDR(QTimer, start), qTimer_start_stub);
    m_tester->onTouchPadEvent("", "down", 0);
    EXPECT_TRUE(g_funcname == "qTimer_start_stub");
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_paintEvent)
{
    QPaintEvent paint(QRect(m_tester->rect()));
    m_tester->paintEvent(&paint);
    EXPECT_FALSE(m_tester->grab().isNull());
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_focusOutEvent)
{
    QSignalSpy spy(m_tester, SIGNAL(sigCloseNoteWidget(bool)));
    QFocusEvent *event = new QFocusEvent(QEvent::FocusOut);
    m_tester->focusOutEvent(event);
    delete event;
    EXPECT_TRUE(spy.count() == 1);
}

