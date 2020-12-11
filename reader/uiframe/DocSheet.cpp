/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar Sheets)
*
* Sheet(SheetSidebar SheetBrowser document)
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
#include "DocSheet.h"
#include "SheetSidebar.h"
#include "SheetBrowser.h"
#include "Database.h"
#include "CentralDocPage.h"
#include "MsgHeader.h"
#include "EncryptionPage.h"
#include "PDFModel.h"
#include "DPrintPreviewDialog"
#include "FileAttrWidget.h"
#include "Application.h"
#include "Utils.h"
#include "DocThread.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStackedWidget>
#include <QMimeData>
#include <QUuid>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QClipboard>
#include <QFileInfo>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QDebug>

DWIDGET_USE_NAMESPACE

QMap<QString, DocSheet *> DocSheet::g_map;
DocSheet::DocSheet(Dr::FileType fileType, QString filePath,  QWidget *parent)
    : DSplitter(parent), m_filePath(filePath), m_fileType(fileType)
{
    setAlive(true);

    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_browser = new SheetBrowser(this);
    m_browser->setMinimumWidth(481);

    if (Dr::PDF == fileType)
        m_sidebar = new SheetSidebar(this, PREVIEW_THUMBNAIL | PREVIEW_CATALOG | PREVIEW_BOOKMARK | PREVIEW_NOTE);
    else if (Dr::DJVU == fileType)
        m_sidebar = new SheetSidebar(this, PREVIEW_THUMBNAIL | PREVIEW_BOOKMARK);
    else
        m_sidebar = new SheetSidebar(this, nullptr);

    m_sidebar->setMinimumWidth(266);

    connect(m_browser, SIGNAL(sigPageChanged(int)), this, SLOT(onBrowserPageChanged(int)));
    connect(m_browser, SIGNAL(sigNeedPagePrev()), this, SLOT(onBrowserPagePrev()));
    connect(m_browser, SIGNAL(sigNeedPageNext()), this, SLOT(onBrowserPageNext()));
    connect(m_browser, SIGNAL(sigNeedPageFirst()), this, SLOT(onBrowserPageFirst()));
    connect(m_browser, SIGNAL(sigNeedPageLast()), this, SLOT(onBrowserPageLast()));
    connect(m_browser, SIGNAL(sigNeedBookMark(int, bool)), this, SLOT(onBrowserBookmark(int, bool)));
    connect(m_browser, SIGNAL(sigOperaAnnotation(int, int, deepin_reader::Annotation *)), this, SLOT(onBrowserOperaAnnotation(int, int, deepin_reader::Annotation *)));
    connect(m_browser, SIGNAL(sigPartThumbnailUpdated(int)), m_sidebar, SLOT(handleUpdatePartThumbnail(int)));
    connect(m_browser, SIGNAL(sigThumbnailUpdated(int)), m_sidebar, SLOT(handleUpdateThumbnail(int)));

    this->addWidget(m_sidebar);
    this->addWidget(m_browser);
}

DocSheet::~DocSheet()
{
    setAlive(false);

    delete m_browser;

    delete m_sidebar;

    if (nullptr != m_document)
        delete m_document;

    qDeleteAll(m_pages);
}

QImage DocSheet::firstThumbnail(const QString &filePath)
{
    for (auto iter = g_map.begin(); iter != g_map.end(); iter++) {
        if (iter.value()->filePath() == filePath) {
            QImage image;
            iter.value()->getImage(0, image, 100, 100);
            return image;
        }
    }

    return SheetBrowser::firstThumbnail(filePath);
}

bool DocSheet::existFileChanged()
{
    foreach (DocSheet *sheet, g_map.values()) {
        if (sheet->fileChanged())
            return true;
    }

    return false;
}

QUuid DocSheet::getUuid(DocSheet *sheet)
{
    return g_map.key(sheet);
}

bool DocSheet::existSheet(DocSheet *sheet)
{
    return g_map.values().contains(sheet);
}

DocSheet *DocSheet::getSheet(QString uuid)
{
    if (g_map.contains(uuid))
        return g_map[uuid];

    return nullptr;
}

