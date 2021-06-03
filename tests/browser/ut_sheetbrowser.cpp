/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chendu <chendu@uniontech.com>
*
* Maintainer: chendu <chendu@uniontech.com>
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

#include "SheetBrowser.h"
#include "BrowserPage.h"
#include "BrowserWord.h"
#include "BrowserMenu.h"
#include "PDFModel.h"
#include "dpdfannot.h"

#include "stub.h"
#include <gtest/gtest.h>
#include <QTest>

///*******************************函数打桩************************************/
QString selectedWordsText_stub()
{
    return "test";
}

QString selectedWordsText_stub_empty()
{
    return "";
}

void initActions_stub(DocSheet *sheet, int index, SheetMenuType_e type, const QString &copytext)
{
    Q_UNUSED(sheet)
    Q_UNUSED(index)
    Q_UNUSED(type)
    Q_UNUSED(copytext)
}

QAction *exec_stub(const QPoint &pos, QAction *at = nullptr)
{
    Q_UNUSED(pos)
    Q_UNUSED(at)
    return nullptr;
}

void clearSelectIconAnnotAfterMenu_stub()
{

}

QPoint mapToGlobal_stub(const QPoint &)
{
    return QPoint();
}

QPointF getAnnotPosInPage_stub(const QPointF &pos, BrowserPage *page)
{
    return QPointF(-1, -1);
}

static QList<BrowserPage *> g_pBrowserPage;
QGraphicsItem *parentItem_stub()
{
    DocSheet sheet(Dr::FileType::PDF, "1.pdf", nullptr);
    g_pBrowserPage.push_back(new BrowserPage(nullptr, 0, &sheet));
    return g_pBrowserPage.last();
}

Annotation *addHighlightAnnotation_stub(QString text, QColor color)
{
    return nullptr;
}

bool calcIconAnnotRect_stub_true(BrowserPage *page, const QPointF &point, QRectF &iconRect)
{
    return true;
}

bool calcIconAnnotRect_stub_false(BrowserPage *page, const QPointF &point, QRectF &iconRect)
{
    return false;
}

int  searchHighlightRectSize_stub_0()
{
    return 0;
}

int  searchHighlightRectSize_stub_1()
{
    return 1;
}

void clearSelectSearchHighlightRects_stub()
{

}

QRectF findSearchforIndex_stub(int index)
{
    Q_UNUSED(index)
    return QRectF(-1, -1, -1, -1);
}

QRectF translateRect_stub(const QRectF &rect)
{
    Q_UNUSED(rect)
    return QRectF(-1, -1, -1, -1);
}

void render_stub(const double &scaleFactor, const Dr::Rotation &rotation, const bool &renderLater, const bool &force)
{
    Q_UNUSED(scaleFactor)
    Q_UNUSED(rotation)
    Q_UNUSED(renderLater)
    Q_UNUSED(force)
}

void beginViewportChange_stub()
{

}

QList<QRectF> boundary_stub()
{
    return QList<QRectF>();
}

QList<QRectF> boundary_stub2()
{
    QList<QRectF> rectFList;
    QRectF rectf(-1, -1, -1, -1);
    rectFList.append(rectf);
    return rectFList;
}
SheetOperation operation_stub()
{
    SheetOperation operation;
    operation.rotation = Dr::RotateBy90;
    return operation;
}

SheetOperation operation_stub2()
{
    SheetOperation operation;
    operation.rotation = Dr::RotateBy180;
    return operation;
}

SheetOperation operation_stub3()
{
    SheetOperation operation;
    operation.rotation = Dr::RotateBy270;
    return operation;
}

bool setDocTapGestrue_stub(QPoint mousePos)
{
    Q_UNUSED(mousePos)
    return true;
}

Qt::MouseEventSource source_stub()
{
    return Qt::MouseEventSynthesizedByQt;
}

static BrowserPage *g_pBrowserPage2 = nullptr;
BrowserPage *getBrowserPageForPoint_stub(QPointF &viewPoint)
{
    DocSheet sheet(Dr::FileType::PDF, "1.pdf", nullptr);
    g_pBrowserPage2 = new BrowserPage(nullptr, 0, &sheet);
    return g_pBrowserPage2;
}

