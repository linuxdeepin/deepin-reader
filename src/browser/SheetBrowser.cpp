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
#include "SheetBrowser.h"
#include "document/PDFModel.h"
#include "BrowserPage.h"
#include "DocSheet.h"
#include "BrowserWord.h"
#include "BrowserAnnotation.h"
#include "widgets/TipsWidget.h"
#include "BrowserMenu.h"
#include "widgets/PrintManager.h"
#include "widgets/FileAttrWidget.h"
#include "Application.h"
#include "sidebar/note/NoteViewWidget.h"
#include "BrowserMagniFier.h"
#include "widgets/FindWidget.h"
#include "MsgHeader.h"
#include "document/DjVuModel.h"
#include "Utils.h"
#include "RenderViewportThread.h"
#include "BrowserSearch.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>
#include <QBitmap>
#include <DMenu>
#include <DGuiApplicationHelper>
#include <QDomDocument>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QStandardPaths>
#include <QUuid>

const int ICONANNOTE_WIDTH = 24;

DWIDGET_USE_NAMESPACE

SheetBrowser::SheetBrowser(DocSheet *parent) : DGraphicsView(parent), m_sheet(parent)
{
    setScene(new QGraphicsScene());

    setFrameShape(QFrame::NoFrame);

    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("QGraphicsView{background-color:white}");     //由于DTK bug不响应WA_TranslucentBackground参数写死了颜色，这里加样式表让dtk style失效

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScrollBarValueChanged(int)));

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onHorizontalScrollBarValueChanged(int)));

    m_tipsWidget = new TipsWidget(this);

    m_searchTask = new BrowserSearch(this);

    qRegisterMetaType<deepin_reader::SearchResult>("deepin_reader::SearchResult");
    connect(m_searchTask, &BrowserSearch::sigSearchReady, m_sheet, &DocSheet::onFindContentComming, Qt::QueuedConnection);
    connect(m_searchTask, &BrowserSearch::finished, m_sheet, &DocSheet::onFindFinished, Qt::QueuedConnection);
    connect(this, SIGNAL(sigAddHighLightAnnot(BrowserPage *, QString, QColor)), this, SLOT(onAddHighLightAnnot(BrowserPage *, QString, QColor)));
}

SheetBrowser::~SheetBrowser()
{
    disconnect(this, SIGNAL(sigAddHighLightAnnot(BrowserPage *, QString, QColor)), this, SLOT(onAddHighLightAnnot(BrowserPage *, QString, QColor)));

    qDeleteAll(m_items);
    m_items.clear();

    if (nullptr != m_document)
        delete m_document;

    if (nullptr != m_noteEditWidget)
        delete m_noteEditWidget;
}

QImage SheetBrowser::firstThumbnail(const QString &filePath)
{
    deepin_reader::Document *document = nullptr;

    int fileType = Dr::fileType(filePath);

    if (Dr::PDF == fileType)
        document = deepin_reader::PDFDocument::loadDocument(filePath);
    else if (Dr::DjVu == fileType)
        document = deepin_reader::DjVuDocument::loadDocument(filePath);

    if (nullptr == document)
        return QImage();

    if (document->numberOfPages() <= 0)
        return QImage();

    deepin_reader::Page *page = document->page(0);

    if (page == nullptr)
        return QImage();

    QImage image = page->render(Dr::RotateBy0);

    delete page;

    delete document;

    return image;
}

bool SheetBrowser::open(const Dr::FileType &fileType, const QString &filePath)
{
    if (Dr::PDF == fileType)
        m_document = deepin_reader::PDFDocument::loadDocument(filePath);
    else if (Dr::DjVu == fileType)
        m_document = deepin_reader::DjVuDocument::loadDocument(filePath);

    if (nullptr == m_document)
        return false;

    m_fileType = fileType;
    m_filePath = filePath;

    return true;
}

bool SheetBrowser::reOpen(const Dr::FileType &fileType, const QString &filePath)
{
    if (nullptr == m_document)  //未打开的无法重新打开
        return false;

    deepin_reader::Document *tempDocument = m_document;

    if (Dr::PDF == fileType)
        m_document = deepin_reader::PDFDocument::loadDocument(filePath);
    else if (Dr::DjVu == fileType)
        m_document = deepin_reader::DjVuDocument::loadDocument(filePath);

    if (nullptr == m_document)
        return false;

    for (int i = 0; i < m_document->numberOfPages(); ++i) {
        if (m_items.count() > i)
            m_items[i]->reOpen(m_document->page(i));
    }

    delete tempDocument;

    return true;
}

bool SheetBrowser::save()
{
    if (m_filePath.isEmpty())
        return false;

    QString tmpFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QUuid::createUuid().toString() + ".tmp";

    if (!m_document->save(tmpFilePath, true)) {
        return false;
    }

    QFile tmpFile(tmpFilePath);

    if (!Utils::copyFile(tmpFilePath, m_filePath)) {
        tmpFile.remove();
        return false;
    }

    tmpFile.remove();

    return reOpen(m_fileType, m_filePath);
}

bool SheetBrowser::saveAs(const QString &filePath)
{
    if (!m_document->save(filePath, true))
        return false;

    return reOpen(m_fileType, filePath);
}

bool SheetBrowser::loadPages(SheetOperation &operation, const QSet<int> &bookmarks)
{
    if (nullptr == m_document)
        return false;

    for (int i = 0; i < m_document->numberOfPages(); ++i) {
        deepin_reader::Page *page = m_document->page(i);

        if (page == nullptr)
            return false;

        if (page->size().width() > m_maxWidth)
            m_maxWidth = page->size().width();

        if (page->size().height() > m_maxHeight)
            m_maxHeight = page->size().height();

        BrowserPage *item = new BrowserPage(this, page);

        item->setItemIndex(i);

        if (bookmarks.contains(i))
            item->setBookmark(true);

        m_items.append(item);

    }

    for (int i = 0; i < m_items.count(); ++i) {
        scene()->addItem(m_items[i]);
    }

    setMouseShape(operation.mouseShape);

    deform(operation);

    m_initPage = operation.currentPage;

    m_hasLoaded = true;

    return true;
}

void SheetBrowser::setMouseShape(const Dr::MouseShape &shape)
{
    closeMagnifier();
    if (Dr::MouseShapeHand == shape) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        foreach (BrowserPage *item, m_items) {
            item->setWordSelectable(false);
        }
    } else if (Dr::MouseShapeNormal == shape) {
        setDragMode(QGraphicsView::RubberBandDrag);
        foreach (BrowserPage *item, m_items)
            item->setWordSelectable(true);
    }
}

