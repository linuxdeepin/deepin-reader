// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BrowserPage.h"
#include "Model.h"
#include "PageRenderThread.h"
#include "SheetBrowser.h"
#include "BrowserWord.h"
#include "BrowserAnnotation.h"
#include "Application.h"
#include "Utils.h"
#include "Global.h"
#include "SheetRenderer.h"
#include "ddlog.h"

#include <DGuiApplicationHelper>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QTime>
#include <QMutexLocker>
#include <QTimer>
#include <QUuid>
#include <QDesktopServices>
#include <QDebug>
#include <QMutexLocker>
#include <QDebug>

const int ICON_SIZE = 23;

BrowserPage::BrowserPage(SheetBrowser *parent, int index, DocSheet *sheet) :
    QGraphicsItem(), m_sheet(sheet), m_parent(parent), m_index(index)
{
    qCDebug(appLog) << "BrowserPage created, index:" << index;
    setAcceptHoverEvents(true);

    setFlag(QGraphicsItem::ItemIsPanel);

    m_originSizeF = sheet->renderer()->getPageSize(index);
    qCDebug(appLog) << "BrowserPage::BrowserPage() - Constructor completed";
}

BrowserPage::~BrowserPage()
{
    // qCDebug(appLog) << "BrowserPage destroyed, index:" << m_index;
    PageRenderThread::clearImageTasks(m_sheet, this);

    qDeleteAll(m_annotations);

    qDeleteAll(m_annotationItems);

    qDeleteAll(m_words);
    // qCDebug(appLog) << "BrowserPage::~BrowserPage() - Destructor completed";
}

QRectF BrowserPage::boundingRect() const
{
    // qCDebug(appLog) << "BrowserPage::boundingRect() - Calculating bounding rectangle";
    QRectF rect = QRectF(0, 0, m_originSizeF.width() * m_scaleFactor, m_originSizeF.height() * m_scaleFactor);
    // qCDebug(appLog) << "BrowserPage::boundingRect() - Bounding rectangle:" << rect;
    return rect;
}

QRectF BrowserPage::rect()
{
    // qCDebug(appLog) << "BrowserPage::rect() - Calculating rect with rotation:" << m_rotation;
    switch (m_rotation) {
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        // qCDebug(appLog) << "BrowserPage::rect() - Using rotated dimensions (90/270 degrees)";
        return QRectF(0, 0, static_cast<double>(m_originSizeF.height() * m_scaleFactor), static_cast<double>(m_originSizeF.width() * m_scaleFactor));
    default: 
        // qCDebug(appLog) << "BrowserPage::rect() - Using normal dimensions (0/180 degrees)";
        break;
    }

    QRectF rect = QRectF(0, 0, static_cast<double>(m_originSizeF.width() * m_scaleFactor), static_cast<double>(m_originSizeF.height() * m_scaleFactor));
    // qCDebug(appLog) << "BrowserPage::rect() - Final rect:" << rect;
    return rect;
}

qreal BrowserPage::scaleFactor()
{
    // qCDebug(appLog) << "BrowserPage::scaleFactor() - Returning scale factor:" << m_scaleFactor;
    return m_scaleFactor;
}

QRectF BrowserPage::bookmarkRect()
{
    // qCDebug(appLog) << "BrowserPage::bookmarkRect() - Calculating bookmark rectangle";
    QRectF rect = QRectF(boundingRect().width() - 40, 1, 39, 39);
    // qCDebug(appLog) << "BrowserPage::bookmarkRect() - Bookmark rectangle:" << rect;
    return rect;
}

QRectF BrowserPage::bookmarkMouseRect()
{
    // qCDebug(appLog) << "BrowserPage::bookmarkMouseRect() - Calculating bookmark mouse rectangle";
    QRectF rect = QRectF(boundingRect().width() - 27, 10, 14, 20);
    // qCDebug(appLog) << "BrowserPage::bookmarkMouseRect() - Bookmark mouse rectangle:" << rect;
    return rect;
}

void BrowserPage::setBookmark(const bool &hasBookmark)
{
    qCDebug(appLog) << "BrowserPage::setBookmark:" << hasBookmark;
    m_bookmark = hasBookmark;

    if (hasBookmark) {
        qCDebug(appLog) << "BrowserPage::setBookmark() - Setting bookmark state to checked (3)";
        m_bookmarkState = 3;
    } else {
        qCDebug(appLog) << "BrowserPage::setBookmark() - Setting bookmark state to unchecked (0)";
        m_bookmarkState = 0;
    }

    update();
    qCDebug(appLog) << "BrowserPage::setBookmark() - Set bookmark completed";
}

void BrowserPage::updateBookmarkState()
{
    qCDebug(appLog) << "BrowserPage::updateBookmarkState() - Starting update bookmark state";
    if (m_bookmark) {
        qCDebug(appLog) << "BrowserPage::updateBookmarkState() - Setting bookmark state to checked (3)";
        m_bookmarkState = 3;
    } else {
        qCDebug(appLog) << "BrowserPage::updateBookmarkState() - Setting bookmark state to unchecked (0)";
        m_bookmarkState = 0;
    }

    update();
    qCDebug(appLog) << "BrowserPage::updateBookmarkState() - Update bookmark state completed";
}