static PDFAnnotation *g_pPDFAnnotation = nullptr;
static DPdfTextAnnot *g_pDPdfAnnot = nullptr;
Annotation *getClickAnnot_stub(BrowserPage *page, const QPointF clickPoint, bool drawRect)
{
    Q_UNUSED(page)
    Q_UNUSED(clickPoint)
    Q_UNUSED(drawRect)
    g_pDPdfAnnot = new DPdfTextAnnot();
    g_pPDFAnnotation = new PDFAnnotation(g_pDPdfAnnot);
    g_pPDFAnnotation->m_dannotation->m_type = DPdfAnnot::AText;
    return g_pPDFAnnotation;
}

QList<QGraphicsItem *> g_QGraphicsItemList;
QList<QGraphicsItem *> items_stub(const QPointF &pos, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape, Qt::SortOrder order = Qt::DescendingOrder, const QTransform &deviceTransform = QTransform())
{
    g_QGraphicsItemList.push_back((new QGraphicsLineItem(nullptr)));
    return g_QGraphicsItemList;
}
///*******************************函数打桩************************************/


// 测试CompressPage
class TestSheetBrowser : public ::testing::Test
{
public:
    TestSheetBrowser(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new SheetBrowser;
        m_tester->disconnect();
        DocSheet sheet(Dr::FileType::PDF, "1.pdf", nullptr);
        BrowserPage *p1 = new BrowserPage(nullptr, 0, &sheet);
        BrowserPage *p2 = new BrowserPage(nullptr, 1, &sheet);
        m_tester->m_items.push_back(p1);
        m_tester->m_items.push_back(p2);
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    SheetBrowser *m_tester;
};

TEST_F(TestSheetBrowser, initTest)
{

}

TEST_F(TestSheetBrowser, testmouseMoveEvent)
{
    QTest::mouseMove(m_tester, QPoint(0, 0));
}

TEST_F(TestSheetBrowser, testshowMenu001)
{
    Stub stub;
    stub.set(ADDR(SheetBrowser, selectedWordsText), selectedWordsText_stub);
    stub.set(ADDR(BrowserMenu, initActions), initActions_stub);
    stub.set((QAction * (QMenu::*)(const QPoint &, QAction *))ADDR(QMenu, exec), exec_stub);
    stub.set(ADDR(SheetBrowser, clearSelectIconAnnotAfterMenu), clearSelectIconAnnotAfterMenu_stub);

    BrowserWord *p = new BrowserWord(nullptr, Word());
    m_tester->m_selectEndWord = p;
    DocSheet *p2 = new DocSheet(Dr::FileType::PDF, "1.pdf", nullptr);
    m_tester->m_sheet = p2;
    m_tester->showMenu();

    delete p;
    delete p2;
}

TEST_F(TestSheetBrowser, testshowMenu002)
{
    Stub stub;
    stub.set(ADDR(SheetBrowser, selectedWordsText), selectedWordsText_stub_empty);
    stub.set(ADDR(BrowserMenu, initActions), initActions_stub);
    stub.set((QAction * (QMenu::*)(const QPoint &, QAction *))ADDR(QMenu, exec), exec_stub);
    stub.set(ADDR(SheetBrowser, clearSelectIconAnnotAfterMenu), clearSelectIconAnnotAfterMenu_stub);

    BrowserWord *p = new BrowserWord(nullptr, Word());
    m_tester->m_selectEndWord = p;
    PDFAnnotation *p2 = new PDFAnnotation(nullptr);
    m_tester->m_iconAnnot = p2;
    DocSheet *p3 = new DocSheet(Dr::FileType::PDF, "1.pdf", nullptr);
    m_tester->m_sheet = p3;
    m_tester->showMenu();

    delete p;
    delete p2;
    delete p3;
}

TEST_F(TestSheetBrowser, testshowMenu003)
{
    Stub stub;
    stub.set(ADDR(SheetBrowser, selectedWordsText), selectedWordsText_stub_empty);
    stub.set(ADDR(BrowserMenu, initActions), initActions_stub);
    stub.set((QAction * (QMenu::*)(const QPoint &, QAction *))ADDR(QMenu, exec), exec_stub);
    stub.set(ADDR(SheetBrowser, clearSelectIconAnnotAfterMenu), clearSelectIconAnnotAfterMenu_stub);
    stub.set(ADDR(QWidget, mapToGlobal), mapToGlobal_stub);

    BrowserWord *p = new BrowserWord(nullptr, Word());
    m_tester->m_selectEndWord = p;
    DocSheet *p2 = new DocSheet(Dr::FileType::PDF, "1.pdf", nullptr);
    m_tester->m_sheet = p2;
    m_tester->showMenu();

    delete p;
    delete p2;
}

TEST_F(TestSheetBrowser, testcalcIconAnnotRect001)
{
    QPointF point;
    QRectF iconRect;
    m_tester->calcIconAnnotRect(nullptr, point, iconRect);
}

TEST_F(TestSheetBrowser, testcalcIconAnnotRect002)
{
    QPointF point;
    QRectF iconRect;
    DocSheet sheet(Dr::FileType::PDF, "1.pdf", nullptr);
    BrowserPage page(nullptr, 0, &sheet);
    m_tester->m_sheet = &sheet;
    m_tester->calcIconAnnotRect(&page, point, iconRect);
}

TEST_F(TestSheetBrowser, testcalcIconAnnotRect003)
{
    QPointF point;
    QRectF iconRect;
    DocSheet sheet(Dr::FileType::PDF, "1.pdf", nullptr);
    BrowserPage page(nullptr, 0, &sheet);
    m_tester->m_sheet = &sheet;
    Stub stub;
    stub.set(ADDR(SheetBrowser, getAnnotPosInPage), getAnnotPosInPage_stub);
    m_tester->calcIconAnnotRect(&page, point, iconRect);
}

TEST_F(TestSheetBrowser, testaddHighLightAnnotation001)
{
    QString contains;
    QPoint showPoint;
    m_tester->addHighLightAnnotation(contains, QColor(), showPoint);

    BrowserWord *pStart = new BrowserWord(nullptr, Word());
    BrowserWord *pEnd = new BrowserWord(nullptr, Word());
    m_tester->m_selectStartWord = pStart;
    m_tester->m_selectEndWord = pEnd;
    m_tester->addHighLightAnnotation(contains, QColor(), showPoint);

    m_tester->m_selectStartWord = pStart;
    m_tester->m_selectEndWord = pEnd;
    m_tester->addHighLightAnnotation(contains, QColor(), showPoint);

    delete pStart;
    delete pEnd;
}

TEST_F(TestSheetBrowser, testaddHighLightAnnotation002)
{
    QString contains;
    QPoint showPoint;
    BrowserWord *pStart = new BrowserWord(nullptr, Word());
    BrowserWord *pEnd = new BrowserWord(nullptr, Word());
    m_tester->m_selectStartWord = pStart;
    m_tester->m_selectEndWord = pEnd;
    m_tester->addHighLightAnnotation(contains, QColor(), showPoint);

    delete pStart;
    delete pEnd;
}

TEST_F(TestSheetBrowser, testaddHighLightAnnotation003)
{
    QString contains;
    QPoint showPoint;
    BrowserWord *pStart = new BrowserWord(nullptr, Word());
    BrowserWord *pEnd = new BrowserWord(nullptr, Word());
    m_tester->m_selectStartWord = pStart;
    m_tester->m_selectEndWord = pEnd;
    Stub stub;
    stub.set(ADDR(QGraphicsItem, parentItem), parentItem_stub);
    stub.set(ADDR(BrowserPage, addHighlightAnnotation), addHighlightAnnotation_stub);
    m_tester->addHighLightAnnotation(contains, QColor(), showPoint);

    delete pStart;
    delete pEnd;
    qDeleteAll(g_pBrowserPage);
}



TEST_F(TestSheetBrowser, testmoveIconAnnot001)
{
    QPointF point;
    m_tester->moveIconAnnot(nullptr, point);
}

TEST_F(TestSheetBrowser, testmoveIconAnnot002)
{
    QPointF point;
    DocSheet sheet(Dr::FileType::PDF, "1.pdf", nullptr);
    BrowserPage page(nullptr, 0, &sheet);
    Stub stub;
    stub.set(ADDR(SheetBrowser, calcIconAnnotRect), calcIconAnnotRect_stub_false);
    m_tester->moveIconAnnot(&page, point);
}

TEST_F(TestSheetBrowser, testmoveIconAnnot003)
{
    QPointF point;
    DocSheet sheet(Dr::FileType::PDF, "1.pdf", nullptr);
    BrowserPage page(nullptr, 0, &sheet);
    Stub stub;
    stub.set(ADDR(SheetBrowser, calcIconAnnotRect), calcIconAnnotRect_stub_true);
    m_tester->moveIconAnnot(&page, point);
}

TEST_F(TestSheetBrowser, testjumpToPrevSearchResult001)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, searchHighlightRectSize), searchHighlightRectSize_stub_0);

    m_tester->jumpToPrevSearchResult();
}