void SheetBrowser::setBookMark(int index, int state)
{
    if (m_items.count() > index) {
        m_items.at(index)->setBookmark(state);
    }
}

void SheetBrowser::setAnnotationInserting(bool inserting)
{
    m_annotationInserting = inserting;
}

void SheetBrowser::onVerticalScrollBarValueChanged(int)
{
    handleVerticalScrollLater();
    emit sigPageChanged(currentPage());
}

void SheetBrowser::onHorizontalScrollBarValueChanged(int)
{
    QList<QGraphicsItem *> items = scene()->items(mapToScene(this->rect()));

    foreach (QGraphicsItem *item, items) {
        BrowserPage *pdfItem = static_cast<BrowserPage *>(item);

        if (!m_items.contains(pdfItem))
            continue;

        if (nullptr == pdfItem)
            continue;

        pdfItem->renderViewPort();
    }
}

void SheetBrowser::onSceneOfViewportChanged()
{
    foreach (BrowserPage *page, m_items) {
        if (mapToScene(this->rect()).intersects(page->mapToScene(page->boundingRect()))) {
            page->loadWords();
            page->scaleWords(true);
            page->renderViewPort();
        }
    }
}

/**
 * @brief SheetBrowser::onAddHighLightAnnot
 */
void SheetBrowser::onAddHighLightAnnot(BrowserPage *page, QString text, QColor color)
{
    if (page)
        page->addHighlightAnnotation(text, color);
}

void SheetBrowser::showNoteEditWidget(deepin_reader::Annotation *annotation)
{
    if (annotation == nullptr) {
        Q_ASSERT(false && "showNoteEditWidget Annotation Is Null");
        return;
    }

    m_tipsWidget->hide();
    if (m_noteEditWidget == nullptr) {
        m_noteEditWidget = new NoteShadowViewWidget(this);
        connect(m_noteEditWidget->getNoteViewWidget(), &NoteViewWidget::sigNeedShowTips, m_sheet, &DocSheet::showTips);
        connect(m_noteEditWidget->getNoteViewWidget(), &NoteViewWidget::sigRemoveAnnotation, this, &SheetBrowser::onRemoveAnnotation);
        connect(m_noteEditWidget->getNoteViewWidget(), &NoteViewWidget::sigUpdateAnnotation, this, &SheetBrowser::onUpdateAnnotation);
    }
    m_noteEditWidget->getNoteViewWidget()->setEditText(annotation->contents());
    m_noteEditWidget->getNoteViewWidget()->setAnnotation(annotation);
    m_noteEditWidget->showWidget(QCursor::pos());
}

/**
 * @brief SheetBrowser::calcIconAnnotRect
 * 目前该计算方法
 * @param page
 * @param point
 * @param iconRect
 * @return
 */
bool SheetBrowser::calcIconAnnotRect(BrowserPage *page, const QPointF point, QRectF &iconRect)
{
    QPointF clickPoint;

    if (nullptr == page || nullptr == m_sheet)
        return false;

    //计算添加图标注释的位置和大小(考虑缩放和旋转)
    Dr::Rotation rotation{Dr::RotateBy0};
    qreal scaleFactor{1.0};
    SheetOperation  operation = m_sheet->operation();
    rotation    = operation.rotation;
    scaleFactor = operation.scaleFactor;

    qreal width{0.0};
    qreal height{0.0};
    qreal value{0.0};
    qreal x1{0};
    qreal y1{0};

    clickPoint = QPointF(point.x() - page->pos().x(), point.y() - page->pos().y());
    if (clickPoint.x() < 0 || clickPoint.y() < 0)
        return false;

    qreal x{0};
    qreal y{0};

    //计算坐标小于图标宽度情况
    qreal space = ICONANNOTE_WIDTH * scaleFactor;
    if (clickPoint.x() > ICONANNOTE_WIDTH / 2) {
        x = (clickPoint.x() + space / 2.) > (page->boundingRect().width()) ? static_cast<int>((page->boundingRect().width()) - space / 2.) : clickPoint.x();
    } else {
        x = (clickPoint.x() < space / 2.) ? space / 2. : clickPoint.x();
    }
    if (clickPoint.y() > space / 2.) {
        y = (clickPoint.y() + space / 2.) > (page->boundingRect().height()) ? static_cast<int>((page->boundingRect().height()) - space / 2.) : clickPoint.y();
    } else {
        y = (clickPoint.y() < space / 2.) ? space / 2. : clickPoint.y();
    }

    clickPoint.setX(x);
    clickPoint.setY(y);

    switch (rotation) {
    case Dr::RotateBy0 : {
        x1 = clickPoint.x() / (page->boundingRect().width());
        y1 = clickPoint.y() / (page->boundingRect().height());
        width  = ICONANNOTE_WIDTH *  scaleFactor / page->boundingRect().width();
        height = ICONANNOTE_WIDTH * scaleFactor / page->boundingRect().height();
    }
    break;
    case Dr::RotateBy90 : {
        clickPoint = QPointF(clickPoint.y(), page->boundingRect().width() - clickPoint.x());

        x1 = clickPoint.x() / (page->boundingRect().height());
        y1 = clickPoint.y() / (page->boundingRect().width());
        width  = ICONANNOTE_WIDTH * scaleFactor / page->boundingRect().height();
        height = ICONANNOTE_WIDTH * scaleFactor / page->boundingRect().width();
    }
    break;
    case Dr::RotateBy180 : {
        clickPoint = QPointF(page->boundingRect().width() - clickPoint.x(), page->boundingRect().height() - clickPoint.y());

        x1 = clickPoint.x() / (page->boundingRect().width());
        y1 = clickPoint.y() / (page->boundingRect().height());
        width  = ICONANNOTE_WIDTH * scaleFactor / page->boundingRect().width();
        height = ICONANNOTE_WIDTH * scaleFactor / page->boundingRect().height();
    }
    break;
    case Dr::RotateBy270 : {
        clickPoint = QPointF(page->boundingRect().height() - clickPoint.y(), clickPoint.x());

        x1 = clickPoint.x() / (page->boundingRect().height());
        y1 = clickPoint.y() / (page->boundingRect().width());
        width  = ICONANNOTE_WIDTH * scaleFactor / page->boundingRect().height();
        height = ICONANNOTE_WIDTH * scaleFactor / page->boundingRect().width();
    }
    break;
    default:
        break;
    };

    value = x1 - width / 2.0;
    iconRect.setX((value < 0.0) ? 0.0 : value);
    value = y1 - height / 2.0;
    iconRect.setY((value < 0.0) ? 0.0 : value);

    iconRect.setWidth(width);
    iconRect.setHeight(height);

    return true;
}