void BrowserPage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option)
    // qCDebug(appLog) << "BrowserPage::paint() - Starting paint operation";

    if (!qFuzzyCompare(m_renderPixmapScaleFactor, m_scaleFactor)) {
        // qCDebug(appLog) << "BrowserPage::paint() - Scale factor changed, re-rendering";
        render(m_scaleFactor, m_rotation);
    }

    if (!m_viewportRendered && !m_pixmapHasRendered && isBigDoc()) {
        // qCDebug(appLog) << "BrowserPage::paint() - Rendering viewport for big document";
        renderViewPort();
    }

    painter->drawPixmap(0, 0, m_renderPixmap);  //m_renderPixmap的大小存在系统缩放，可能不等于option->rect()，需要按坐标绘制

    if (1 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_hover").pixmap(QSize(39, 39)));
    if (2 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_pressed").pixmap(QSize(39, 39)));
    if (3 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_checked").pixmap(QSize(39, 39)));

    painter->setPen(Qt::NoPen);

    painter->setBrush(QColor(238, 220, 0, 100));

    //search
    for (const PageSection &section : m_searchLightrectLst) {
        for (const PageLine &line : section) {
            painter->drawRect(getNorotateRect(line.rect));
        }
    }

    //search and Select
    painter->setBrush(QColor(59, 148, 1, 100));
    for (const PageLine &line : m_searchSelectLighRectf) {
        painter->drawRect(getNorotateRect(line.rect));
    }

    if (m_drawMoveIconRect) {
        QPen pen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
        painter->setPen(pen);

        qreal iconWidth = (nullptr != m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem)) ? (m_lastClickIconAnnotationItem->boundingRect().width()) : ICON_SIZE;
        qreal iconHeight = (nullptr != m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem)) ? (m_lastClickIconAnnotationItem->boundingRect().height()) : ICON_SIZE;

        int x = static_cast<int>(m_drawMoveIconPoint.x() - iconWidth / 2.0);
        int y = static_cast<int>(m_drawMoveIconPoint.y() - iconHeight / 2.0);

        QRect rect = QRect(x, y, static_cast<int>(iconHeight), static_cast<int>(iconHeight));

        painter->drawRect(rect);
    }
    // qCDebug(appLog) << "BrowserPage::paint() - Paint operation completed";
}

void BrowserPage::render(const double &scaleFactor, const Dr::Rotation &rotation, const bool &renderLater, const bool &force)
{
    // qCDebug(appLog) << "BrowserPage::render scale:" << scaleFactor << "rotation:" << rotation << "force:" << force;
    if (!force && renderLater && qFuzzyCompare(scaleFactor, m_scaleFactor) && rotation == m_rotation) {
        // qCDebug(appLog) << "BrowserPage::render() - No render needed, parameters unchanged";
        return;
    }

    if (m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem))
        m_lastClickIconAnnotationItem->setScaleFactor(scaleFactor);

    m_scaleFactor = scaleFactor;

    if (m_rotation != rotation) {
        // qCDebug(appLog) << "BrowserPage::render() - Rotation changed, updating rotation";
        m_rotation = rotation;
        if (Dr::RotateBy0 == m_rotation)
            this->setRotation(0);
        else if (Dr::RotateBy90 == m_rotation)
            this->setRotation(90);
        else if (Dr::RotateBy180 == m_rotation)
            this->setRotation(180);
        else if (Dr::RotateBy270 == m_rotation)
            this->setRotation(270);
    }

    if (!renderLater && !qFuzzyCompare(m_renderPixmapScaleFactor, m_scaleFactor)) {
        // qCDebug(appLog) << "BrowserPage::render() - Render pixmap scale factor changed, updating render pixmap scale factor";
        m_renderPixmapScaleFactor = m_scaleFactor;

        if (m_pixmap.isNull()) {
            // qCDebug(appLog) << "BrowserPage::render() - Pixmap is null, creating new pixmap";
            m_pixmap = QPixmap(static_cast<int>(boundingRect().width() * dApp->devicePixelRatio()),
                               static_cast<int>(boundingRect().height() * dApp->devicePixelRatio()));
            m_pixmap.fill(Qt::white);
            m_renderPixmap = m_pixmap;
            m_renderPixmap.setDevicePixelRatio(dApp->devicePixelRatio());
        } else {
            // qCDebug(appLog) << "BrowserPage::render() - Pixmap is not null, scaling pixmap";
            m_renderPixmap = m_pixmap.scaled(static_cast<int>(boundingRect().width() * dApp->devicePixelRatio()),
                                             static_cast<int>(boundingRect().height() * dApp->devicePixelRatio()));
            m_renderPixmap.setDevicePixelRatio(dApp->devicePixelRatio());
        }

        ++m_pixmapId;

        PageRenderThread::clearImageTasks(m_sheet, this, m_pixmapId);

        if (isBigDoc()) {
            // qCDebug(appLog) << "BrowserPage::render() - Is big document, creating big image task";
            DocPageBigImageTask task;

            task.sheet = m_sheet;

            task.page = this;

            task.pixmapId = m_pixmapId;

            task.rect = QRect(0, 0,
                              static_cast<int>(boundingRect().width() * dApp->devicePixelRatio()),
                              static_cast<int>(boundingRect().height() * dApp->devicePixelRatio()));

            PageRenderThread::appendTask(task);
        } else {
            // qCDebug(appLog) << "BrowserPage::render() - Is not big document, creating normal image task";
            DocPageNormalImageTask task;

            task.sheet = m_sheet;

            task.page = this;

            task.pixmapId = m_pixmapId;

            task.rect = QRect(0, 0,
                              static_cast<int>(boundingRect().width() * dApp->devicePixelRatio()),
                              static_cast<int>(boundingRect().height() * dApp->devicePixelRatio()));

            PageRenderThread::appendTask(task);
        }

        if (!m_hasLoadedAnnotation || !m_annotatinIsRendering) {
            // qCDebug(appLog) << "BrowserPage::render() - Has not loaded annotation or annotation is not rendering, creating annotation task";
            m_annotatinIsRendering = true;

            DocPageAnnotationTask task;

            task.sheet = m_sheet;

            task.page = this;

            PageRenderThread::appendTask(task);
        }
    }

    if (!m_parent->property("pinchgetsturing").toBool()) {
        //在触摸屏捏合状态时,不要进行word缩放,不然会被卡的
        // qCDebug(appLog) << "BrowserPage::render() - Not pinch get sturing, scaling words and annotations";
        scaleWords();
        scaleAnnots();
    }

    update();
    // qCDebug(appLog) << "BrowserPage::render() - Render operation completed";
}