TEST_F(TestSheetBrowser, testjumpToPrevSearchResult002)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, searchHighlightRectSize), searchHighlightRectSize_stub_1);
    stub.set(ADDR(BrowserPage, clearSelectSearchHighlightRects), clearSelectSearchHighlightRects_stub);
    stub.set(ADDR(BrowserPage, findSearchforIndex), findSearchforIndex_stub);
    stub.set(ADDR(BrowserPage, translateRect), translateRect_stub);

    m_tester->jumpToPrevSearchResult();
}

TEST_F(TestSheetBrowser, testjumpToNextSearchResult001)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, searchHighlightRectSize), searchHighlightRectSize_stub_0);

    m_tester->jumpToNextSearchResult();
}

TEST_F(TestSheetBrowser, testjumpToNextSearchResult002)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, searchHighlightRectSize), searchHighlightRectSize_stub_1);
    stub.set(ADDR(BrowserPage, clearSelectSearchHighlightRects), clearSelectSearchHighlightRects_stub);
    stub.set(ADDR(BrowserPage, findSearchforIndex), findSearchforIndex_stub);
    stub.set(ADDR(BrowserPage, translateRect), translateRect_stub);

    m_tester->jumpToNextSearchResult();
}

TEST_F(TestSheetBrowser, testdeform001)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, render), render_stub);
    stub.set(ADDR(SheetBrowser, beginViewportChange), beginViewportChange_stub);

    SheetOperation operation;
    operation.scaleMode = Dr::FitToPageWidthMode;
    operation.layoutMode = Dr::SinglePageMode;

    m_tester->deform(operation);
}