/**
 * @brief SheetBrowser::translate2Local
 * 将非0度的坐标点转换成0度的坐标点
 * @return
 */
QPointF SheetBrowser::translate2Local(const QPointF clickPoint)
{
    QPointF point = clickPoint;
    BrowserPage *page{nullptr};

    page = mouseClickInPage(point);

    if (nullptr == m_sheet || nullptr == page || clickPoint.x() < 0 || clickPoint.y() < 0)
        return  point;

    Dr::Rotation rotation{Dr::RotateBy0};
    SheetOperation  operation = m_sheet->operation();
    rotation    = operation.rotation;

    point = QPointF(point.x() - page->pos().x(), point.y() - page->pos().y());

    switch (rotation) {
    case Dr::RotateBy0 :
        point = QPointF(abs(point.x()) / page->boundingRect().width(),
                        abs(point.y()) / page->boundingRect().height());
        break;
    case Dr::RotateBy90 : {
        point = QPointF(point.y(), page->boundingRect().width() - point.x());
        point = QPointF(abs(point.x()) / page->boundingRect().height(),
                        abs(point.y()) / page->boundingRect().width());
    }
    break;
    case Dr::RotateBy180 : {
        point = QPointF(page->boundingRect().width() - point.x(), page->boundingRect().height() - point.y());
        point = QPointF(abs(point.x()) / page->boundingRect().width(),
                        abs(point.y()) / page->boundingRect().height());
    }
    break;
    case Dr::RotateBy270 : {
        point = QPointF(page->boundingRect().height() - point.y(), point.x());
        point = QPointF(abs(point.x()) / page->boundingRect().height(),
                        abs(point.y()) / page->boundingRect().width());
    }
    break;
    default:
        break;
    };

    return  point;
}

Annotation *SheetBrowser::getClickAnnot(const QPointF clickPoint, bool drawRect)
{
    if (nullptr == m_sheet)
        return nullptr;

    QPointF point = clickPoint;
    BrowserPage *page{nullptr};

    page = mouseClickInPage(point);

    if (nullptr == page)
        return nullptr;

    if (drawRect && m_lastClickPage)
        m_lastClickPage->setSelectIconRect(false);

    m_lastClickPage = page;

    point = translate2Local(point);

    foreach (Annotation *annot, page->annotations()) {
        foreach (QRectF rect, annot->boundary()) {
            if (rect.contains(point)) {
                if (drawRect)
                    page->setSelectIconRect(true, annot);
                return annot;
            }
        }
    }

    return nullptr;
}

Annotation *SheetBrowser::addHighLightAnnotation(const QString contains, const QColor color)
{
    BrowserPage *startPage{nullptr};
    BrowserPage *endPage{nullptr};
    Annotation *highLightAnnot{nullptr};

    startPage = mouseClickInPage(m_selectStartPos);
    endPage = mouseClickInPage(m_selectEndPos);

    if (nullptr == startPage || nullptr == endPage)
        return nullptr;

    m_selectEndPos = QPointF();
    m_selectStartPos = QPointF();

    if (startPage == endPage) {
        highLightAnnot = startPage->addHighlightAnnotation(contains, color);
    } else {
        if (startPage != endPage && endPage->itemIndex() < startPage->itemIndex()) {
            BrowserPage *tmpPage = endPage;
            endPage = startPage;
            startPage = tmpPage;
            tmpPage = nullptr;
        }

        int startIndex = m_items.indexOf(startPage);
        int endIndex = m_items.indexOf(endPage);

        for (int index = startIndex; index < endIndex; index++) {
            if (m_items.at(index))
                emit sigAddHighLightAnnot(m_items.at(index), contains, color);
        }

        highLightAnnot = endPage->addHighlightAnnotation(contains, color);

        for (int index = startIndex; index < endIndex; index++) {
            if (m_items.at(index))
                emit sigAddHighLightAnnot(m_items.at(index), contains, color);
        }
    }

    if (highLightAnnot) {
        emit sigOperaAnnotation(MSG_NOTE_ADD, highLightAnnot);
    }

    return highLightAnnot;
}

void SheetBrowser::jump2PagePos(BrowserPage *jumpPage, const qreal posLeft, const qreal posTop)
{
    if (nullptr == jumpPage)
        return;

    Dr::Rotation rotation{Dr::RotateBy0};
    SheetOperation  operation = m_sheet->operation();
    rotation = operation.rotation;

    int linkX{0};
    int linkY{0};

    //各自留0.05的余量,为了能更好的浏览效果
    switch (rotation) {
    case Dr::RotateBy0: {
        linkY = static_cast<int>((posTop - 0.05) * jumpPage->boundingRect().height() + jumpPage->pos().y());
        linkX = static_cast<int>((posLeft - 0.05) * jumpPage->boundingRect().width() + jumpPage->pos().x());
    }
    break;
    case Dr::RotateBy90: {
        linkY = static_cast<int>((posLeft - 0.05) * jumpPage->boundingRect().height() + jumpPage->pos().y());
        linkX = static_cast<int>((1.0 - posTop - 0.05) * jumpPage->boundingRect().width() + jumpPage->pos().x());
    }
    break;
    case Dr::RotateBy180: {
        linkY = static_cast<int>((1.0 - posTop - 0.05) * jumpPage->boundingRect().height() + jumpPage->pos().y());
        linkX = static_cast<int>((1.0 - posLeft - 0.05) * jumpPage->boundingRect().width() + jumpPage->pos().x());
    }
    break;
    case Dr::RotateBy270: {
        linkY = static_cast<int>((1.0 - posLeft - 0.05) * jumpPage->boundingRect().height() + jumpPage->pos().y());
        linkX = static_cast<int>((posTop - 0.05) * jumpPage->boundingRect().width() + jumpPage->pos().x());
    }
    break;
    default: break;
    }

    if (this->verticalScrollBar()) {
        linkY = (linkY > this->verticalScrollBar()->maximum() ? this->verticalScrollBar()->maximum() : linkY);
        this->verticalScrollBar()->setValue(linkY);
    }
    if (this->horizontalScrollBar()) {
        linkX = (linkX > this->horizontalScrollBar()->maximum() ? this->horizontalScrollBar()->maximum() : linkX);
        this->horizontalScrollBar()->setValue(linkX);
    }

}

/**
 * @brief SheetBrowser::addNewIconAnnotDeleteOld
 * 鼠标移动图标注释,删除之前的,在新位置添加一个新的
 * @param point
 */