void BrowserPage::renderRect(const QRectF &rect)
{
    // qCDebug(appLog) << "BrowserPage::renderRect() - Starting render rect";
    if (nullptr == m_parent)
        return;

    QRect validRect = boundingRect().intersected(rect).toRect();

    DocPageSliceImageTask task;

    task.sheet = m_sheet;

    task.page = this;

    task.pixmapId = m_pixmapId;

    task.whole = QRect(0, 0,
                       static_cast<int>(boundingRect().width() * dApp->devicePixelRatio()),
                       static_cast<int>(boundingRect().height() * dApp->devicePixelRatio()));

    task.slice = QRect(static_cast<int>(validRect.x() * dApp->devicePixelRatio()),
                       static_cast<int>(validRect.y() * dApp->devicePixelRatio()),
                       static_cast<int>(validRect.width() * dApp->devicePixelRatio()),
                       static_cast<int>(validRect.height() * dApp->devicePixelRatio()));

    PageRenderThread::appendTask(task);
    // qCDebug(appLog) << "BrowserPage::renderRect() - Render rect completed";
}

void BrowserPage::renderViewPort()
{
    // qCDebug(appLog) << "BrowserPage::renderViewPort() - Starting render viewport";
    if (nullptr == m_parent)
        return;

    QRect viewPortRect = QRect(0, 0, m_parent->size().width(), m_parent->size().height());

    QRectF visibleSceneRectF = m_parent->mapToScene(viewPortRect).boundingRect();

    QRectF intersectedRectF = this->mapToScene(this->boundingRect()).boundingRect().intersected(visibleSceneRectF);

    //如果不在当前可视范围则不加载 强制也没用
    if (intersectedRectF.height() <= 0 && intersectedRectF.width() <= 0)
        return;

    QRectF viewRenderRectF = mapFromScene(intersectedRectF).boundingRect();

    QRect viewRenderRect = QRect(static_cast<int>(viewRenderRectF.x()), static_cast<int>(viewRenderRectF.y()),
                                 static_cast<int>(viewRenderRectF.width()), static_cast<int>(viewRenderRectF.height()));

    //扩大加载的视图窗口范围 防止小范围的拖动
    int expand = 200;

    viewRenderRect.setX(viewRenderRect.x() - expand < 0 ? 0 : viewRenderRect.x() - expand);

    viewRenderRect.setY(viewRenderRect.y() - expand < 0 ? 0 : viewRenderRect.y() - expand);

    viewRenderRect.setWidth(viewRenderRect.x() + viewRenderRect.width() + expand * 2 > boundingRect().width() ? viewRenderRect.width() :  viewRenderRect.width() + expand * 2);

    viewRenderRect.setHeight(viewRenderRect.y() + viewRenderRect.height() + expand * 2 > boundingRect().height() ? viewRenderRect.height() :  viewRenderRect.height() + expand * 2);

    renderRect(viewRenderRect);

    m_viewportRendered = true;
    // qCDebug(appLog) << "BrowserPage::renderViewPort() - Render viewport completed";
}

void BrowserPage::handleRenderFinished(const int &pixmapId, const QPixmap &pixmap, const QRect &slice)
{
    // qCDebug(appLog) << "BrowserPage::handleRenderFinished pixmapId:" << pixmapId << "slice:" << slice;
    if (m_pixmapId != pixmapId)
        return;

    if (!slice.isValid()) { //不是切片，整体更新
        m_pixmapHasRendered = true;
        m_pixmap = pixmap;
    } else { //局部
        QPainter painter(&m_pixmap);
        painter.drawPixmap(slice, pixmap);
    }

    m_renderPixmap = m_pixmap;

    m_renderPixmap.setDevicePixelRatio(dApp->devicePixelRatio());

    update();
    // qCDebug(appLog) << "BrowserPage::handleRenderFinished() - Handle render finished completed";
}

void BrowserPage::handleWordLoaded(const QList<Word> &words)
{
    // qCDebug(appLog) << "BrowserPage::handleWordLoaded() - Starting handle word loaded";
    m_wordIsRendering = false;

    if (!m_wordNeeded)
        return;

    if (m_wordHasRendered)
        return;

    m_wordHasRendered = true;

    for (int i = 0; i < words.count(); ++i) {
        BrowserWord *word = new BrowserWord(this, words[i]);

        word->setSelectable(m_wordSelectable);

        m_words.append(word);
    }

    scaleWords(true);
    // qCDebug(appLog) << "BrowserPage::handleWordLoaded() - Handle word loaded completed";
}

void BrowserPage::handleAnnotationLoaded(const QList<Annotation *> &annots)
{
    // qCDebug(appLog) << "BrowserPage::handleAnnotationLoaded() - Starting handle annotation loaded";
    m_annotatinIsRendering = false;

    //如果已经加载了，则过滤本次加载 (存在不通过线程加载的情况)
    if (m_hasLoadedAnnotation) {
        qDeleteAll(annots);
        return;
    }

    //在reload之前将上一次选中去掉,避免操作野指针
    if (m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem)) {
        m_lastClickIconAnnotationItem->setDrawSelectRect(false);
        m_lastClickIconAnnotationItem = nullptr;
    }

    qDeleteAll(m_annotations);
    m_annotations.clear();

    qDeleteAll(m_annotationItems);
    m_annotationItems.clear();

    m_annotations = annots;

    for (int i = 0; i < m_annotations.count(); ++i) {
        m_annotations[i]->page = m_index + 1;
        foreach (const QRectF &rect, m_annotations[i]->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, m_annotations[i], m_scaleFactor);
            m_annotationItems.append(annotationItem);
        }
    }

    m_hasLoadedAnnotation = true;
    // qCDebug(appLog) << "BrowserPage::handleAnnotationLoaded() - Handle annotation loaded completed";
}

QImage BrowserPage::getCurrentImage(int width, int height)
{
    qCDebug(appLog) << "Getting current image, requested size:" << width << "x" << height;
    if (m_pixmap.isNull()) {
        qCWarning(appLog) << "Pixmap is null, returning empty image";
        return QImage();
    }

    //获取图片比原图还大,就不需要原图了
    if (qMin(width, height) > qMax(m_pixmap.width(), m_pixmap.height()))
        return QImage();

    QImage image = m_pixmap.toImage().scaled(static_cast<int>(width), static_cast<int>(height), Qt::KeepAspectRatio);

    return image;
}