bool DocSheet::openFileExec(const QString &password, QString &error)
{
    QEventLoop loop;
    connect(this, &DocSheet::sigFileOpened, &loop, &QEventLoop::quit);
    openFileAsync(password);
    loop.exec();

    error = m_lastError;
    return error.isEmpty();
}

void DocSheet::openFileAsync(const QString &password)
{
    m_password = password;

    DocOpenThreadTask task;

    task.sheet = this;

    DocThread::appendTask(task);
}

void DocSheet::jumpToPage(int page)
{
    m_browser->setCurrentPage(page);
}

void DocSheet::jumpToIndex(int index)
{
    m_browser->setCurrentPage(index + 1);
}

void DocSheet::jumpToFirstPage()
{
    m_browser->setCurrentPage(1);
}

void DocSheet::jumpToLastPage()
{
    jumpToPage(m_browser->allPages());
}

void DocSheet::jumpToNextPage()
{
    int page = m_browser->currentPage() + (m_operation.layoutMode == Dr::TwoPagesMode ? 2 : 1);

    page = page >= m_browser->allPages() ? m_browser->allPages() : page;

    jumpToPage(page);
}

void DocSheet::jumpToPrevPage()
{
    int page = m_browser->currentPage() - (m_operation.layoutMode == Dr::TwoPagesMode ? 2 : 1);

    page = page < 1 ? 1 : page;

    jumpToPage(page);
}

deepin_reader::Outline DocSheet::outline()
{
    return m_browser->outline();
}

void DocSheet::jumpToOutline(const qreal  &left, const qreal &top, int index)
{
    m_browser->jumpToOutline(left, top, index);
}

void DocSheet::jumpToHighLight(deepin_reader::Annotation *annotation, const int index)
{
    m_browser->jumpToHighLight(annotation, index);
}