void SheetBrowser::addNewIconAnnotDeleteOld(const QPointF clickPoint)
{
    if (nullptr == m_lastClickPage)
        return;

    QPointF pointf = clickPoint;
    QRectF iconRect;
    BrowserPage *nowPage{nullptr};
    QString  containt{""};

    nowPage = mouseClickInPage(pointf);

    if (nullptr == nowPage || m_lastClickPage != nowPage)
        return;

    iconRect.setWidth(nowPage->boundingRect().width());
    iconRect.setHeight(nowPage->boundingRect().height());

    bool isVaild = calcIconAnnotRect(nowPage, clickPoint, iconRect);

    if (isVaild) {
        nowPage->moveIconAnnotation(iconRect);
    }
}

bool SheetBrowser::mouseClickIconAnnot(QPointF &clickPoint)
{
    if (nullptr == m_document)
        return false;

    BrowserPage *item = mouseClickInPage(clickPoint);

    if (item) {
        return item->mouseClickIconAnnot(clickPoint);
    }

    return false;
}

QString SheetBrowser::selectedWordsText()
{
    QString text;
    foreach (BrowserPage *item, m_items)
        text += item->selectedWords();

    return text;
}

void SheetBrowser::handleVerticalScrollLater()
{
    foreach (BrowserPage *item, m_items) {
        if (!item->isVisible())
            item->clearWords();
    }

    if (nullptr == m_scrollTimer) {
        m_scrollTimer = new QTimer(this);
        connect(m_scrollTimer, &QTimer::timeout, this, &SheetBrowser::onSceneOfViewportChanged);
        m_scrollTimer->setSingleShot(true);
    }

    if (m_scrollTimer->isActive())
        m_scrollTimer->stop();

    m_scrollTimer->start(100);
}

QList<deepin_reader::Annotation *> SheetBrowser::annotations()
{
    QList<deepin_reader::Annotation *> list;
    foreach (BrowserPage *item, m_items) {
        item->loadAnnotations();
        list.append(item->annotations());
    }

    return list;
}

bool SheetBrowser::removeAnnotation(deepin_reader::Annotation *annotation)
{
    bool ret = false;
    foreach (BrowserPage *item, m_items) {
        if (item->hasAnnotation(annotation)) {
            ret = item->removeAnnotation(annotation);
            break;
        }
    }

    if (ret)
        emit sigOperaAnnotation(MSG_NOTE_DELETE, annotation);

    return ret;
}

bool SheetBrowser::removeAllAnnotation()
{
    foreach (BrowserPage *item, m_items) {
        if (item)
            item->removeAllAnnotation();
    }

    emit sigOperaAnnotation(MSG_ALL_NOTE_DELETE, nullptr);

    return true;
}

bool SheetBrowser::updateAnnotation(deepin_reader::Annotation *annotation, const QString &text, QColor color)
{
    if (nullptr == m_document || nullptr == annotation)
        return false;

    bool ret{false};

    foreach (BrowserPage *item, m_items) {
        if (item && item->hasAnnotation(annotation)) {
            ret = item->updateAnnotation(annotation, text, color);
        }
    }

    if (ret) {
        if (!text.isEmpty())
            emit sigOperaAnnotation(MSG_NOTE_ADD, annotation);
        else
            emit sigOperaAnnotation(MSG_NOTE_DELETE, annotation);
    }

    return ret;
}

void SheetBrowser::onRemoveAnnotation(deepin_reader::Annotation *annotation)
{
    m_sheet->removeAnnotation(annotation);
}

void SheetBrowser::onUpdateAnnotation(deepin_reader::Annotation *annotation, const QString &text)
{
    updateAnnotation(annotation, text);
}

deepin_reader::Outline SheetBrowser::outline()
{
    return m_document->outline();
}

Properties SheetBrowser::properties() const
{
    return m_document->properties();
}

void SheetBrowser::jumpToOutline(const qreal &linkLeft, const qreal &linkTop, unsigned int index)
{
    int pageIndex = static_cast<int>(index);

    if (nullptr == m_sheet || pageIndex < 0 || pageIndex >= m_items.count() || linkLeft < 0 || linkTop < 0)
        return;

    BrowserPage *jumpPage = m_items.at(pageIndex);
    if (nullptr == jumpPage)
        return;

    Dr::Rotation rotation{Dr::RotateBy0};
    SheetOperation  operation = m_sheet->operation();
    rotation = operation.rotation;

    if (rotation != Dr::NumberOfRotations && rotation != Dr::RotateBy0) {
        setCurrentPage(++pageIndex);
        return;
    }

    jump2PagePos(jumpPage, linkLeft, linkTop);
}

void SheetBrowser::jumpToHighLight(deepin_reader::Annotation *annotation, const int index)
{
    if (nullptr == m_sheet || nullptr == annotation || (index < 0 || index >= m_items.count()))
        return;

    BrowserPage *jumpPage = m_items.at(index);
    QList<QRectF> anootList = annotation->boundary();

    if (nullptr == jumpPage || anootList.count() < 1)
        return;

    QRectF firstRect = anootList.at(0);
    if (firstRect.isNull() || firstRect.isEmpty())
        return;

    jump2PagePos(jumpPage, firstRect.x(), firstRect.y());

    //画选中边框(仅图标注释)
    if (m_lastClickPage)
        m_lastClickPage->setSelectIconRect(false);
    m_lastClickPage = jumpPage;
    m_lastClickPage->setSelectIconRect(true, annotation);
}

BrowserPage *SheetBrowser::mouseClickInPage(QPointF &point)
{
    foreach (BrowserPage *page, m_items) {
        if (nullptr != page) {
            if (point.x() > page->pos().x() && point.y() > page->pos().y() &&
                    point.x() < (page->pos().x() + page->boundingRect().width()) &&
                    point.y() < (page->pos().y() + page->boundingRect().height())) {
                return  page;
            }
        }
    }

    return nullptr;
}

void SheetBrowser::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
        if (nullptr == m_sheet)
            return;

        if (event->delta() > 0) {
            m_sheet->zoomin();
        } else {
            m_sheet->zoomout();
        }

        return;
    }

    QGraphicsView::wheelEvent(event);
}