QImage BrowserPage::getImagePoint(double scaleFactor, QPoint point)
{
    qCDebug(appLog) << "Getting image at point:" << point << "with scale factor:" << scaleFactor;
    QTransform transform;

    transform.rotate(m_rotation * 90);

    int ss = static_cast<int>(122 * scaleFactor / m_scaleFactor);

    QRect rect = QRect(qRound(point.x() * scaleFactor / m_scaleFactor - ss / 2.0),
                       qRound(point.y() * scaleFactor / m_scaleFactor - ss / 2.0), ss, ss);

    return m_sheet->renderer()->getImage(itemIndex(),
                                         static_cast<int>(m_originSizeF.width() * scaleFactor),
                                         static_cast<int>(m_originSizeF.height() * scaleFactor),
                                         rect)
           .transformed(transform, Qt::SmoothTransformation);
}

QImage BrowserPage::getCurImagePoint(QPointF point)
{
    // qCDebug(appLog) << "BrowserPage::getCurImagePoint() - Starting get current image point";
    int ds = static_cast<int>(122 * dApp->devicePixelRatio());
    QTransform transform;
    transform.rotate(m_rotation * 90);
    const QImage &image = Utils::copyImage(m_renderPixmap.toImage(), qRound(point.x() * dApp->devicePixelRatio() - ds / 2.0), qRound(point.y() * dApp->devicePixelRatio()  - ds / 2.0), ds, ds).transformed(transform, Qt::SmoothTransformation);
    // qCDebug(appLog) << "BrowserPage::getCurImagePoint() - Get current image point completed";
    return image;
}

int BrowserPage::itemIndex()
{
    // qCDebug(appLog) << "BrowserPage::itemIndex() - Returning item index:" << m_index;
    return m_index;
}

QString BrowserPage::selectedWords()
{
    // qCDebug(appLog) << "BrowserPage::selectedWords() - Starting selected words";
    QString text;
    foreach (BrowserWord *word, m_words) {
        if (word->isSelected()) {
            text += word->text();
        }
    }

    // qCDebug(appLog) << "BrowserPage::selectedWords() - Selected words completed";
    return text;
}

void BrowserPage::setWordSelectable(bool selectable)
{
    // qCDebug(appLog) << "BrowserPage::setWordSelectable() - Starting set word selectable";
    m_wordSelectable = selectable;
    foreach (BrowserWord *word, m_words) {
        word->setSelectable(selectable);
    }
    // qCDebug(appLog) << "BrowserPage::setWordSelectable() - Set word selectable completed";
}

void BrowserPage::loadWords()
{
    // qCDebug(appLog) << "BrowserPage::loadWords() - Starting load words";
    m_wordNeeded = true;

    if (m_wordIsRendering) {
        // qCDebug(appLog) << "Word is rendering, return";
        return;
    }

    if (m_wordHasRendered) {
        //如果已经加载则取消隐藏和改变大小
        // qCDebug(appLog) << "BrowserPage::loadWords() - Word is rendered, cancel hide and change size";
        if (m_words.count() <= 0)
            return;

        prepareGeometryChange();

        if (!qFuzzyCompare(m_wordScaleFactor, m_scaleFactor)) {
            // qCDebug(appLog) << "BrowserPage::loadWords() - Word scale factor changed, updating word scale factor";
            m_wordScaleFactor = m_scaleFactor;
            foreach (BrowserWord *word, m_words) {
                word->setScaleFactor(m_scaleFactor);
            }
        }

        // qCDebug(appLog) << "BrowserPage::loadWords() - Word scale factor changed, updating word scale factor completed";
        return;
    }

    DocPageWordTask task;

    task.sheet = m_sheet;

    task.page = this;

    PageRenderThread::appendTask(task);

    m_wordHasRendered = false;

    m_wordIsRendering = true;
    // qCDebug(appLog) << "BrowserPage::loadWords() - Load words completed";
}

void BrowserPage::clearPixmap()
{
    // qCDebug(appLog) << "Clearing pixmap for page" << m_index;
    if (m_renderPixmapScaleFactor < -0.0001)
        return;

    m_pixmap = QPixmap();

    m_renderPixmap = m_pixmap;

    ++m_pixmapId;

    m_pixmapHasRendered = false;

    m_viewportRendered  = false;

    m_renderPixmapScaleFactor = -1;

    PageRenderThread::clearImageTasks(m_sheet, this);
    // qCDebug(appLog) << "BrowserPage::clearPixmap() - Clear pixmap completed";
}

void BrowserPage::clearWords()
{
    // qCDebug(appLog) << "BrowserPage::clearWords() - Starting clear words";
    if (!m_wordNeeded)
        return;

    if (!m_wordHasRendered)
        return;

    foreach (BrowserWord *word, m_words) {
        if (word->isSelected())
            return;
    }

    QList<BrowserWord *> t_word = m_words;
    m_words.clear();

    m_wordHasRendered = false;
    m_wordNeeded = false;

    foreach (BrowserWord *word, t_word) {
        word->setParentItem(nullptr);
        scene()->removeItem(word);
        delete word;
    }
    // qCDebug(appLog) << "BrowserPage::clearWords() - Clear words completed";
}

void BrowserPage::scaleAnnots(bool force)
{
    // qCDebug(appLog) << "BrowserPage::scaleAnnots() - Starting scale annotations";
    if (!m_annotatinIsRendering || m_annotationItems.count() <= 0)
        return;

    prepareGeometryChange();

    if (force || !qFuzzyCompare(m_annotScaleFactor, m_scaleFactor)) {
        // qCDebug(appLog) << "BrowserPage::scaleAnnots() - Scale factor changed, updating scale factor";
        m_wordScaleFactor = m_scaleFactor;
        foreach (BrowserAnnotation *annot, m_annotationItems) {
            annot->setScaleFactor(m_scaleFactor);
        }
    }
    // qCDebug(appLog) << "BrowserPage::scaleAnnots() - Scale annotations completed";
}

void BrowserPage::scaleWords(bool force)
{
    // qCDebug(appLog) << "BrowserPage::scaleWords() - Starting scale words";
    if (!m_wordHasRendered || m_words.count() <= 0)
        return;

    prepareGeometryChange();

    if (force || !qFuzzyCompare(m_wordScaleFactor, m_scaleFactor)) {
        // qCDebug(appLog) << "BrowserPage::scaleWords() - Scale factor changed, updating scale factor";
        m_wordScaleFactor = m_scaleFactor;
        foreach (BrowserWord *word, m_words) {
            word->setScaleFactor(m_scaleFactor);
        }
    }
    // qCDebug(appLog) << "BrowserPage::scaleWords() - Scale words completed";
}