TEST_F(TestSheetBrowser, testdeform002)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, render), render_stub);
    stub.set(ADDR(SheetBrowser, beginViewportChange), beginViewportChange_stub);

    SheetOperation operation;
    operation.scaleMode = Dr::FitToPageWidthMode;
    operation.layoutMode = Dr::TwoPagesMode;

    m_tester->deform(operation);
}

TEST_F(TestSheetBrowser, testdeform003)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, render), render_stub);
    stub.set(ADDR(SheetBrowser, beginViewportChange), beginViewportChange_stub);

    SheetOperation operation;
    operation.scaleMode = Dr::FitToPageHeightMode;
    operation.layoutMode = Dr::SinglePageMode;

    m_tester->deform(operation);
}

TEST_F(TestSheetBrowser, testdeform004)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, render), render_stub);
    stub.set(ADDR(SheetBrowser, beginViewportChange), beginViewportChange_stub);

    SheetOperation operation;
    operation.scaleMode = Dr::FitToPageHeightMode;
    operation.layoutMode = Dr::SinglePageMode;
    operation.rotation = Dr::RotateBy90;

    m_tester->deform(operation);
}

TEST_F(TestSheetBrowser, testdeform005)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, render), render_stub);
    stub.set(ADDR(SheetBrowser, beginViewportChange), beginViewportChange_stub);

    SheetOperation operation;
    operation.scaleMode = Dr::FitToPageDefaultMode;
    operation.layoutMode = Dr::SinglePageMode;

    m_tester->deform(operation);
}

TEST_F(TestSheetBrowser, testdeform006)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, render), render_stub);
    stub.set(ADDR(SheetBrowser, beginViewportChange), beginViewportChange_stub);

    SheetOperation operation;
    operation.scaleMode = Dr::FitToPageWorHMode;
    operation.layoutMode = Dr::SinglePageMode;

    m_tester->deform(operation);
}