void SheetBrowser::deform(SheetOperation &operation)
{
    m_lastScaleFactor = operation.scaleFactor;

    switch (operation.rotation) {
    default:
    case Dr::RotateBy0:
    case Dr::RotateBy180:
        if (Dr::FitToPageWidthMode == operation.scaleMode)
            operation.scaleFactor = static_cast<double>(this->width() - 25.0) / static_cast<double>(m_maxWidth) / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
        else if (Dr::FitToPageHeightMode == operation.scaleMode)
            operation.scaleFactor = static_cast<double>(this->height()) / static_cast<double>(m_maxHeight);
        else if (Dr::FitToPageDefaultMode == operation.scaleMode)
            operation.scaleFactor = 1.0 ;
        else if (Dr::FitToPageWorHMode == operation.scaleMode) {
            qreal scaleFactor = static_cast<double>(this->width() - 25.0) / static_cast<double>(m_maxWidth) / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
            if (scaleFactor * m_maxHeight > this->height())
                scaleFactor = static_cast<double>(this->height()) / static_cast<double>(m_maxHeight);
            operation.scaleFactor = scaleFactor;
        } else
            operation.scaleMode = Dr::ScaleFactorMode;
        break;
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        if (Dr::FitToPageWidthMode == operation.scaleMode)
            operation.scaleFactor = static_cast<double>(this->width() - 25.0) / static_cast<double>(m_maxHeight) / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
        else if (Dr::FitToPageHeightMode == operation.scaleMode)
            operation.scaleFactor = static_cast<double>(this->height()) / static_cast<double>(m_maxWidth);
        else if (Dr::FitToPageDefaultMode == operation.scaleMode)
            operation.scaleFactor = 1.0 ;
        else if (Dr::FitToPageWorHMode == operation.scaleMode) {
            qreal scaleFactor = static_cast<double>(this->width() - 25.0) / static_cast<double>(m_maxHeight) / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
            if (scaleFactor * m_maxWidth > this->height())
                scaleFactor = static_cast<double>(this->height()) / static_cast<double>(m_maxWidth);
            operation.scaleFactor = scaleFactor;
        } else
            operation.scaleMode = Dr::ScaleFactorMode;
        break;
    }

    int page = operation.currentPage;
    int diffY = 0;
    int diffX = 0;

    if (page > 0 && page <= m_items.count()) {
        diffY = qRound((verticalScrollBar()->value() - m_items.at(page - 1)->pos().y()) * operation.scaleFactor * 1.0 / m_lastScaleFactor);
        diffX = qRound((horizontalScrollBar()->value() - m_items.at(page - 1)->pos().x()) * operation.scaleFactor * 1.0 / m_lastScaleFactor);
    }


    for (int i = 0; i < m_items.count(); ++i) {
        m_items.at(i)->render(operation.scaleFactor, operation.rotation, true);
    }

    int width = 0;
    int height = 0;

    if (Dr::SinglePageMode == operation.layoutMode) {
        for (int i = 0; i < m_items.count(); ++i) {
            m_items.at(i)->setPos(0, height);

            height += m_items.at(i)->boundingRect().height() + 5;

            if (m_items.at(i)->boundingRect().width() > width)
                width = static_cast<int>(m_items.at(i)->boundingRect().width());
        }
    } else if (Dr::TwoPagesMode == operation.layoutMode) {
        for (int i = 0; i < m_items.count(); ++i) {
            if (i % 2 == 1)
                continue;

            if (m_items.count() > i + 1) {
                m_items.at(i)->setPos(0, height);
                m_items.at(i + 1)->setPos(m_items.at(i)->boundingRect().width() + 5, height);
            } else {
                m_items.at(i)->setPos(0, height);
            }

            if (m_items.count() > i + 1) {
                height +=  qMax(m_items.at(i)->boundingRect().height(), m_items.at(i + 1)->boundingRect().height()) + 5;
            } else
                height += m_items.at(i)->boundingRect().height() + 5;

            if (m_items.count() > i + 1) {
                if (m_items.at(i)->boundingRect().width() + m_items.at(i + 1)->boundingRect().width() > width)
                    width = static_cast<int>(m_items.at(i)->boundingRect().width() + m_items.at(i + 1)->boundingRect().width());
            } else {
                if (m_items.at(i)->boundingRect().width() > width)
                    width = static_cast<int>(m_items.at(i)->boundingRect().width());
            }
        }
    }

    setSceneRect(0, 0, width, height);

    if (page > 0 && page <= m_items.count()) {
        verticalScrollBar()->setValue(static_cast<int>(m_items[page - 1]->pos().y() + diffY));
        horizontalScrollBar()->setValue(static_cast<int>(m_items[page - 1]->pos().x() + diffX));
    }

    handleVerticalScrollLater();
}

bool SheetBrowser::hasLoaded()
{
    return m_hasLoaded;
}

void SheetBrowser::resizeEvent(QResizeEvent *event)
{
    foreach (BrowserPage *item, m_items) {
        item->clearWords();
    }

    if (hasLoaded() && m_sheet->operation().scaleMode != Dr::ScaleFactorMode) {
        deform(m_sheet->operationRef());
        m_sheet->setOperationChanged();
    }

    QGraphicsView::resizeEvent(event);

    if (nullptr == m_resizeTimer) {
        m_resizeTimer = new QTimer(this);
        connect(m_resizeTimer, &QTimer::timeout, this, &SheetBrowser::onSceneOfViewportChanged);
        m_resizeTimer->setSingleShot(true);
    }

    if (m_resizeTimer->isActive())
        m_resizeTimer->stop();

    m_resizeTimer->start(100);
    if (m_pFindWidget) m_pFindWidget->showPosition(this->width());
}