QList<deepin_reader::Annotation *> BrowserPage::annotations()
{
    // qCDebug(appLog) << "BrowserPage::annotations() - Starting get annotations";
    if (!m_hasLoadedAnnotation) {
        handleAnnotationLoaded(m_sheet->renderer()->getAnnotations(itemIndex()));
    }

    // qCDebug(appLog) << "BrowserPage::annotations() - Get annotations completed";
    return m_annotations;
}

bool BrowserPage::updateAnnotation(deepin_reader::Annotation *annotation, const QString &text, const QColor &color)
{
    // qCDebug(appLog) << "BrowserPage::updateAnnotation() - Starting update annotation";
    if (nullptr == annotation)
        return false;

    if (!m_annotations.contains(annotation))
        return false;

    if (!m_sheet->renderer()->updateAnnotation(itemIndex(), annotation, text, color))
        return false;

    // qCDebug(appLog) << "BrowserPage::updateAnnotation() - Update annotation completed";
    QRectF renderBoundary;
    const QList<QRectF> &annoBoundaries = annotation->boundary();
    for (int i = 0; i < annoBoundaries.size(); i++) {
        renderBoundary = renderBoundary | annoBoundaries.at(i);
    }

    renderBoundary.adjust(-10, -10, 10, 10);
    renderRect(QRectF(renderBoundary.x() * m_scaleFactor,
                      renderBoundary.y() * m_scaleFactor,
                      renderBoundary.width() * m_scaleFactor,
                      renderBoundary.height() * m_scaleFactor));

    m_sheet->handlePageModified(m_index);

    // qCDebug(appLog) << "BrowserPage::updateAnnotation() - Update annotation completed";
    return true;
}

Annotation *BrowserPage::addHighlightAnnotation(QString text, QColor color)
{
    // qCInfo(appLog) << "BrowserPage::addHighlightAnnotation text:" << text << "color:" << color;
    Annotation *highLightAnnot = nullptr;
    QList<QRectF> boundaries;

    //加载文档文字无旋转情况下的文字(即旋转0度时的所有文字)
    const QList<deepin_reader::Word> &twords = m_sheet->renderer()->getWords(itemIndex());

    if (m_words.size() != twords.size())
        return nullptr;

    int wordCnt = twords.size();

    QRectF selectBoundRectF;
    bool bresetSelectRect = true;

    for (int index = 0; index < wordCnt; index++) {
        if (m_words.at(index) && m_words.at(index)->isSelected()) {
            m_words.at(index)->setSelected(false);

            const QRectF &textRectf = twords[index].wordBoundingRect();
            if (bresetSelectRect) {
                bresetSelectRect = false;
                selectBoundRectF = textRectf;
            } else {
                if (qFuzzyCompare(selectBoundRectF.height(), textRectf.height()) && qFuzzyCompare(selectBoundRectF.y(), textRectf.y()) && selectBoundRectF.right() > (textRectf.x() - 1)) {
                    selectBoundRectF = selectBoundRectF.united(textRectf);
                } else if (qFuzzyCompare(selectBoundRectF.right(), textRectf.x())) {
                    selectBoundRectF = selectBoundRectF.united(textRectf);
                } else {
                    if (selectBoundRectF.width() > 0.00001 && selectBoundRectF.height() > 0.00001)
                        boundaries << selectBoundRectF;
                    selectBoundRectF = textRectf;
                }
            }
        }
    }

    if (selectBoundRectF.width() > 0.00001 && selectBoundRectF.height() > 0.00001)
        boundaries << selectBoundRectF;

    if (boundaries.count() > 0) {
        // qCDebug(appLog) << "BrowserPage::addHighlightAnnotation() - Boundaries count:" << boundaries.count();
        //需要保证已经加载注释
        if (!m_hasLoadedAnnotation) {
            // qCDebug(appLog) << "BrowserPage::addHighlightAnnotation() - Has not loaded annotation, loading annotation";
            handleAnnotationLoaded(m_sheet->renderer()->getAnnotations(itemIndex()));
        }

        highLightAnnot = m_sheet->renderer()->addHighlightAnnotation(itemIndex(), boundaries, text, color);
        if (highLightAnnot == nullptr)
            return nullptr;

        highLightAnnot->page = m_index + 1;
        m_annotations.append(highLightAnnot);

        foreach (const QRectF &rect, highLightAnnot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, highLightAnnot, m_scaleFactor);
            m_annotationItems.append(annotationItem);
        }

        QRectF renderBoundary;
        for (int i = 0; i < boundaries.size(); i++) {
            renderBoundary = renderBoundary | boundaries.at(i);
        }

        renderBoundary.adjust(-10, -10, 10, 10);
        renderRect(QRectF(renderBoundary.x() * m_scaleFactor, renderBoundary.y() * m_scaleFactor,
                          renderBoundary.width() * m_scaleFactor, renderBoundary.height() * m_scaleFactor));
    }

    m_sheet->handlePageModified(m_index);

    // qCDebug(appLog) << "BrowserPage::addHighlightAnnotation() - Add highlight annotation completed";
    return highLightAnnot;
}

bool BrowserPage::hasAnnotation(deepin_reader::Annotation *annotation)
{
    // qCDebug(appLog) << "BrowserPage::hasAnnotation() - Starting has annotation";
    return m_annotations.contains(annotation);
}