TEST_F(TestSheetBrowser, testdeform007)
{
    Stub stub;
    stub.set(ADDR(BrowserPage, render), render_stub);
    stub.set(ADDR(SheetBrowser, beginViewportChange), beginViewportChange_stub);

    SheetOperation operation;
    operation.scaleMode = Dr::FitToPageWorHMode;
    operation.layoutMode = Dr::TwoPagesMode;

    m_tester->deform(operation);
}

TEST_F(TestSheetBrowser, testjumpToHighLight001)
{
    m_tester->jumpToHighLight(nullptr, 0);
}

TEST_F(TestSheetBrowser, testjumpToHighLight002)
{
    typedef QList<QRectF> (*fptr)(PDFAnnotation *);
    fptr A_foo = (fptr)(&PDFAnnotation::boundary);   //获取虚函数地址
    Stub stub;
    stub.set(A_foo, boundary_stub);

    DocSheet p1(Dr::FileType::PDF, "1.pdf", nullptr);
    m_tester->m_sheet = &p1;
    PDFAnnotation p2(nullptr);
    m_tester->jumpToHighLight(&p2, 0);
}

TEST_F(TestSheetBrowser, testjumpToHighLight003)
{
    typedef QList<QRectF> (*fptr)(PDFAnnotation *);
    fptr A_foo = (fptr)(&PDFAnnotation::boundary);   //获取虚函数地址
    Stub stub;
    stub.set(A_foo, boundary_stub2);
    stub.set(ADDR(DocSheet, operation), operation_stub);

    DocSheet p1(Dr::FileType::PDF, "1.pdf", nullptr);
    m_tester->m_sheet = &p1;
    PDFAnnotation p2(nullptr);
    m_tester->jumpToHighLight(&p2, 0);
}

TEST_F(TestSheetBrowser, testjumpToHighLight004)
{
    typedef QList<QRectF> (*fptr)(PDFAnnotation *);
    fptr A_foo = (fptr)(&PDFAnnotation::boundary);   //获取虚函数地址
    Stub stub;
    stub.set(A_foo, boundary_stub2);
    stub.set(ADDR(DocSheet, operation), operation_stub2);

    DocSheet p1(Dr::FileType::PDF, "1.pdf", nullptr);
    m_tester->m_sheet = &p1;
    PDFAnnotation p2(nullptr);
    m_tester->jumpToHighLight(&p2, 0);
}

TEST_F(TestSheetBrowser, testjumpToHighLight005)
{
    typedef QList<QRectF> (*fptr)(PDFAnnotation *);
    fptr A_foo = (fptr)(&PDFAnnotation::boundary);   //获取虚函数地址
    Stub stub;
    stub.set(A_foo, boundary_stub2);
    stub.set(ADDR(DocSheet, operation), operation_stub3);

    DocSheet p1(Dr::FileType::PDF, "1.pdf", nullptr);
    m_tester->m_sheet = &p1;
    PDFAnnotation p2(nullptr);
    m_tester->jumpToHighLight(&p2, 0);
}

TEST_F(TestSheetBrowser, testmousePressEvent001)
{
    Stub stub;
    stub.set(ADDR(SheetBrowser, setDocTapGestrue), setDocTapGestrue_stub);
    stub.set(ADDR(SheetBrowser, getBrowserPageForPoint), getBrowserPageForPoint_stub);
    stub.set(ADDR(QMouseEvent, source), source_stub);
    stub.set(ADDR(SheetBrowser, getClickAnnot), getClickAnnot_stub);

    DocSheet sheet(Dr::FileType::PDF, "1.pdf", nullptr);
    BrowserPage p1(nullptr, 0, &sheet);
    m_tester->m_lastSelectIconAnnotPage = &p1;
    QPointF localPos;
    QMouseEvent event(QEvent::MouseButtonPress, localPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_tester->mousePressEvent(&event);

    delete g_pBrowserPage2;
    delete g_pPDFAnnotation;
    delete g_pDPdfAnnot;
}


TEST_F(TestSheetBrowser, testmousePressEvent002)
{
    QPointF localPos;
    QMouseEvent event(QEvent::MouseButtonPress, localPos, Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    m_tester->mousePressEvent(&event);
}