void SheetBrowser::mousePressEvent(QMouseEvent *event)
{
    if (QGraphicsView::NoDrag == dragMode() || QGraphicsView::RubberBandDrag == dragMode()) {
        Qt::MouseButton btn = event->button();
        if (btn == Qt::LeftButton) {
            scene()->setSelectionArea(QPainterPath());
            m_selectEndPos = QPointF();

            m_selectStartPos = m_selectPressedPos = mapToScene(event->pos());

            deepin_reader::Annotation *clickAnno = nullptr;

            //使用此方法,为了处理所有旋转角度的情况(0,90,180,270)
            clickAnno = getClickAnnot(m_selectPressedPos, true);
            if (clickAnno && clickAnno->type() == 1) {
                m_selectIconAnnotation = true;
                m_iconAnnotationMovePos = m_selectPressedPos;
                m_annotationInserting = false;
                return DGraphicsView::mousePressEvent(event);
            }
        } else if (btn == Qt::RightButton) {
            closeMagnifier();
            m_selectPressedPos = QPointF();

            BrowserPage *item = nullptr;

            BrowserAnnotation *annotation = nullptr;

            QList<QGraphicsItem *>  list = scene()->items(mapToScene(event->pos()));

            const QString &selectWords = selectedWordsText();

            foreach (QGraphicsItem *baseItem, list) {
                if (nullptr != dynamic_cast<BrowserAnnotation *>(baseItem)) {
                    annotation = dynamic_cast<BrowserAnnotation *>(baseItem);
                }

                if (nullptr != dynamic_cast<BrowserPage *>(baseItem)) {
                    item = dynamic_cast<BrowserPage *>(baseItem);
                }
            }

            m_tipsWidget->hide();
            BrowserMenu menu;
            connect(&menu, &BrowserMenu::signalMenuItemClicked, [ & ](const QString & objectname) {
                const QPointF &clickPos = mapToScene(event->pos());
                if (objectname == "Copy") {
                    Utils::copyText(selectWords);
                } else if (objectname == "CopyAnnoText") {
                    if (annotation)
                        Utils::copyText(annotation->annotationText());
                } else if (objectname == "AddTextHighlight") {
                    QColor color = menu.getColor();
                    addHighLightAnnotation("", color);
                } else if (objectname == "ChangeAnnotationColor") {
                    if (annotation) {
                        QColor color = menu.getColor();
                        updateAnnotation(annotation->annotation(), annotation->annotationText(), color);
                    }
                } else if (objectname == "RemoveAnnotation") {
                    if (annotation)
                        m_sheet->removeAnnotation(annotation->annotation());
                } else if (objectname == "AddAnnotationIcon") {
                    if (annotation)  {
                        updateAnnotation(annotation->annotation(), annotation->annotationText(), QColor());
                        showNoteEditWidget(annotation->annotation());
                    } else {
                        showNoteEditWidget(addIconAnnotation(clickPos, ""));
                    }
                    m_noteEditWidget->move(mapToGlobal(event->pos()) - QPoint(12, 12));
                } else if (objectname == "AddBookmark") {
                    m_sheet->setBookMark(item->itemIndex(), true);
                } else if (objectname == "RemoveHighlight") {
                    if (annotation)
                        m_sheet->removeAnnotation(annotation->annotation(), !annotation->annotationText().isEmpty());
                } else if (objectname == "AddAnnotationHighlight") {
                    QColor color = menu.getColor();
                    if (annotation)  {
                        updateAnnotation(annotation->annotation(), annotation->annotationText(), color);
                        showNoteEditWidget(annotation->annotation());
                    } else {
                        showNoteEditWidget(addHighLightAnnotation("", color));
                    }
                    m_noteEditWidget->move(mapToGlobal(event->pos()) - QPoint(12, 12));
                } else if (objectname == "Search") {
                    m_sheet->handleSearch();
                } else if (objectname == "RemoveBookmark") {
                    m_sheet->setBookMark(item->itemIndex(), false);
                } else if (objectname == "Fullscreen") {
                    m_sheet->openFullScreen();
                } else if (objectname == "ExitFullscreen") {
                    m_sheet->closeFullScreen();
                } else if (objectname == "SlideShow") {
                    m_sheet->openSlide();
                } else if (objectname == "FirstPage") {
                    this->emit sigNeedPageFirst();
                } else if (objectname == "PreviousPage") {
                    this->emit sigNeedPagePrev();
                } else if (objectname == "NextPage") {
                    this->emit sigNeedPageNext();
                } else if (objectname == "LastPage") {
                    this->emit sigNeedPageLast();
                } else if (objectname == "RotateLeft") {
                    m_sheet->rotateLeft();
                } else if (objectname == "RotateRight") {
                    m_sheet->rotateRight();
                } else if (objectname == "Print") {
                    PrintManager p(m_sheet);
                    p.showPrintDialog(m_sheet);
                } else if (objectname == "DocumentInfo") {
                    FileAttrWidget *pFileAttrWidget = new FileAttrWidget;
                    pFileAttrWidget->setFileAttr(m_sheet);
                    pFileAttrWidget->showScreenCenter();
                }
            });

            if (nullptr != annotation && annotation->annotationType() == deepin_reader::Annotation::AnnotationText) {
                //文字注释(图标)
                menu.initActions(m_sheet, item->itemIndex(), SheetMenuType_e::DOC_MENU_ANNO_ICON);
            } else if (nullptr != annotation && annotation->annotationType() == deepin_reader::Annotation::AnnotationHighlight) {
                //文字高亮注释
                menu.initActions(m_sheet, item->itemIndex(), SheetMenuType_e::DOC_MENU_ANNO_HIGHLIGHT);
            }  else if (!selectWords.isEmpty()) {
                //选择文字
                menu.initActions(m_sheet, item->itemIndex(), SheetMenuType_e::DOC_MENU_SELECT_TEXT);
            } else if (nullptr != item) {
                //默认
                menu.initActions(m_sheet, item->itemIndex(), SheetMenuType_e::DOC_MENU_DEFAULT);
            }
            menu.exec(mapToGlobal(event->pos()));
        }
    }

    DGraphicsView::mousePressEvent(event);
}