void BrowserPage::setSelectIconRect(const bool draw, Annotation *iconAnnot)
{
    // qCDebug(appLog) << "BrowserPage::setSelectIconRect() - Starting set select icon rect";
    QList<QRectF> rectList;

    if (iconAnnot) {
        // qCDebug(appLog) << "BrowserPage::setSelectIconRect() - Icon annotation found, setting select icon rect";
        foreach (BrowserAnnotation *annotation, m_annotationItems) {
            if (annotation && annotation->isSame(iconAnnot)) {
                m_lastClickIconAnnotationItem = annotation;
                m_lastClickIconAnnotationItem->setScaleFactor(m_scaleFactor);
                if (iconAnnot->type() == deepin_reader::Annotation::AText)
                    m_lastClickIconAnnotationItem->setDrawSelectRect(draw);
            }
        }
    } else {
        // qCDebug(appLog) << "BrowserPage::setSelectIconRect() - Icon annotation not found, setting select icon rect to false";
        if (m_lastClickIconAnnotationItem && m_annotationItems.contains(m_lastClickIconAnnotationItem))
            m_lastClickIconAnnotationItem->setDrawSelectRect(draw);
    }
    // qCDebug(appLog) << "BrowserPage::setSelectIconRect() - Set select icon rect completed";
}

void BrowserPage::setDrawMoveIconRect(const bool draw)
{
    // qCDebug(appLog) << "BrowserPage::setDrawMoveIconRect() - Starting set draw move icon rect";
    m_drawMoveIconRect = draw;

    update();
    // qCDebug(appLog) << "BrowserPage::setDrawMoveIconRect() - Set draw move icon rect completed";
}

QPointF BrowserPage::iconMovePos()
{
    // qCDebug(appLog) << "BrowserPage::iconMovePos() - Starting get icon move pos";
    return m_drawMoveIconPoint;
}

void BrowserPage::setIconMovePos(const QPointF movePoint)
{
    // qCDebug(appLog) << "BrowserPage::setIconMovePos() - Starting set icon move pos";
    m_drawMoveIconPoint = movePoint;

    update();
    // qCDebug(appLog) << "BrowserPage::setIconMovePos() - Set icon move pos completed";
}

bool BrowserPage::moveIconAnnotation(const QRectF &moveRect)
{
    // qCDebug(appLog) << "BrowserPage::moveIconAnnotation() - Starting move icon annotation";
    if (nullptr == m_lastClickIconAnnotationItem)
        return false;

    QList<QRectF> annoBoundaries;

    m_annotationItems.removeAll(m_lastClickIconAnnotationItem);
    annoBoundaries << m_lastClickIconAnnotationItem->annotation()->boundary();
    annoBoundaries << moveRect;
    Annotation *annot = m_sheet->renderer()->moveIconAnnotation(itemIndex(), m_lastClickIconAnnotationItem->annotation(), moveRect);

    if (annot && m_annotations.contains(annot)) {
        // qCDebug(appLog) << "BrowserPage::moveIconAnnotation() - Annotation found, deleting last click icon annotation item";
        delete m_lastClickIconAnnotationItem;
        m_lastClickIconAnnotationItem = nullptr;
        annot->page = m_index + 1;
        foreach (const QRectF &rect, annot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, annot, m_scaleFactor);
            m_annotationItems.append(annotationItem);
            m_lastClickIconAnnotationItem = annotationItem;
        }

        if (annot->type() == 1) {
            // qCDebug(appLog) << "BrowserPage::moveIconAnnotation() - Annotation type is 1, setting last click icon annotation item";
            if (m_lastClickIconAnnotationItem) {
                m_lastClickIconAnnotationItem->setScaleFactor(m_scaleFactor);
                m_lastClickIconAnnotationItem->setDrawSelectRect(true);
            }
        }

        QRectF renderBoundary;
        for (int i = 0; i < annoBoundaries.size(); i++) {
            renderBoundary = renderBoundary | annoBoundaries.at(i);
        }

        renderBoundary.adjust(-10, -10, 10, 10);
        renderRect(QRectF(renderBoundary.x() * m_scaleFactor, renderBoundary.y() * m_scaleFactor, renderBoundary.width() * m_scaleFactor, renderBoundary.height() * m_scaleFactor));
    }

    m_sheet->handlePageModified(m_index);

    // qCDebug(appLog) << "BrowserPage::moveIconAnnotation() - Move icon annotation completed";
    return true;
}

bool BrowserPage::removeAllAnnotation()
{
    // qCDebug(appLog) << "BrowserPage::removeAllAnnotation() - Starting remove all annotation";
    //未加载注释时则直接加载 无需添加图元
    if (!m_hasLoadedAnnotation) {
        // qCDebug(appLog) << "BrowserPage::removeAllAnnotation() - Has not loaded annotation, loading annotation";
        m_annotations = m_sheet->renderer()->getAnnotations(itemIndex());
        m_hasLoadedAnnotation = true;
    }

    m_lastClickIconAnnotationItem = nullptr;

    if (m_annotations.isEmpty())
        return false;

    //所有的注释区域 需要被重新加载界面
    QList<QRectF> annoBoundaries;

    for (int index = 0; index < m_annotations.size(); index++) {
        deepin_reader::Annotation *annota = m_annotations.at(index);

        if (!m_annotations.contains(annota) || (annota && annota->contents().isEmpty()))
            continue;

        annoBoundaries << annota->boundary();

        if (!m_sheet->renderer()->removeAnnotation(itemIndex(), annota))
            continue;

        m_annotations.removeAt(index);

        index--;

        foreach (BrowserAnnotation *annotation, m_annotationItems) {
            if (annotation && annotation->isSame(annota)) {
                if (m_lastClickIconAnnotationItem == annotation)
                    m_lastClickIconAnnotationItem = nullptr;
                m_annotationItems.removeAll(annotation);
                delete annotation;
                annotation = nullptr;
            }
        }
    }

    QRectF renderBoundary;

    for (int i = 0; i < annoBoundaries.size(); i++) {
        renderBoundary = renderBoundary | annoBoundaries.at(i);
    }

    renderBoundary.adjust(-10, -10, 10, 10);

    renderRect(QRectF(renderBoundary.x() * m_scaleFactor, renderBoundary.y() * m_scaleFactor, renderBoundary.width() * m_scaleFactor, renderBoundary.height() * m_scaleFactor));

    m_sheet->handlePageModified(m_index);

    // qCDebug(appLog) << "BrowserPage::removeAllAnnotation() - Remove all annotation completed";
    return true;
}