void DocSheet::rotateLeft()
{
    if (Dr::RotateBy0 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy270;
    else if (Dr::RotateBy270 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy180;
    else if (Dr::RotateBy180 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy90;
    else if (Dr::RotateBy90 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy0;

    m_browser->deform(m_operation);
    m_sidebar->handleRotate();
    setOperationChanged();
}

void DocSheet::rotateRight()
{
    if (Dr::RotateBy0 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy90;
    else if (Dr::RotateBy90 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy180;
    else if (Dr::RotateBy180 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy270;
    else if (Dr::RotateBy270 == m_operation.rotation)
        m_operation.rotation = Dr::RotateBy0;

    m_browser->deform(m_operation);
    m_sidebar->handleRotate();
    setOperationChanged();
}

void DocSheet::setBookMark(int index, int state)
{
    if (index < 0 || index >= pagesNumber())
        return;

    if (state)
        m_bookmarks.insert(index);
    else {
        showTips(tr("The bookmark has been removed"));
        m_bookmarks.remove(index);
    }

    if (m_sidebar)
        m_sidebar->setBookMark(index, state);

    m_browser->setBookMark(index, state);

    setBookmarkChanged(true);
}

void DocSheet::setBookMarks(const QList<int> &indexlst, int state)
{
    for (int index : indexlst) {
        if (state)
            m_bookmarks.insert(index);
        else {
            m_bookmarks.remove(index);
        }

        if (m_sidebar)
            m_sidebar->setBookMark(index, state);

        m_browser->setBookMark(index, state);
    }

    if (!state)
        showTips(tr("The bookmark has been removed"));

    setBookmarkChanged(true);
}

int DocSheet::pagesNumber()
{
    if (m_browser)
        return m_browser->allPages();

    return 0;
}

int DocSheet::currentPage()
{
    if (m_operation.currentPage < 1 || m_operation.currentPage > pagesNumber())
        return 1;

    return m_operation.currentPage;
}

int DocSheet::currentIndex()
{
    if (m_operation.currentPage < 1 || m_operation.currentPage > pagesNumber())
        return 0;

    return m_operation.currentPage - 1;
}

void DocSheet::setLayoutMode(Dr::LayoutMode mode)
{
    if (mode == m_operation.layoutMode)
        return;

    if (mode >= Dr::SinglePageMode && mode < Dr::NumberOfLayoutModes) {
        m_operation.layoutMode = mode;
        m_browser->deform(m_operation);
        setOperationChanged();
    }
}

void DocSheet::setMouseShape(Dr::MouseShape shape)
{
    if (shape >= Dr::MouseShapeNormal && shape < Dr::NumberOfMouseShapes) {
        closeMagnifier();
        m_operation.mouseShape = shape;
        m_browser->setMouseShape(m_operation.mouseShape);
        setOperationChanged();
    }
}

void DocSheet::setAnnotationInserting(bool inserting)
{
    if (nullptr == m_browser)
        return;

    m_browser->setAnnotationInserting(inserting);
}

void DocSheet::setScaleMode(Dr::ScaleMode mode)
{
    if (mode >= Dr::ScaleFactorMode && mode <= Dr::FitToPageWorHMode) {
        m_operation.scaleMode = mode;

        m_browser->deform(m_operation);

        setOperationChanged();

    }
}

void DocSheet::setScaleFactor(qreal scaleFactor)
{
    if (Dr::ScaleFactorMode == m_operation.scaleMode && qFuzzyCompare(scaleFactor, m_operation.scaleFactor))
        return;

    if (scaleFactor > maxScaleFactor())
        scaleFactor = maxScaleFactor();

    m_operation.scaleMode = Dr::ScaleFactorMode;

    m_operation.scaleFactor = scaleFactor;

    m_browser->deform(m_operation);

    setOperationChanged();
}

bool DocSheet::getImage(int index, QImage &image, int width, int height, bool bSrc)
{
    return m_browser->getImage(index, image, width, height, bSrc);
}

bool DocSheet::fileChanged()
{
    return (m_documentChanged || m_bookmarkChanged);
}

bool DocSheet::saveData()
{
    PERF_PRINT_BEGIN("POINT-04", QString("filename=%1,filesize=%2").arg(QFileInfo(this->filePath()).fileName()).arg(QFileInfo(this->filePath()).size()));
    if (m_documentChanged && !m_browser->save())
        return false;

    m_documentChanged = false;

    if (m_bookmarkChanged && !Database::instance()->saveBookmarks(filePath(), m_bookmarks))
        return false;

    m_bookmarkChanged = false;

    m_sidebar->changeResetModelData();

    dApp->unBlockShutdown();

    PERF_PRINT_END("POINT-04", "");

    return true;
}

bool DocSheet::saveAsData(QString filePath)
{
    stopSearch();

    if (m_documentChanged) {
        if (!m_browser->saveAs(filePath))
            return false;
    } else {
        if (!Utils::copyFile(this->filePath(), filePath))
            return false;
    }

    Database::instance()->saveBookmarks(filePath, m_bookmarks);

    m_sidebar->changeResetModelData();

    return true;
}

void DocSheet::handleOpened(bool result, QString error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages)
{
    m_lastError = error;
    m_document = document;
    m_pages = pages;

    if (!m_browser->init(document, pages, m_operation, m_bookmarks))
        emit sigFileOpened(this, false, tr("Please check if the file is damaged"));

    m_sidebar->handleOpenSuccess();

    emit sigFileOpened(this, result, error);

    emit sigOperationChanged(this);
}

void DocSheet::handleSearch()
{
    m_browser->handleSearch();
}

void DocSheet::stopSearch()
{
    m_browser->stopSearch();
}

void DocSheet::onFindContentComming(const deepin_reader::SearchResult &res)
{
    m_sidebar->handleFindContentComming(res);
}

void DocSheet::copySelectedText()
{
    QString selectedWordsText = m_browser->selectedWordsText();
    if (selectedWordsText.isEmpty())
        return;

    QClipboard *clipboard = DApplication::clipboard();  //获取系统剪贴板指针
    clipboard->setText(selectedWordsText);
}

void DocSheet::highlightSelectedText()
{
    if (!m_browser || fileType() != Dr::FileType::PDF)
        return;

    if (m_browser->selectedWordsText().isEmpty()) {
        showTips(tr("Please select the text"), 1);
        return;
    }

    QPoint ponintend;

    m_browser->addHighLightAnnotation("", Utils::getCurHiglightColor(), ponintend);
}

void DocSheet::addSelectedTextHightlightAnnotation()
{
    if (!m_browser || fileType() != Dr::FileType::PDF)
        return;

    if (m_browser->selectedWordsText().isEmpty()) {
        showTips(tr("Please select the text"), 1);
        return;
    }

    //进行高亮编辑
    QPoint ponintend;

    Annotation *annot = m_browser->addHighLightAnnotation("", Utils::getCurHiglightColor(), ponintend);

    if (annot)
        m_browser->showNoteEditWidget(annot, ponintend);
}

void DocSheet::openMagnifier()
{
    if (m_browser)
        m_browser->openMagnifier();
}

void DocSheet::closeMagnifier()
{
    if (m_browser)
        m_browser->closeMagnifier();
}

void DocSheet::defaultFocus()
{
    if (isUnLocked() && m_browser)
        m_browser->setFocus();
}

bool DocSheet::magnifierOpened()
{
    if (m_browser)
        return m_browser->magnifierOpened();
    return false;
}

QList<deepin_reader::Annotation *> DocSheet::annotations()
{
    if (nullptr == m_browser)
        return QList< deepin_reader::Annotation * > ();
    return m_browser->annotations();
}

bool DocSheet::removeAnnotation(deepin_reader::Annotation *annotation, bool tips)
{
    int ret = m_browser->removeAnnotation(annotation);
    if (ret) {
        if (tips) this->showTips(tr("The annotation has been removed"));
    }
    return ret;
}

bool DocSheet::removeAllAnnotation()
{
    bool ret = m_browser->removeAllAnnotation();
    if (ret) {
        this->showTips(tr("The annotation has been removed"));
    }
    return ret;
}

QList<qreal> DocSheet::scaleFactorList()
{
    QList<qreal> dataList = {0.1, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 3, 4, 5};
    QList<qreal> factorList;

    qreal maxFactor = maxScaleFactor();

    foreach (qreal factor, dataList) {
        if (maxFactor - factor > -0.0001)
            factorList.append(factor);
    }

    return  factorList;
}

qreal DocSheet::maxScaleFactor()
{
    qreal maxScaleFactor = 20000 / (m_browser->maxHeight() * qApp->devicePixelRatio());

    maxScaleFactor = qBound(0.1, maxScaleFactor, 5.0);

    return maxScaleFactor;
}

QString DocSheet::filter()
{
    if (Dr::PDF == m_fileType)
        return  "Pdf File (*.pdf)";
    else if (Dr::DJVU == m_fileType)
        return "Djvu files (*.djvu)";

    return "";
}

QString DocSheet::format()
{
    if (Dr::PDF == m_fileType) {
        const Properties &propertys = m_browser->properties();
        return QString("PDF %1").arg(propertys.value("Version").toString());
    } else if (Dr::DJVU == m_fileType) {
        return QString("DJVU");
    }
    return "";
}

QSet<int> DocSheet::getBookMarkList() const
{
    return m_bookmarks;
}

SheetOperation DocSheet::operation() const
{
    return m_operation;
}

SheetOperation &DocSheet::operationRef()
{
    return m_operation;
}

Dr::FileType DocSheet::fileType()
{
    return m_fileType;
}

QString DocSheet::filePath()
{
    return m_filePath;
}

QString DocSheet::password()
{
    return m_password;
}

bool DocSheet::hasBookMark(int index)
{
    return m_bookmarks.contains(index);
}

void DocSheet::zoomin()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = 0; i < dataList.count(); ++i) {
        if (dataList[i] > (m_operation.scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

void DocSheet::zoomout()
{
    QList<qreal> dataList = scaleFactorList();

    for (int i = dataList.count() - 1; i >= 0; --i) {
        if (dataList[i] < (m_operation.scaleFactor)) {
            setScaleFactor(dataList[i]);
            return;
        }
    }
}

void DocSheet::showTips(const QString &tips, int iconIndex)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->showTips(tips, iconIndex);
}

void DocSheet::onPrintRequested(DPrinter *printer)
{
    printer->setDocName(QFileInfo(filePath()).fileName());

    printer->setFullPage(true);

    QPainter painter(printer);

    const QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

    int pagesCount = pagesNumber();

    int fromIndex = printer->fromPage() <= 0 ? 0 : printer->fromPage() - 1;

    int toIndex = printer->toPage() <= 0 ? pagesCount - 1 : printer->toPage() - 1;

    for (int index = fromIndex; index <= toIndex; index++) {
        if (index >= pagesCount)
            break;

        QImage image;
        if (getImage(index, image, static_cast<int>(pageRect.width()), static_cast<int>(pageRect.height()))) {
            painter.drawImage(QRect(0, 0, static_cast<int>(pageRect.width()), static_cast<int>(pageRect.height())), image);
        }

        if (index != toIndex)
            printer->newPage();
    }
}

void DocSheet::openSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->openSlide();
}

void DocSheet::closeSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->quitSlide();
}

void DocSheet::setSidebarVisible(bool isVisible, bool notify)
{
    if (notify) {
        m_sidebar->setVisible(isVisible);
        m_operation.sidebarVisible = isVisible;

        if (isVisible) {
            this->insertWidget(0, m_sidebar);
        } else if (isFullScreen()) {
            m_sidebar->setParent(nullptr);
            m_sidebar->setParent(this);

            m_sidebar->resize(m_sidebar->width(), dApp->desktop()->screenGeometry().height());
            m_sidebar->move(-m_sidebar->width(), 0);
            m_sidebar->setVisible(false);
        }

        setOperationChanged();
    } else {
        if (m_sideAnimation == nullptr) {
            m_sideAnimation = new QPropertyAnimation(m_sidebar, "movepos");
            connect(m_sideAnimation, &QPropertyAnimation::finished, this, &DocSheet::onSideAniFinished);
        }

        if (m_sideAnimation->state() == QPropertyAnimation::Running && m_fullSiderBarVisible == isVisible)
            return;

        if (isFullScreen() && this->indexOf(m_sidebar) >= 0) {
            m_sidebar->setParent(nullptr);
            m_sidebar->setParent(this);

            m_sidebar->resize(m_sidebar->width(), dApp->desktop()->screenGeometry().height());
            m_sidebar->move(-m_sidebar->width(), 0);
            m_sidebar->setVisible(false);
        }

        m_sideAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_fullSiderBarVisible = isVisible;
        m_sideAnimation->stop();
        int duration = 300 * (m_sidebar->width() + m_sidebar->pos().x()) / m_sidebar->width();
        duration = duration <= 0 ? 300 : duration;
        m_sideAnimation->setDuration(duration);
        m_sideAnimation->setStartValue(QPoint(m_sidebar->pos().x(), 0));
        if (isVisible) {
            m_sidebar->setVisible(true);
            m_sideAnimation->setEndValue(QPoint(0, 0));
        } else {
            m_sideAnimation->setEndValue(QPoint(-m_sidebar->width(), 0));
        }
        m_sideAnimation->start();
    }
}

void DocSheet::onSideAniFinished()
{
    if (m_sidebar->pos().x() < 0) {
        m_sidebar->setVisible(false);
    }
}

void DocSheet::onOpenFileAsyncFinished(bool result, QString error)
{

}

bool DocSheet::isFullScreen()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)

        return false;
    return doc->isFullScreen();
}

void DocSheet::openFullScreen()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    setSidebarVisible(false);
    m_sidebar->setParent(nullptr);
    m_sidebar->setParent(this);

    m_sidebar->resize(m_sidebar->width(), dApp->desktop()->screenGeometry().height());
    m_sidebar->move(-m_sidebar->width(), 0);
    m_sidebar->setVisible(false);

    if (m_browser)
        m_browser->hideSubTipsWidget();

    doc->openFullScreen();
}

bool DocSheet::closeFullScreen(bool force)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return false;

    if (m_browser)
        m_browser->hideSubTipsWidget();

    if (force) {
        this->insertWidget(0, m_sidebar);
        m_sidebar->move(0, 0);
        m_sidebar->setVisible(this->operation().sidebarVisible);

        return doc->quitFullScreen(force);
    } else if (doc->topLevelWidget()->windowState().testFlag(Qt::WindowFullScreen)) {
        doc->topLevelWidget()->showNormal();
        return true;
    }
    return false;
}

void DocSheet::setDocumentChanged(bool changed)
{
    m_documentChanged = changed;

    emit sigFileChanged(this);
}

void DocSheet::setBookmarkChanged(bool changed)
{
    m_bookmarkChanged = changed;

    emit sigFileChanged(this);
}

void DocSheet::setOperationChanged()
{

}

bool DocSheet::haslabel()
{
    if (nullptr == m_browser)
        return false;

    return  m_browser->pageHasLable();
}

void DocSheet::docBasicInfo(deepin_reader::FileInfo &tFileInfo)
{
    QFileInfo fileInfo(filePath());
    tFileInfo.size = fileInfo.size();
    tFileInfo.createTime = fileInfo.birthTime();
    tFileInfo.changeTime = fileInfo.lastModified();
    tFileInfo.auther = fileInfo.owner();
    tFileInfo.filePath = fileInfo.filePath();

    const Properties &propertys = m_browser->properties();
    tFileInfo.format = format();
    tFileInfo.optimization = propertys.value("Linearized").toBool();
    tFileInfo.keyword = propertys.value("KeyWords").toString();
    tFileInfo.theme = propertys.value("Title").toString();
    tFileInfo.producter = propertys.value("Producer").toString();
    tFileInfo.creater = propertys.value("Creator").toString();
    tFileInfo.safe = propertys.value("Encrypted").toBool();
    tFileInfo.width = static_cast<unsigned int>(m_browser->maxWidth());
    tFileInfo.height = static_cast<unsigned int>(m_browser->maxHeight());
    tFileInfo.numpages = static_cast<unsigned int>(m_browser->allPages());
}

void DocSheet::onBrowserPageChanged(int page)
{
    if (m_operation.currentPage != page) {
        m_operation.currentPage = page;
        if (m_sidebar)
            m_sidebar->setCurrentPage(page);
    }
}

void DocSheet::onBrowserPageFirst()
{
    jumpToFirstPage();
}

void DocSheet::onBrowserPagePrev()
{
    jumpToPrevPage();
}

void DocSheet::onBrowserPageNext()
{
    jumpToNextPage();
}

void DocSheet::onBrowserPageLast()
{
    jumpToLastPage();
}

void DocSheet::onBrowserBookmark(int index, bool state)
{
    setBookMark(index, state);
}

void DocSheet::onBrowserOperaAnnotation(int type, int index, deepin_reader::Annotation *anno)
{
    m_sidebar->handleAnntationMsg(type, index, anno);
    setDocumentChanged(true);
}

void DocSheet::handleFindNext()
{
    m_browser->handleFindNext();
}

void DocSheet::handleFindPrev()
{
    m_browser->handleFindPrev();
}

void DocSheet::handleFindExit()
{
    m_browser->handleFindExit();
    m_sidebar->handleFindOperation(E_FIND_EXIT);
    emit sigFindOperation(E_FIND_EXIT);
}

void DocSheet::handleFindContent(const QString &strFind)
{
    m_browser->handleFindContent(strFind);
    m_sidebar->handleFindOperation(E_FIND_CONTENT, strFind);
    emit sigFindOperation(E_FIND_CONTENT);
}

void DocSheet::onFindFinished()
{
    int count = m_sidebar->handleFindFinished();
    m_browser->handleFindFinished(count);
}

void DocSheet::resizeEvent(QResizeEvent *event)
{
    DSplitter::resizeEvent(event);
    if (m_encryPage) {
        m_encryPage->setGeometry(0, 0, this->width(), this->height());
    }

    if (isFullScreen()) {
        m_sidebar->resize(m_sidebar->width(), this->height());
    }
}

void DocSheet::childEvent(QChildEvent *event)
{
    //Not todO;
    if (event->removed()) {
        return DSplitter::childEvent(event);
    }
}

void DocSheet::setAlive(bool alive)
{
    if (alive) {
        if (!m_uuid.isEmpty())
            setAlive(false);

        m_uuid = QUuid::createUuid().toString();
        g_map[m_uuid] = this;
        Database::instance()->readOperation(this);
        Database::instance()->readBookmarks(m_filePath, m_bookmarks);
    } else {
        if (m_uuid.isEmpty())
            return;

        stopSearch();
        Database::instance()->saveOperation(this);
        g_map.remove(m_uuid);
        m_uuid.clear();
    }
}

void DocSheet::showEncryPage()
{
    if (m_encryPage == nullptr) {
        m_encryPage = new EncryptionPage(this);
        connect(m_encryPage, &EncryptionPage::sigExtractPassword, this, &DocSheet::onExtractPassword);
        this->stackUnder(m_encryPage);
    }

    //bugid:46645 密码输入框在的时候,先暂时屏蔽掉browser的焦点
    m_browser->setFocusPolicy(Qt::NoFocus);

    m_encryPage->setGeometry(0, 0, this->width(), this->height());
    m_encryPage->raise();
    m_encryPage->show();
}

bool DocSheet::needPassword()
{
    int status = -1;
    if (Dr::PDF == m_fileType)
        status = deepin_reader::PDFDocument::tryLoadDocument(filePath(), QString());

    if (status == deepin_reader::Document::PASSWORD_ERROR)
        return true;

    return false;
}

bool DocSheet::isUnLocked()
{
    return m_browser->isUnLocked();
}

int DocSheet::getIndexByPageLable(const QString &pageLable)
{
    if (m_browser == nullptr)
        return -1;

    return m_browser->pageLableIndex(pageLable);
}

QString DocSheet::getPageLabelByIndex(const int &index)
{
    if (nullptr == m_browser)
        return "";

    return m_browser->pageNum2Lable(index);
}

bool DocSheet::tryPassword(QString password)
{
    int status = -1;
    if (Dr::PDF == m_fileType)
        status = deepin_reader::PDFDocument::tryLoadDocument(filePath(), password);

    if (status == deepin_reader::Document::SUCCESS) {
        return true;
    }
    return false;
}

void DocSheet::onExtractPassword(const QString &password)
{
    bool ret = this->tryPassword(password);
    if (ret) {
        m_browser->setFocusPolicy(Qt::StrongFocus);
        m_encryPage->hide();

        this->openFileAsync(password);
        this->defaultFocus();

        m_encryPage->close();
        m_encryPage = nullptr;

        CentralDocPage *doc = dynamic_cast<CentralDocPage *>(parent());

        emit doc->sigCurSheetChanged(this);
    } else {
        m_encryPage->wrongPassWordSlot();
    }
}

void DocSheet::deadDeleteLater()
{
    setAlive(false);
    DSplitter::deleteLater();
}

void DocSheet::onPopPrintDialog()
{
    DPrintPreviewDialog preview(this);
#if (DTK_VERSION_MAJOR == 5 && DTK_VERSION_MAJOR == 3 && DTK_VERSION_MINOR == 2 )
    preview.setPrintFromPath(m_filePath);       //旧版本和最新版本使用新接口，解决打印模糊问题
#endif
    connect(&preview, &DPrintPreviewDialog::paintRequested, this, &DocSheet::onPrintRequested);
    preview.exec();
}

void DocSheet::onPopInfoDialog()
{
    FileAttrWidget pFileAttrWidget(this);
    pFileAttrWidget.setFileAttr(this);
    pFileAttrWidget.exec();
}

QSizeF DocSheet::pageSizeByIndex(int index)
{
    return m_browser->pageSizeByIndex(index);
}

