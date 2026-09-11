// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "Application.h"
#include "BrowserPage.h"
#include "CatalogTreeView.h"
#include "CatalogOutlineModel.h"
#include "DocSheet.h"
#include "Navigation.h"
#include "SecurityDialog.h"
#include "SheetBrowser.h"
#include "SheetRenderer.h"
#include <QDesktopServices>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QKeyEvent>
#include <QProcess>
#include <QScrollBar>
#include <QTemporaryDir>
#include <QTimer>
#include <cmath>

using namespace deepin_reader;
static int checks = 0;
static void verify(bool condition, const char *description)
{
    if (!condition)
        qFatal("FAIL: %s", description);
    ++checks;
}
static void settle()
{
    QEventLoop loop;
    QTimer::singleShot(250, &loop, &QEventLoop::quit);
    loop.exec();
}
class UrlSink : public QObject {
    Q_OBJECT
public:
    int calls = 0;
public slots:
    void capture(const QUrl &) { ++calls; }
};

int main(int argc, char **argv)
{
    Application app(argc, argv);
    QTimer::singleShot(60000, [] { qFatal("Smoke test timed out"); });
    QTemporaryDir fixture;
    verify(fixture.isValid(), "fixture directory");
    QByteArray nested;
    for (int i = 0; i < 8; ++i)
        nested += "<OutlineElem Title=\"Level " + QByteArray::number(i) + "\" Expanded=\"true\">";
    nested += "<OutlineElem Title=\"Destination\"><Actions><Action Event=\"CLICK\">"
              "<Goto><Dest Type=\"XYZ\" PageID=\"42\" Left=\"23\" Top=\"35\" Zoom=\"2\"/>"
              "</Goto></Action></Actions></OutlineElem>";
    for (int i = 0; i < 8; ++i)
        nested += "</OutlineElem>";
    nested += "<OutlineElem Title=\"Website\"><Actions><Action Event=\"CLICK\">"
              "<URI URI=\"https://example.invalid/navigation\"/></Action></Actions></OutlineElem>";
    const QMap<QString, QByteArray> entries{
        {"OFD.xml", "<OFD><DocBody><DocInfo/><DocRoot>Document.xml</DocRoot></DocBody></OFD>"},
        {"Document.xml", "<Document><CommonData><PageArea><PhysicalBox>7 11 210 297</PhysicalBox>"
                         "</PageArea></CommonData><Pages><Page ID=\"1\" BaseLoc=\"Page.xml\"/>"
                         "<Page ID=\"42\" BaseLoc=\"Second.xml\"/></Pages><Outlines>" + nested
                         + "</Outlines></Document>"},
        {"Page.xml", "<Page><Area><PhysicalBox>7 11 210 297</PhysicalBox></Area><Content>"
                     "<Layer ID=\"2\"><PathObject ID=\"3\" Boundary=\"20 30 40 25\" Fill=\"true\">"
                     "<Actions><Action Event=\"CLICK\"><Goto><Dest Type=\"Fit\" PageID=\"42\"/>"
                     "</Goto></Action></Actions><FillColor Value=\"255 0 0\"/>"
                     "<AbbreviatedData>M 0 0 L 40 0 L 40 25 L 0 25 C</AbbreviatedData>"
                     "</PathObject></Layer></Content></Page>"},
        {"Second.xml", "<Page><Area><PhysicalBox>3 5 100 120</PhysicalBox></Area><Content/></Page>"}
    };
    for (auto it = entries.cbegin(); it != entries.cend(); ++it) {
        QFile file(fixture.filePath(it.key()));
        verify(file.open(QIODevice::WriteOnly) && file.write(it.value()) == it.value().size(), "fixture file");
    }
    QProcess zip;
    zip.setWorkingDirectory(fixture.path());
    zip.start("cmake", QStringList{"-E", "tar", "cf", "smoke.ofd", "--format=zip"} + entries.keys());
    verify(zip.waitForFinished() && zip.exitCode() == 0, "fixture archive");
    UrlSink sink;
    QDesktopServices::setUrlHandler("https", &sink, "capture");
    DocSheet sheet(Dr::OFD, fixture.filePath("smoke.ofd"));
    sheet.resize(1100, 800);
    sheet.show();
    verify(sheet.openFileExec(QString()), "open OFD in real DocSheet");
    settle();
    auto *browser = sheet.getSheetBrowser();
    auto *catalog = sheet.findChild<CatalogTreeView *>();
    verify(browser && catalog, "OFD has browser and catalog widgets");
    catalog->handleOpenSuccess();
    verify(catalog->model()->rowCount() == 2, "root catalog rows");
    QModelIndex leaf;
    for (int i = 0; i < 9; ++i) {
        leaf = catalog->model()->index(0, 0, leaf);
        verify(leaf.isValid(), "full catalog depth");
        if (i < 8)
            verify(catalog->isExpanded(leaf), "document expansion defaults");
    }
    verify(sheet.currentPage() == 1, "population does not navigate");
    catalog->setCurrentIndex(leaf);
    verify(sheet.currentPage() == 1, "selection does not navigate");
    QKeyEvent enter(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QCoreApplication::sendEvent(catalog, &enter);
    verify(sheet.currentPage() == 2 && qAbs(sheet.operation().scaleFactor - 2) < .001,
           "keyboard activation uses target page and zoom");
    catalog->restoreExpandedSections({});
    verify(catalog->getExpandedSections().isEmpty(), "all-collapsed saved state wins");

    const QModelIndex website = catalog->model()->index(1, 1);
    int dialogs = 0;
    QTimer cancel;
    QObject::connect(&cancel, &QTimer::timeout, [&] {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            if (auto *dialog = qobject_cast<SecurityDialog *>(widget); dialog && dialog->isVisible()) {
                ++dialogs;
                dialog->reject();
            }
        }
    });
    cancel.start(10);
    catalog->setCurrentIndex(website);
    settle();
    verify(dialogs == 0 && sink.calls == 0, "URI selection has no effects");
    QCoreApplication::sendEvent(catalog, &enter);
    verify(dialogs == 1 && sink.calls == 0, "URI keyboard activation confirms once and cancellation blocks opening");
    QMetaObject::invokeMethod(catalog, "onItemClicked", Qt::DirectConnection, Q_ARG(QModelIndex, website));
    verify(dialogs == 2 && sink.calls == 0, "URI click confirms once and cancellation blocks opening");

    for (int rotation = 0; rotation < 4; ++rotation) {
        if (rotation)
            sheet.rotateRight();
        for (Dr::LayoutMode layout : {Dr::SinglePageMode, Dr::TwoPagesMode}) {
            sheet.setLayoutMode(layout);
            for (DestinationMode mode : {DestinationMode::XYZ, DestinationMode::Fit, DestinationMode::FitH,
                                         DestinationMode::FitV, DestinationMode::FitR}) {
                NavigationDestination destination;
                destination.pageIndex = 1;
                destination.mode = mode;
                destination.left = 20;
                destination.top = 30;
                destination.right = 200;
                destination.bottom = 250;
                destination.zoom = 1.5;
                NavigationTarget target;
                target.destination = destination;
                verify(sheet.navigateTo(target), "navigate mode/rotation/layout");
                verify(sheet.currentPage() == 2 && std::isfinite(sheet.operation().scaleFactor), "valid navigation state");
                BrowserPage *page = nullptr;
                for (auto *item : browser->scene()->items())
                    if (auto *candidate = dynamic_cast<BrowserPage *>(item); candidate && candidate->itemIndex() == 1)
                        page = candidate;
                verify(page, "target scene item");
                const auto view = navigationView(destination, sheet.renderer()->getPageSize(1),
                    QSizeF(browser->viewport()->size()), {}, sheet.operation().scaleFactor,
                    sheet.maxScaleFactor(), rotation * 90, layout == Dr::TwoPagesMode);
                verify(view.has_value(), "navigation reference");
                const qreal scale = sheet.operation().scaleFactor;
                const QPointF expected = page->mapRectToScene(QRectF(view->focusRect.topLeft() * scale,
                                                                    view->focusRect.size() * scale)).topLeft();
                verify(browser->horizontalScrollBar()->value() == qRound(qBound(qreal(browser->horizontalScrollBar()->minimum()), expected.x(), qreal(browser->horizontalScrollBar()->maximum())))
                    && browser->verticalScrollBar()->value() == qRound(qBound(qreal(browser->verticalScrollBar()->minimum()), expected.y(), qreal(browser->verticalScrollBar()->maximum()))),
                    "scroll aligns to transformed target");
            }
        }
    }
    // Capture the old viewport position independently of navigationView. This
    // exercises omitted-coordinate conversion through a rotated, scrolled page.
    sheet.setLayoutMode(Dr::SinglePageMode);
    sheet.setScaleFactor(2);
    sheet.jumpToPage(2);
    BrowserPage *sourcePage = nullptr;
    BrowserPage *targetPage = nullptr;
    for (auto *item : browser->scene()->items()) {
        if (auto *page = dynamic_cast<BrowserPage *>(item)) {
            if (page->itemIndex() == 1)
                sourcePage = page;
            if (page->itemIndex() == 0)
                targetPage = page;
        }
    }
    verify(sourcePage && targetPage, "omitted-axis scene items");
    browser->horizontalScrollBar()->setValue(browser->horizontalScrollBar()->maximum() / 2);
    const QPointF priorPosition = sourcePage->mapFromScene(browser->mapToScene(QPoint(0, 0))) / 2;
    NavigationTarget partial;
    partial.destination = NavigationDestination{};
    partial.destination->pageIndex = 0;
    partial.destination->top = 40;
    partial.destination->zoom = 0;
    verify(sheet.navigateTo(partial), "rotated XYZ with omitted left");
    verify(sheet.operation().scaleFactor == 2, "zero zoom keeps the old scale");
    const QPointF expectedPartial = targetPage->mapToScene(
        QPointF(qBound(qreal(0), priorPosition.x(), sheet.renderer()->getPageSize(0).width()), 40) * 2);
    verify(browser->horizontalScrollBar()->value() == qRound(qBound(qreal(browser->horizontalScrollBar()->minimum()), expectedPartial.x(), qreal(browser->horizontalScrollBar()->maximum())))
        && browser->verticalScrollBar()->value() == qRound(qBound(qreal(browser->verticalScrollBar()->minimum()), expectedPartial.y(), qreal(browser->verticalScrollBar()->maximum()))),
        "omitted axis retains captured current-page position");
    const auto oldScale = sheet.operation().scaleFactor;
    NavigationTarget invalid;
    invalid.destination = NavigationDestination{};
    invalid.destination->pageIndex = 999;
    verify(!sheet.navigateTo(invalid) && sheet.operation().scaleFactor == oldScale,
           "out-of-range target has no effects");
    verify(sink.calls == 0, "no network navigation attempted");
    QDesktopServices::unsetUrlHandler("https");
    qInfo("PASS: %d real-widget navigation checks", checks);
    return 0;
}
#include "navigation_smoke.moc"