void BrowserPage::setPageBookMark(const QPointF clickPoint)
{
    // qCDebug(appLog) << "BrowserPage::setPageBookMark() - Starting set page book mark";
    if (bookmarkMouseRect().contains(clickPoint)) {
        // qCDebug(appLog) << "BrowserPage::setPageBookMark() - Bookmark mouse rect contains click point";
        m_bookmarkState = 2;
        if (nullptr != m_parent) {
            // qCDebug(appLog) << "BrowserPage::setPageBookMark() - Parent is not null, calling need bookmark";
            m_parent->needBookmark(m_index, !m_bookmark);
            if (!m_bookmark && bookmarkMouseRect().contains(clickPoint))
                m_bookmarkState = 1;
            else if (m_bookmark)
                m_bookmarkState = 3;
            else
                m_bookmarkState = 0;
        }
        update();
    }
    // qCDebug(appLog) << "BrowserPage::setPageBookMark() - Set page book mark completed";
}

QPointF BrowserPage::getTopLeftPos()
{
    // qCDebug(appLog) << "BrowserPage::getTopLeftPos() - Starting get top left pos";
    QPointF p;
    switch (m_rotation) {
    default:
    case Dr::RotateBy0:
        p = pos();
        break;
    case Dr::RotateBy270:
        p.setX(pos().x());
        p.setY(pos().y() - rect().height());
        break;
    case Dr::RotateBy180:
        p.setX(pos().x() - rect().width());
        p.setY(pos().y() - rect().height());
        break;
    case Dr::RotateBy90:
        p.setX(pos().x() - rect().width());
        p.setY(pos().y());
        break;
    }
    // qCDebug(appLog) << "BrowserPage::getTopLeftPos() - Get top left pos completed";
    return p;
}

bool BrowserPage::removeAnnotation(deepin_reader::Annotation *annota)
{
    // qCInfo(appLog) << "Removing annotation" << annota << "from page" << m_index;
    if (nullptr == annota)
        return false;

    if (!m_annotations.contains(annota))
        return false;

    m_annotations.removeAll(annota);

    const QList<QRectF> &annoBoundaries = annota->boundary();

    if (!m_sheet->renderer()->removeAnnotation(itemIndex(), annota))
        return false;

    foreach (BrowserAnnotation *annotation, m_annotationItems) {
        if (annotation && annotation->isSame(annota)) {
            if (m_lastClickIconAnnotationItem == annotation)
                m_lastClickIconAnnotationItem = nullptr;
            m_annotationItems.removeAll(annotation);
            delete annotation;
            annotation = nullptr;
        }
    }

    QRectF renderBoundary;

    for (int i = 0; i < annoBoundaries.size(); i++) {
        renderBoundary = renderBoundary | annoBoundaries.at(i);
    }

    renderBoundary.adjust(-10, -10, 10, 10);

    renderRect(QRectF(renderBoundary.x() * m_scaleFactor, renderBoundary.y() * m_scaleFactor, renderBoundary.width() * m_scaleFactor, renderBoundary.height() * m_scaleFactor));

    m_sheet->handlePageModified(m_index);

    // qCDebug(appLog) << "BrowserPage::removeAnnotation() - Remove annotation completed";
    return true;
}

Annotation *BrowserPage::addIconAnnotation(const QRectF &rect, const QString &text)
{
    // qCInfo(appLog) << "Adding icon annotation at rect:" << rect << "with text:" << text;
    Annotation *annot = m_sheet->renderer()->addIconAnnotation(itemIndex(), rect, text);

    if (annot) {
        // qCDebug(appLog) << "BrowserPage::addIconAnnotation() - Annotation found, adding annotation";
        annot->page = m_index + 1;

        m_annotations.append(annot);

        foreach (const QRectF &arect, annot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, arect, annot, m_scaleFactor);
            m_annotationItems.append(annotationItem);
            m_lastClickIconAnnotationItem = annotationItem;
        }

        if (annot->type() == deepin_reader::Annotation::AText) {
            // qCDebug(appLog) << "BrowserPage::addIconAnnotation() - Annotation type is AText, setting last click icon annotation item";
            if (m_lastClickIconAnnotationItem) {
                m_lastClickIconAnnotationItem->setScaleFactor(m_scaleFactor);
                m_lastClickIconAnnotationItem->setDrawSelectRect(true);
            }
        }

        QRectF renderBoundary;

        const QList<QRectF> &annoBoundaries = annot->boundary();

        for (int i = 0; i < annoBoundaries.size(); i++) {
            renderBoundary = renderBoundary | annoBoundaries.at(i);
        }

        renderBoundary.adjust(-10, -10, 10, 10);

        renderRect(QRectF(renderBoundary.x() * m_scaleFactor, renderBoundary.y() * m_scaleFactor, renderBoundary.width() * m_scaleFactor, renderBoundary.height() * m_scaleFactor));

        m_sheet->handlePageModified(m_index);
    }

    // qCDebug(appLog) << "BrowserPage::addIconAnnotation() - Add icon annotation completed";
    return annot;
}

bool BrowserPage::sceneEvent(QEvent *event)
{
    // qCDebug(appLog) << "BrowserPage::sceneEvent() - Starting scene event";
    if (event->type() == QEvent::GraphicsSceneHoverMove) {
        // qCDebug(appLog) << "BrowserPage::sceneEvent() - Graphics scene hover move";
        QGraphicsSceneHoverEvent *moveevent = dynamic_cast<QGraphicsSceneHoverEvent *>(event);
        if (!m_bookmark && bookmarkMouseRect().contains(moveevent->pos()))
            m_bookmarkState = 1;
        else if (m_bookmark)
            m_bookmarkState = 3;
        else
            m_bookmarkState = 0;
        update();
    }
    // qCDebug(appLog) << "BrowserPage::sceneEvent() - Scene event completed";
    return QGraphicsItem::sceneEvent(event);
}

void BrowserPage::setSearchHighlightRectf(const QVector<PageSection> &sections)
{
    // qCDebug(appLog) << "Setting search highlight for" << sections.size() << "sections";
    if (sections.size() > 0) {
        // qCDebug(appLog) << "BrowserPage::setSearchHighlightRectf() - Sections size is greater than 0";
        if (m_parent->currentPage() == this->itemIndex() + 1)
            m_searchSelectLighRectf = sections.first();
        m_searchLightrectLst = sections;
        update();
    }
    // qCDebug(appLog) << "BrowserPage::setSearchHighlightRectf() - Set search highlight completed";
}

