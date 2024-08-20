// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TextEditWidget.h"
#include "DocSheet.h"
#include "SheetBrowser.h"
#include "TransparentTextEdit.h"
#include "Model.h"
#include "PDFModel.h"
#include "dpdfannot.h"
#include "ut_common.h"
#include "stub.h"
#include "addr_pri.h"
#include <QSignalSpy>
#include <QDebug>

#include <gtest/gtest.h>

class UT_TextEditShadowWidgetayWidget : public ::testing::Test
{
public:
    UT_TextEditShadowWidgetayWidget() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_pBrowser = new SheetBrowser(m_sheet);
        m_tester = new TextEditShadowWidget(m_pBrowser);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_pBrowser;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    SheetBrowser *m_pBrowser = nullptr;
    TextEditShadowWidget *m_tester = nullptr;
};

TEST_F(UT_TextEditShadowWidgetayWidget, initTest)
{

}

ACCESS_PRIVATE_FIELD(TextEditShadowWidget, TextEditWidget *, m_TextEditWidget);

TEST_F(UT_TextEditShadowWidgetayWidget, UT_TextEditShadowWidgetayWidget_getTextEditWidget)
{
    EXPECT_TRUE(m_tester->getTextEditWidget() == access_private_field::TextEditShadowWidgetm_TextEditWidget(*m_tester));
}

TEST_F(UT_TextEditShadowWidgetayWidget, UT_TextEditShadowWidgetayWidget_showWidget)
{
    m_tester->showWidget(QPoint());
    EXPECT_TRUE(access_private_field::TextEditShadowWidgetm_TextEditWidget(*m_tester) != nullptr);
}

TEST_F(UT_TextEditShadowWidgetayWidget, UT_TextEditShadowWidgetayWidget_slotCloseNoteWidget)
{
    m_tester->showWidget(QPoint());
    EXPECT_TRUE(access_private_field::TextEditShadowWidgetm_TextEditWidget(*m_tester) != nullptr);
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
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_pBrowser = new SheetBrowser(m_sheet);
        m_pShadow = new TextEditShadowWidget(m_pBrowser);
        m_tester = new TextEditWidget(m_pShadow);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_pBrowser;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet;
    SheetBrowser *m_pBrowser;
    TextEditShadowWidget *m_pShadow;
    TextEditWidget *m_tester;
};

TEST_F(TestTextEditWidget, initTest)
{

}

ACCESS_PRIVATE_FUN(TextEditWidget, void(), onShowMenu);
ACCESS_PRIVATE_FIELD(TextEditWidget, TransparentTextEdit *, m_pTextEdit);
TEST_F(TestTextEditWidget, test_TestTextEditWidget_onShowMenu)
{
    Stub stub;
    UTCommon::stub_QWidget_isVisible(stub, true);
    UTCommon::stub_DMenu_exec(stub);
    call_private_fun::TextEditWidgetonShowMenu(*m_tester);
    EXPECT_TRUE(access_private_field::TextEditWidgetm_pTextEdit(*m_tester) != nullptr);
}

ACCESS_PRIVATE_FIELD(TextEditWidget, QString, m_strNote);
TEST_F(TestTextEditWidget, test_TestTextEditWidget_setEditText)
{
    m_tester->setEditText("123");
    EXPECT_TRUE(access_private_field::TextEditWidgetm_pTextEdit(*m_tester)->toPlainText() == "123");
    EXPECT_TRUE(access_private_field::TextEditWidgetm_strNote(*m_tester) == "123");
}

ACCESS_PRIVATE_FIELD(TextEditWidget, deepin_reader::Annotation *, m_annotation);
TEST_F(TestTextEditWidget, test_TestTextEditWidget_setAnnotation)
{
    m_tester->setAnnotation(nullptr);
    EXPECT_TRUE(access_private_field::TextEditWidgetm_annotation(*m_tester) == nullptr);
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_setEditFocus)
{
    m_tester->setEditFocus();
    EXPECT_TRUE(access_private_field::TextEditWidgetm_pTextEdit(*m_tester) != nullptr);
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_getTextEdit)
{
    EXPECT_TRUE(m_tester->getTextEdit() == access_private_field::TextEditWidgetm_pTextEdit(*m_tester));
}

ACCESS_PRIVATE_FUN(TextEditWidget, void(QHideEvent *event), hideEvent);

TEST_F(TestTextEditWidget, test_TestTextEditWidget_hideEvent_001)
{
    DPdfTextAnnot *pDPdfAnnot  = new DPdfTextAnnot();
    deepin_reader::PDFAnnotation *p = new deepin_reader::PDFAnnotation(pDPdfAnnot);

    access_private_field::TextEditWidgetm_annotation(*m_tester) = p;
    QHideEvent *event = new QHideEvent();
    QSignalSpy spy(m_tester, SIGNAL(sigRemoveAnnotation(deepin_reader::Annotation *, bool)));
    call_private_fun::TextEditWidgethideEvent(*m_tester, event);

    EXPECT_TRUE(spy.count() == 1);
    delete p;
    delete pDPdfAnnot;
    delete event;
}

TEST_F(TestTextEditWidget, test_TestTextEditWidget_hideEvent_002)
{
    DPdfTextAnnot *pDPdfAnnot  = new DPdfTextAnnot();
    deepin_reader::PDFAnnotation *p = new deepin_reader::PDFAnnotation(pDPdfAnnot);

    access_private_field::TextEditWidgetm_pTextEdit(*m_tester)->setPlainText("123");
    access_private_field::TextEditWidgetm_annotation(*m_tester) = p;
    QHideEvent *event = new QHideEvent();
    QSignalSpy spy(m_tester, SIGNAL(sigUpdateAnnotation(deepin_reader::Annotation *, const QString &)));
    call_private_fun::TextEditWidgethideEvent(*m_tester, event);
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

ACCESS_PRIVATE_FUN(TextEditWidget, void(QPaintEvent *event), paintEvent);
TEST_F(TestTextEditWidget, test_TestTextEditWidget_paintEvent)
{
    QPaintEvent paint(QRect(m_tester->rect()));
    call_private_fun::TextEditWidgetpaintEvent(*m_tester, &paint);
    EXPECT_FALSE(m_tester->grab().isNull());
}

ACCESS_PRIVATE_FUN(TextEditWidget, void(QFocusEvent *event), focusOutEvent);
TEST_F(TestTextEditWidget, test_TestTextEditWidget_focusOutEvent)
{
    QSignalSpy spy(m_tester, SIGNAL(sigCloseNoteWidget(bool)));
    QFocusEvent *event = new QFocusEvent(QEvent::FocusOut);
    call_private_fun::TextEditWidgetfocusOutEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(spy.count() == 1);
}