void SheetBrowser::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();

    if (m_selectIconAnnotation && m_lastClickPage) {
        m_iconAnnotationMovePos = mapToScene(mousePos);
        m_lastClickPage->setDrawMoveIconRect(true);
        m_lastClickPage->setIconMovePos(QPoint(static_cast<int>(m_iconAnnotationMovePos.x() - m_lastClickPage->x()),
                                               static_cast<int>(m_iconAnnotationMovePos.y() - m_lastClickPage->y())));
        setCursor(QCursor(Qt::PointingHandCursor));
        return;
    }

    if (m_sheet->isFullScreen()) {
        if (mousePos.x() == 0 && !m_sheet->sideBarVisible()) {
            m_sheet->setSidebarVisible(true, false);
        } else if (!m_sheet->operation().sidebarVisible && m_sheet->sideBarVisible()) {
            m_sheet->setSidebarVisible(false, false);
        }
    }

    if (m_magnifierLabel) {
        QPoint magnifierPos = mousePos;
        if (mousePos.y() < 122) {
            verticalScrollBar()->setValue(verticalScrollBar()->value() - (122 - mousePos.y()));
            magnifierPos.setY(122);
        }

        if (mousePos.x() < 122) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (122 - mousePos.x()));
            magnifierPos.setX(122);
        }

        if (mousePos.y() > (this->size().height() - 122) && (this->size().height() - 122 > 0)) {
            verticalScrollBar()->setValue(verticalScrollBar()->value() + (mousePos.y() - (this->size().height() - 122)));
            magnifierPos.setY(this->size().height() - 122);
        }

        if (mousePos.x() > (this->size().width() - 122) && (this->size().width() - 122 > 0)) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + (mousePos.x() - (this->size().width() - 122)));
            magnifierPos.setX(this->size().width() - 122);
        }

        m_magnifierLabel->showMagnigierImage(mousePos, magnifierPos, m_lastScaleFactor);
    } else {
        if (m_selectPressedPos.isNull()) {
            BrowserPage *page = getBrowserPageForPoint(mousePos);
            if (page) {
                BrowserAnnotation *browserAnno = page->getBrowserAnnotation(mousePos);
                if (browserAnno && !browserAnno->annotationText().isEmpty()) {
                    m_tipsWidget->setText(browserAnno->annotationText());
                    QPoint showRealPos(QCursor::pos().x(), QCursor::pos().y() + 20);
                    m_tipsWidget->move(showRealPos);
                    m_tipsWidget->show();
                    setCursor(QCursor(Qt::PointingHandCursor));
                } else if (page->getBrowserWord(mousePos)) {
                    m_tipsWidget->hide();
                    setCursor(QCursor(Qt::IBeamCursor));
                } else {
                    m_tipsWidget->hide();
                    setCursor(QCursor(Qt::ArrowCursor));
                }
            } else {
                setCursor(QCursor(Qt::ArrowCursor));
            }
        } else {
            m_tipsWidget->hide();
            QPointF beginPos = m_selectPressedPos;
            QPointF endPos = mapToScene(event->pos());

            //开始的word
            QList<QGraphicsItem *> beginItemList = scene()->items(beginPos);
            BrowserWord *beginWord = nullptr;
            foreach (QGraphicsItem *item, beginItemList) {
                if (!item->isPanel()) {
                    beginWord = qgraphicsitem_cast<BrowserWord *>(item);
                    break;
                }
            }

            //结束的word
            QList<QGraphicsItem *> endItemList = scene()->items(endPos);
            BrowserWord *endWord = nullptr;
            foreach (QGraphicsItem *item, endItemList) {
                if (!item->isPanel()) {
                    endWord = qgraphicsitem_cast<BrowserWord *>(item);
                    break;
                }
            }

            QList<QGraphicsItem *> words;               //应该只存这几页的words 暂时等于所有的
            words = scene()->items(sceneRect());        //倒序

            if (endWord == nullptr) {
                if (endPos.y() > beginPos.y()) {
                    for (int i = 0; i < words.size(); ++i) {//从后向前检索
                        if (words[i]->mapToScene(QPointF(words[i]->boundingRect().x(), words[i]->boundingRect().y())).y() < endPos.y()) {
                            endWord = qgraphicsitem_cast<BrowserWord *>(words[i]);
                            break;
                        }
                    }
                } else {
                    for (int i = words.size() - 1; i >= 0; i--) {
                        if (words[i]->mapToScene(QPointF(words[i]->boundingRect().x(), words[i]->boundingRect().y())).y() > endPos.y()) {
                            endWord = qgraphicsitem_cast<BrowserWord *>(words[i]);
                            break;
                        }
                    }
                }
            }

            //先考虑字到字的
            if (beginWord != nullptr && endWord != nullptr && beginWord != endWord) {
                scene()->setSelectionArea(QPainterPath());
                bool between = false;
                for (int i = words.size() - 1; i >= 0; i--) {
                    if (words[i]->isPanel())
                        continue;

                    if (qgraphicsitem_cast<BrowserWord *>(words[i]) == beginWord || qgraphicsitem_cast<BrowserWord *>(words[i]) == endWord) {
                        if (between) {
                            words[i]->setSelected(true);
                            break;
                        } else
                            between = true;
                    }

                    if (between)
                        words[i]->setSelected(true);
                }
            }
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void SheetBrowser::mouseReleaseEvent(QMouseEvent *event)
{
    Qt::MouseButton btn = event->button();
    if (btn == Qt::LeftButton) {
        const QPointF &mousepoint = mapToScene(event->pos());
        m_selectEndPos = mousepoint;

        deepin_reader::Annotation *clickAnno = nullptr;
        //使用此方法,为了处理所有旋转角度的情况(0,90,180,270)
        clickAnno = getClickAnnot(mousepoint);

        if (m_annotationInserting) {
            if (clickAnno && clickAnno->type() == 1/*AText*/) {
                updateAnnotation(clickAnno, clickAnno->contents());
            } else {
                clickAnno = addIconAnnotation(mousepoint, "");
            }
            showNoteEditWidget(clickAnno);
            m_annotationInserting = false;
        } else {
            if (nullptr != clickAnno)
                showNoteEditWidget(clickAnno);
        }
        m_selectEndPos = mapToScene(event->pos());

        if (m_selectIconAnnotation && m_lastClickPage && (m_selectPressedPos != m_selectEndPos)) {
            m_lastClickPage->setDrawMoveIconRect(false);
            addNewIconAnnotDeleteOld(m_selectEndPos);
        } else if (m_selectIconAnnotation && m_lastClickPage && (m_selectPressedPos == m_selectEndPos)) {
            showNoteEditWidget(clickAnno);
        }

        m_selectPressedPos = QPointF();
        m_selectIconAnnotation = false;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

int SheetBrowser::allPages()
{
    return m_items.count();
}

int SheetBrowser::currentPage()
{
    int value = verticalScrollBar()->value();

    int index = 0;

    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items[i]->pos().y() + m_items[i]->boundingRect().height() >= value) {
            index = i;
            break;
        }
    }

    return index + 1;
}

int SheetBrowser::viewPointInIndex(QPoint viewPoint)
{
    BrowserPage *item  = static_cast<BrowserPage *>(itemAt(viewPoint));

    return m_items.indexOf(item);
}

void SheetBrowser::setCurrentPage(int page)
{
    if (page < 1 && page > allPages())
        return;

    verticalScrollBar()->setValue(static_cast<int>(m_items.at(page - 1)->pos().y()));
}

bool SheetBrowser::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode, bool bSrc)
{
    if (m_items.count() <= index)
        return false;

    image = m_items.at(index)->getImage(static_cast<int>(width), static_cast<int>(height), mode, bSrc);

    return true;
}

BrowserPage *SheetBrowser::getBrowserPageForPoint(QPoint &viewPoint)
{
    BrowserPage *item = nullptr;
    const QList<QGraphicsItem *> &itemlst = this->items(viewPoint);
    for (QGraphicsItem *itemIter : itemlst) {
        item = dynamic_cast<BrowserPage *>(itemIter);
        if (item != nullptr) {
            break;
        }
    }

    if (nullptr == item)
        return nullptr;

    const QPointF &itemPoint = item->mapFromScene(mapToScene(viewPoint));

    if (item->contains(itemPoint)) {
        viewPoint = itemPoint.toPoint();
        return item;
    }

    return nullptr;
}

Annotation *SheetBrowser::addIconAnnotation(const QPointF clickPoint, const QString contents)
{
    BrowserPage *page{nullptr};
    QPointF pointf = clickPoint;
    Annotation *anno = nullptr;
    QRectF iconRect;

    page = mouseClickInPage(pointf);

    if (nullptr != page) {
        iconRect.setWidth(page->boundingRect().width());
        iconRect.setHeight(page->boundingRect().height());

        bool isVaild = calcIconAnnotRect(page, clickPoint, iconRect);

        if (isVaild)
            anno = page->addIconAnnotation(iconRect, contents);
    }

    if (anno && !contents.isEmpty()) {
        emit sigOperaAnnotation(MSG_NOTE_ADD, anno);
    }
    return anno;
}

void SheetBrowser::openMagnifier()
{
    if (nullptr == m_magnifierLabel) {
        m_magnifierLabel = new BrowserMagniFier(this);
        setDragMode(QGraphicsView::RubberBandDrag);
        setMouseTracking(true);
        setCursor(QCursor(Qt::BlankCursor));
    }
}

void SheetBrowser::closeMagnifier()
{
    if (nullptr != m_magnifierLabel) {
        m_magnifierLabel->hide();
        m_magnifierLabel->deleteLater();
        m_magnifierLabel = nullptr;

        setMouseTracking(false);
        setCursor(QCursor(Qt::ArrowCursor));
        if (Dr::MouseShapeHand == m_sheet->operation().mouseShape) {
            setDragMode(QGraphicsView::ScrollHandDrag);
        } else if (Dr::MouseShapeNormal == m_sheet->operation().mouseShape) {
            setDragMode(QGraphicsView::RubberBandDrag);
        }
    }
}

bool SheetBrowser::magnifierOpened()
{
    return (nullptr != m_magnifierLabel) ;
}

int SheetBrowser::maxWidth()
{
    return m_maxWidth;
}

int SheetBrowser::maxHeight()
{
    return m_maxHeight;
}

void SheetBrowser::needBookmark(int index, bool state)
{
    emit sigNeedBookMark(index, state);
}

void SheetBrowser::dragEnterEvent(QDragEnterEvent *event)
{
    event->ignore();
}

void SheetBrowser::showEvent(QShowEvent *event)
{
    if (1 != m_initPage) {
        setCurrentPage(m_initPage);
        m_initPage = 1;
    }

    QGraphicsView::showEvent(event);
}

void SheetBrowser::handleSearch()
{
    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(this);
        m_pFindWidget->setDocSheet(m_sheet);
    }

    m_pFindWidget->showPosition(this->width());
    m_pFindWidget->setSearchEditFocus();
}