void BrowserPage::clearSearchHighlightRects()
{
    // qCDebug(appLog) << "Clearing search highlights";
    m_searchSelectLighRectf.clear();
    m_searchLightrectLst.clear();
    update();
    // qCDebug(appLog) << "BrowserPage::clearSearchHighlightRects() - Clear search highlights completed";
}

void BrowserPage::clearSelectSearchHighlightRects()
{
    // qCDebug(appLog) << "BrowserPage::clearSelectSearchHighlightRects() - Starting clear select search highlight rects";
    m_searchSelectLighRectf.clear();
    update();
    // qCDebug(appLog) << "BrowserPage::clearSelectSearchHighlightRects() - Clear select search highlight rects completed";
}

int BrowserPage::searchHighlightRectSize()
{
    // qCDebug(appLog) << "BrowserPage::searchHighlightRectSize() - Starting search highlight rect size";
    return m_searchLightrectLst.size();
}

PageSection BrowserPage::findSearchforIndex(int index)
{
    // qCDebug(appLog) << "BrowserPage::findSearchforIndex() - Starting find search for index";
    if (index >= 0 && index < m_searchLightrectLst.size()) {
        // qCDebug(appLog) << "BrowserPage::findSearchforIndex() - Index is greater than 0 and less than search light rect list size";
        m_searchSelectLighRectf = m_searchLightrectLst[index];
        update();
        return m_searchSelectLighRectf;
    }

    // qCDebug(appLog) << "BrowserPage::findSearchforIndex() - Find search for index completed";
    return PageSection();
}

QRectF BrowserPage::getNorotateRect(const QRectF &rect)
{
    // qCDebug(appLog) << "BrowserPage::getNorotateRect() - Starting get norotate rect";
    QRectF newrect;
    newrect.setX(rect.x()*m_scaleFactor);
    newrect.setY(rect.y()*m_scaleFactor);
    newrect.setWidth(rect.width()*m_scaleFactor);
    newrect.setHeight(rect.height()*m_scaleFactor);
    // qCDebug(appLog) << "BrowserPage::getNorotateRect() - Get norotate rect completed";
    return newrect;
}

QRectF BrowserPage::translateRect(const QRectF &rect)
{
    //旋转角度逆时针增加
    QRectF newrect;
    // qCDebug(appLog) << "BrowserPage::translateRect() - Starting translate rect";
    switch (m_rotation) {
    case Dr::RotateBy0: {
        // qCDebug(appLog) << "BrowserPage::translateRect() - Rotate by 0";
        newrect.setX(rect.x()*m_scaleFactor);
        newrect.setY(rect.y()*m_scaleFactor);
        newrect.setWidth(rect.width()*m_scaleFactor);
        newrect.setHeight(rect.height()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy90: {
        // qCDebug(appLog) << "BrowserPage::translateRect() - Rotate by 90";
        newrect.setX((m_originSizeF.height() - rect.y() - rect.height())*m_scaleFactor - boundingRect().height());
        newrect.setY(rect.x()*m_scaleFactor);
        newrect.setWidth(rect.height()*m_scaleFactor);
        newrect.setHeight(rect.width()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy180: {
        // qCDebug(appLog) << "BrowserPage::translateRect() - Rotate by 180";
        newrect.setX((m_originSizeF.width() - rect.x() - rect.width())*m_scaleFactor - boundingRect().width());
        newrect.setY((m_originSizeF.height() - rect.y() - rect.height())*m_scaleFactor - boundingRect().height());
        newrect.setWidth(rect.width()*m_scaleFactor);
        newrect.setHeight(rect.height()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy270: {
        // qCDebug(appLog) << "BrowserPage::translateRect() - Rotate by 270";
        newrect.setX(rect.y()*m_scaleFactor);
        newrect.setY((m_originSizeF.width() - rect.x() - rect.width())*m_scaleFactor - boundingRect().width());
        newrect.setWidth(rect.height()*m_scaleFactor);
        newrect.setHeight(rect.width()*m_scaleFactor);
        break;
    }
    default:
        // qCDebug(appLog) << "BrowserPage::translateRect() - Rotate by default";
        break;
    }
    // qCDebug(appLog) << "BrowserPage::translateRect() - Translate rect completed";
    return  newrect;
}

BrowserAnnotation *BrowserPage::getBrowserAnnotation(const QPointF &point)
{
    // qCDebug(appLog) << "BrowserPage::getBrowserAnnotation() - Starting get browser annotation";
    BrowserAnnotation *item = nullptr;
    const QList<QGraphicsItem *> &itemlst = scene()->items(this->mapToScene(point));
    for (QGraphicsItem *itemIter : itemlst) {
        item = dynamic_cast<BrowserAnnotation *>(itemIter);
        if (item != nullptr) {
            // qCDebug(appLog) << "BrowserPage::getBrowserAnnotation() - Browser annotation found";
            return item;
        }
    }

    // qCDebug(appLog) << "BrowserPage::getBrowserAnnotation() - Browser annotation not found";
    return nullptr;
}

BrowserWord *BrowserPage::getBrowserWord(const QPointF &point)
{
    // qCDebug(appLog) << "BrowserPage::getBrowserWord() - Starting get browser word";
    BrowserWord *item = nullptr;
    const QList<QGraphicsItem *> &itemlst = scene()->items(this->mapToScene(point));
    for (QGraphicsItem *itemIter : itemlst) {
        item = dynamic_cast<BrowserWord *>(itemIter);
        if (item != nullptr) {
            // qCDebug(appLog) << "BrowserPage::getBrowserWord() - Browser word found";
            return item;
        }
    }

    // qCDebug(appLog) << "BrowserPage::getBrowserWord() - Browser word not found";
    return nullptr;
}

bool BrowserPage::isBigDoc()
{
    qCDebug(appLog) << "BrowserPage::isBigDoc() - Starting is big doc";
    bool isBig = Dr::PDF == m_sheet->fileType() && boundingRect().width() > 1000 && boundingRect().height() > 1000;
    qCDebug(appLog) << "Checking if document is big:" << isBig;
    return isBig;
}