void SheetBrowser::stopSearch()
{
    if (!m_pFindWidget)
        return;

    m_pFindWidget->stopSearch();
}

void SheetBrowser::handleFindNext()
{
    int size = m_items.size();
    for (int index = 0; index < size; index++) {
        int curIndex = m_searchCurIndex % size;
        BrowserPage *page = m_items.at(curIndex);
        int pageHighlightSize = page->searchHighlightRectSize();
        if (pageHighlightSize > 0) {
            if (m_lastFindPage && m_lastFindPage != page && m_changSearchFlag) {
                m_changSearchFlag = false;
                m_searchPageTextIndex = -1;
                m_lastFindPage->clearSelectSearchHighlightRects();
            }

            m_searchPageTextIndex++;
            if (m_searchPageTextIndex >= pageHighlightSize) {
                //当前页搜索完了，可以进行下一页
                m_changSearchFlag = true;
                m_lastFindPage = page;
                m_searchCurIndex = curIndex + 1;

                if (m_searchCurIndex >= size)
                    m_searchCurIndex = 0;
                continue;
            }

            const QRectF &pageHigRect = page->translateRect(page->findSearchforIndex(m_searchPageTextIndex));
            horizontalScrollBar()->setValue(static_cast<int>(page->pos().x() + pageHigRect.x()) - this->width() / 2);
            verticalScrollBar()->setValue(static_cast<int>(page->pos().y() + pageHigRect.y()) - this->height() / 2);
            break;
        } else {
            m_searchCurIndex++;
            if (m_searchCurIndex >= size)
                m_searchCurIndex = 0;
        }
    }
}

void SheetBrowser::handleFindPrev()
{
    int size = m_items.size();
    for (int index = 0; index < size; index++) {
        int curIndex = m_searchCurIndex % size;
        BrowserPage *page = m_items.at(curIndex);
        int pageHighlightSize = page->searchHighlightRectSize();
        if (pageHighlightSize > 0) {
            if (m_lastFindPage && m_lastFindPage != page && m_changSearchFlag) {
                m_changSearchFlag = false;
                m_searchPageTextIndex = pageHighlightSize;
                m_lastFindPage->clearSelectSearchHighlightRects();
            }

            m_searchPageTextIndex--;
            if (m_searchPageTextIndex < 0) {
                //当前页搜索完了，可以进行下一页
                m_changSearchFlag = true;
                m_lastFindPage = page;
                m_searchCurIndex = curIndex - 1;

                if (m_searchCurIndex < 0)
                    m_searchCurIndex = size - 1;
                continue;
            }

            const QRectF &pageHigRect = page->translateRect(page->findSearchforIndex(m_searchPageTextIndex));
            horizontalScrollBar()->setValue(static_cast<int>(page->pos().x() + pageHigRect.x()) - this->width() / 2);
            verticalScrollBar()->setValue(static_cast<int>(page->pos().y() + pageHigRect.y()) - this->height() / 2);
            break;
        } else {
            m_searchCurIndex--;
            if (m_searchCurIndex < 0)
                m_searchCurIndex = size - 1;
        }
    }
}

void SheetBrowser::handleFindExit()
{
    m_searchCurIndex = 0;
    m_searchPageTextIndex = 0;
    m_searchTask->stopSearch();
    for (BrowserPage *page : m_items)
        page->clearSearchHighlightRects();
}

void SheetBrowser::handleFindContent(const QString &strFind)
{
    m_searchCurIndex = m_sheet->currentIndex();
    m_searchPageTextIndex = 0;
    m_searchTask->startSearch(m_items, strFind, m_searchCurIndex);
}

void SheetBrowser::handleFindFinished(int searchcnt)
{
    m_pFindWidget->setEditAlert(searchcnt == 0);
}
