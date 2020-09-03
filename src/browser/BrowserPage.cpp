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
#include "BrowserPage.h"
#include "document/Model.h"
#include "PageRenderThread.h"
#include "SheetBrowser.h"
#include "BrowserWord.h"
#include "BrowserLink.h"
#include "BrowserAnnotation.h"
#include "Application.h"
#include "PageViewportThread.h"
#include "Utils.h"

#include <DApplicationHelper>

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QTime>
#include <QMutexLocker>
#include <QTimer>
#include <QUuid>
#include <QPainterPath>
#include <QDesktopServices>

QSet<BrowserPage *> BrowserPage::items;
BrowserPage::BrowserPage(SheetBrowser *parent, deepin_reader::Page *page, QList<deepin_reader::Page *> renderPages) : QGraphicsItem(), m_page(page), m_renderPages(renderPages), m_parent(parent)
{
    items.insert(this);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsPanel);
}

BrowserPage::~BrowserPage()
{
    items.remove(this);

    qDeleteAll(m_annotations);

    qDeleteAll(m_annotations0);

    qDeleteAll(m_annotations1);

    qDeleteAll(m_annotations2);

    qDeleteAll(m_annotations3);

    qDeleteAll(m_annotationItems);

    qDeleteAll(m_linkItems);

    qDeleteAll(m_words);

    if (nullptr != m_page)
        delete m_page;

    qDeleteAll(m_renderPages);
}

void BrowserPage::reOpen(Page *page, QList<deepin_reader::Page *> renderPages)
{
    Page *tmpPage = m_page;
    QList<deepin_reader::Page *> tempPages = m_renderPages;

    m_page = page;
    m_renderPages = renderPages;

    qDeleteAll(m_annotations);
    m_annotations.clear();

    qDeleteAll(m_annotationItems);
    m_annotationItems.clear();

    m_hasLoadedAnnotation = false;

    if (nullptr != tmpPage)
        delete tmpPage;

    qDeleteAll(tempPages);

    //word和link不需要重新加载
}

/**
 * @brief BrowserPage::boundingRect
 * 文档页rect(不一定是0度下的)
 * @return
 */
QRectF BrowserPage::boundingRect() const
{
    if (nullptr == m_page)
        return QRectF(0, 0, 0, 0);

    return QRectF(0, 0, static_cast<double>(m_page->sizeF().width() * m_scaleFactor), static_cast<double>(m_page->sizeF().height() * m_scaleFactor));
}

/**
 * @brief BrowserPage::rect
 * 文档页rect(0度)
 * @return
 */
QRectF BrowserPage::rect()
{
    if (nullptr == m_page)
        return QRectF(0, 0, 0, 0);

    switch (m_rotation) {
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        return QRectF(0, 0, static_cast<double>(m_page->sizeF().height() * m_scaleFactor), static_cast<double>(m_page->sizeF().width() * m_scaleFactor));
    default: break;
    }

    return QRectF(0, 0, static_cast<double>(m_page->sizeF().width() * m_scaleFactor), static_cast<double>(m_page->sizeF().height() * m_scaleFactor));
}

/**
 * @brief BrowserPage::bookmarkRect
 * 书签范围
 * @return
 */
QRectF BrowserPage::bookmarkRect()
{
    return QRectF(boundingRect().width() - 40, 1, 39, 39);
}

/**
 * @brief BrowserPage::bookmarkMouseRect
 * 书签图标范围
 * @return
 */
QRectF BrowserPage::bookmarkMouseRect()
{
    return QRectF(boundingRect().width() - 27, 10, 14, 20);
}

/**
 * @brief BrowserPage::setBookmark
 * 设置书签
 * @param hasBookmark
 */
void BrowserPage::setBookmark(const bool &hasBookmark)
{
    m_bookmark = hasBookmark;

    if (hasBookmark)
        m_bookmarkState = 3;
    else
        m_bookmarkState = 0;

    update();
}

/**
 * @brief BrowserPage::updateBookmarkState
 * 更新书签状态
 */
void BrowserPage::updateBookmarkState()
{
    if (m_bookmark)
        m_bookmarkState = 3;
    else
        m_bookmarkState = 0;

    update();
}

/**
 * @brief BrowserPage::paint
 * 绘制接口
 * @param painter
 * @param option
 */
void BrowserPage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (!m_viewportTryRender)
        renderViewPort(false);

    if (!m_pixmapHasRendered) {
        render(m_scaleFactor, m_rotation);
    }

    painter->drawPixmap(option->rect, m_pixmap);

    if (m_viewportRenderedRect.isValid() && qFuzzyCompare(m_viewportScaleFactor, m_scaleFactor))
        painter->drawPixmap(m_viewportRenderedRect, m_viewportPixmap);

    if (1 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_hover").pixmap(QSize(39, 39)));
    if (2 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_pressed").pixmap(QSize(39, 39)));
    if (3 == m_bookmarkState)
        painter->drawPixmap(static_cast<int>(bookmarkRect().x()), static_cast<int>(bookmarkRect().y()), QIcon::fromTheme("dr_bookmark_checked").pixmap(QSize(39, 39)));

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(238, 220, 0, 100));
    int lightsize = m_searchLightrectLst.size();

    for (int i = 0; i < lightsize; i++) {
        painter->drawRect(getNorotateRecr(m_searchLightrectLst[i]));
    }

    painter->setBrush(QColor(59, 148, 1, 100));
    if (m_searchSelectLighRectf.width() > 0 || m_searchSelectLighRectf.height() > 0)
        painter->drawRect(getNorotateRecr(m_searchSelectLighRectf));

    if (m_drawMoveIconRect) {
        QPen pen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
        painter->setPen(pen);

        qreal iconWidth = (nullptr != m_lastClickIconAnnotation) ? (m_lastClickIconAnnotation->boundingRect().width()) : ICON_SIZE;
        qreal iconHeight = (nullptr != m_lastClickIconAnnotation) ? (m_lastClickIconAnnotation->boundingRect().height()) : ICON_SIZE;

        int x = static_cast<int>(m_drawMoveIconPoint.x() - iconWidth / 2.0);
        int y = static_cast<int>(m_drawMoveIconPoint.y() - iconHeight / 2.0);

        if (x < 0 || y < 0 || x > this->boundingRect().width() || y > this->boundingRect().height())
            return;

        QRect rect = QRect(x, y, static_cast<int>(iconHeight), static_cast<int>(iconHeight));

        painter->drawRect(rect);
    }
}

/**
 * @brief BrowserPage::render
 * 文档缩略图渲染接口
 * @param scaleFactor 缩放系数
 * @param rotation 旋转角度
 * @param renderLater
 * @param force
 */
void BrowserPage::render(const double &scaleFactor, const Dr::Rotation &rotation, const bool &renderLater, const bool &force)
{
    if (nullptr == m_page)
        return;

    if (!force && renderLater && qFuzzyCompare(scaleFactor, m_scaleFactor) && rotation == m_rotation)
        return;

    m_pixmapHasRendered = false;

    if (m_lastClickIconAnnotation)
        m_lastClickIconAnnotation->setScaleFactor(scaleFactor);

    if (m_viewportRenderedRect.isValid()) {
        m_viewportRenderedRect = QRect();
        m_viewportPixmap = QPixmap();
    }

    m_scaleFactor = scaleFactor;

    if (m_rotation != rotation) {
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

//    if (m_wordHasRendered && !m_wordIsHide)
//        loadWords(false);

    if (!renderLater && !qFuzzyCompare(m_pixmapScaleFactor, m_scaleFactor)) {
        m_pixmapScaleFactor = m_scaleFactor;

        m_pixmapRenderedRect = QRect(0, 0, static_cast<int>(boundingRect().width()), 0);

        m_pixmapHasRendered = true;

        if (m_pixmap.isNull()) {
            m_pixmap = QPixmap(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
            m_pixmap.fill(Qt::white);
        } else
            m_pixmap = m_pixmap.scaled(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()), Qt::IgnoreAspectRatio);

        PageRenderThread::clearTask(this);

        QRectF rect = boundingRect();

        if ((rect.height() > 1000 || rect.width() > 1000) && (m_page->size().height() > 2000 || m_page->size().height() > 2000)) {
            int pieceWidth = 1000;
            int pieceHeight = 1000;

            QList<RenderPageTask> tasks;

            for (int i = 0 ; i * pieceHeight < rect.height(); ++i) {
                int height = pieceHeight;

                if (rect.height() < (i + 1) * pieceHeight)
                    height = static_cast<int>(rect.height() - pieceHeight * i);

                QRect renderRect = QRect(0, pieceHeight * i, static_cast<int>(boundingRect().width()), height);

                for (int j = 0; j * pieceWidth < renderRect.width(); ++j) {
                    int width = pieceWidth;

                    if (renderRect.width() < (j + 1) * pieceWidth)
                        width = static_cast<int>(renderRect.width() - pieceWidth * j);

                    QRect finalRenderRect = QRect(pieceWidth * j, pieceHeight * i, width, height);

                    RenderPageTask task;
                    task.item = this;
                    task.scaleFactor = m_scaleFactor;
                    task.rotation = Dr::RotateBy0;
                    task.renderRect = finalRenderRect;
                    tasks.append(task);
                }
            }
            PageRenderThread::appendTasks(tasks);
        } else {
            RenderPageTask task;
            task.item = this;
            task.scaleFactor = m_scaleFactor;
            task.rotation = Dr::RotateBy0;
            task.renderRect = QRect(static_cast<int>(rect.x()), static_cast<int>(rect.y()),
                                    static_cast<int>(rect.width()), static_cast<int>(rect.height()));
            PageRenderThread::appendTask(task);
        }

        loadAnnotations();
    }
}

/**
 * @brief BrowserPage::handleRenderFinished
 * 渲染缩略图
 * @param scaleFactor 缩放系数
 * @param image 缩略图
 * @param rect 范围
 */
void BrowserPage::handleRenderFinished(const double &scaleFactor, const QImage &image, const QRect &rect)
{
    if (!qFuzzyCompare(scaleFactor, m_pixmapScaleFactor))
        return;

    if (rect.height() == static_cast<int>(boundingRect().height())) {
        m_pixmap = QPixmap::fromImage(image);
    } else {
        QPainter painter(&m_pixmap);
        painter.drawImage(rect, image);
    }

    m_pixmapRenderedRect.setHeight(rect.y() + rect.height());

    emit m_parent->sigPartThumbnailUpdated(m_index);

    update();
}

void BrowserPage::renderViewPort(bool force)
{
    m_viewportTryRender = true;

    if (nullptr == m_parent)
        return;

    if (!force && boundingRect().width() < 2000 && boundingRect().height() < 2000)
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

    //如果现在已经加载的rect包含viewRender 就不加入任务
    if (!force && m_pixmapRenderedRect.contains(viewRenderRect))
        return;

    //如果只是当前视图区域变小则不加载
//    if (!force && m_viewportRenderedRect.contains(viewRenderRect))
//        return;

    RenderViewportTask task;

    task.page = this;

    task.scaleFactor = m_scaleFactor;

    task.rotation = Dr::RotateBy0;

    task.renderRect = viewRenderRect;

    PageViewportThread::appendTask(task);
}

void BrowserPage::handleViewportRenderFinished(const double &scaleFactor, const QImage &image, const QRect &rect)
{
    if (!qFuzzyCompare(scaleFactor, m_pixmapScaleFactor))
        return;

    m_viewportScaleFactor = scaleFactor;

    m_viewportPixmap = QPixmap::fromImage(image);

    m_viewportRenderedRect = rect;

    QPainter painter(&m_pixmap);

    painter.drawImage(rect, image);

    update();
}

/**
 * brief BrowserPage::handleWordLoaded
 * 控制文字加载
 * @param words 要加载的文字
 */
void BrowserPage::handleWordLoaded(const QList<Word> &words)
{
    m_wordIsRendering = false;

    if (m_wordHasRendered)
        return;

    for (int i = 0; i < words.count(); ++i) {
        BrowserWord *word = new BrowserWord(this, words[i]);
        word->setSelectable(m_wordSelectable);
        m_words.append(word);
    }

    m_wordHasRendered = true;

    loadWords();
}

QImage BrowserPage::getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect, int renderIndex)
{
    deepin_reader::Page *page = m_page;

    if (-1 != renderIndex && m_renderPages.count() > renderIndex)
        page = m_renderPages.value(renderIndex);

    if (nullptr == page)
        return QImage();

    return page->render(rotation, scaleFactor, boundingRect);
}

QImage BrowserPage::getImage(int width, int height, Qt::AspectRatioMode mode, bool bSrc, int renderIndex)
{
    deepin_reader::Page *page = m_page;

    if (-1 != renderIndex)
        page = m_renderPages.value(renderIndex);

    if (nullptr == page)
        return QImage();

    if (bSrc) {
        if (m_pixmap.isNull())
            return QImage();

        QImage image = m_pixmap.toImage().scaled(static_cast<int>(width * dApp->devicePixelRatio()), static_cast<int>(height * dApp->devicePixelRatio()), mode, Qt::SmoothTransformation);
        image.setDevicePixelRatio(dApp->devicePixelRatio());
        return image;
    }

    QSizeF size = page->sizeF().scaled(static_cast<int>(width * dApp->devicePixelRatio()), static_cast<int>(height * dApp->devicePixelRatio()), mode);

    QImage image = page->render(static_cast<int>(size.width()), static_cast<int>(size.height()), mode);

    image.setDevicePixelRatio(dApp->devicePixelRatio());

    return image;
}

/**
 * @brief BrowserPage::thumbnail
 * 文档页的缩略图
 * @return 缩略图
 */
QImage BrowserPage::thumbnail()
{
    return m_page->thumbnail();
}

/**
 * @brief BrowserPage::getImageRect
 * 得到文档缩放之后的图
 * @param scaleFactor 缩放系数
 * @param rect 范围
 * @return  文档缩放之后的图
 */
QImage BrowserPage::getImageRect(double scaleFactor, QRect rect)
{
    return m_page->render(m_rotation, scaleFactor, rect);
}

/**
 * @brief BrowserPage::getImagePoint
 * 得到鼠标点周围的片图(放大镜)
 * @param scaleFactor 缩放比例
 * @param point 鼠标点
 * @return鼠标点周围的片图(带缩放)
 */
QImage BrowserPage::getImagePoint(double scaleFactor, QPoint point)
{
    const QPoint &transformPoint = translatePoint(point);
    int ss = static_cast<int>(122 * scaleFactor / m_scaleFactor);
    QRect rect = QRect(qRound(transformPoint.x() * scaleFactor / m_scaleFactor - ss / 2.0), qRound(transformPoint.y() * scaleFactor / m_scaleFactor - ss / 2.0), ss, ss);
    return m_page->render(m_rotation, scaleFactor, rect);
}

/**
 * @brief BrowserPage::getCurImagePoint
 * 得到鼠标点周围的片图(放大镜)
 * @param point 鼠标点
 * @return 鼠标点周围的片图
 */
QImage BrowserPage::getCurImagePoint(QPoint point)
{
    int ds = 122;
    QTransform transform;
    transform.rotate(m_rotation * 90);
    QImage image = Utils::copyImage(m_pixmap.toImage(), qRound(point.x() - ds / 2.0), qRound(point.y() - ds / 2.0), ds, ds).transformed(transform, Qt::SmoothTransformation);
    image.setDevicePixelRatio(dApp->devicePixelRatio());
    return image;
}

/**
 * @brief BrowserPage::getWords
 * 得到文档无旋转下的左右文字
 * @return 文字列表
 */
QList<Word> BrowserPage::getWords()
{
    if (nullptr == m_page)
        return QList<Word>();

    return m_page->words(Dr::RotateBy0);
}

/**
 * @brief BrowserPage::existInstance
 * 页是否存在
 * @param item 哪一页
 * @return
 */
bool BrowserPage::existInstance(BrowserPage *item)
{
    return items.contains(item);
}

/**
 * @brief BrowserPage::setItemIndex
 * 设置文档页的编号
 * @param itemIndex 编号
 */
void BrowserPage::setItemIndex(int itemIndex)
{
    m_index = itemIndex;
}

/**
 * @brief BrowserPage::itemIndex
 * 当前页的编号(从0开始)
 * @return
 */
int BrowserPage::itemIndex()
{
    return m_index;
}

/**
 * @brief BrowserPage::selectedWords
 * 得到当前选择文字内容
 * @return  选择文字的内容
 */
QString BrowserPage::selectedWords()
{
    QString text;
    foreach (BrowserWord *word, m_words) {
        if (word->isSelected()) {
            text += word->text();
        }
    }

    return text;
}

/**
 * @brief BrowserPage::setWordSelectable
 * 设置文字是否可选择
 * @param selectable true:可选   false:不可选
 */
void BrowserPage::setWordSelectable(bool selectable)
{
    m_wordSelectable = selectable;
    foreach (BrowserWord *word, m_words) {
        word->setSelectable(selectable);
    }
}

/**
 * @brief BrowserPage::loadLinks
 * 加载注释
 */
void BrowserPage::loadLinks()
{
    QList< Link * > links = m_page->links();
    for (int i = 0; i < links.count(); ++i) {
        BrowserLink *link = new BrowserLink(this, links[i]);
        m_linkItems.append(link);
    }
}

/**
 * @brief BrowserPage::loadAnnotations
 * 加载注释
 */
void BrowserPage::loadAnnotations()
{
    if (!m_hasLoadedAnnotation)
        reloadAnnotations();
}

void BrowserPage::loadWords()
{
    m_wordNeeded = true;

    if (m_wordIsRendering)
        return;

    if (m_wordHasRendered) {
        //如果已经加载则取消隐藏和改变大小
        if (m_words.count() <= 0)
            return;

        prepareGeometryChange();

        if (!qFuzzyCompare(m_wordScaleFactor, m_scaleFactor)) {
            m_wordScaleFactor = m_scaleFactor;
            foreach (BrowserWord *word, m_words) {
                word->setScaleFactor(m_scaleFactor);
            }
        }
        return;
    }

    RenderPageTask task;
    task.type = RenderPageTask::word;
    task.item = this;
    PageRenderThread::appendTask(task);

    m_wordIsRendering = true;
}

/**
 * @brief BrowserPage::clearCache
 *  删除缓存图片
 */
void BrowserPage::clearCache()
{
    m_pixmap = QPixmap();
    m_pixmapHasRendered = false;
    m_pixmapScaleFactor = -1;
    m_pixmapRenderedRect = QRect();

    m_viewportPixmap = QPixmap();
    m_viewportRenderedRect = QRect();
    m_viewportScaleFactor = -1;

    PageRenderThread::clearTask(this);
}

/**
 * @brief BrowserPage::clearWords
 *  删除缓存文字
 */
void BrowserPage::clearWords()
{
    if (!m_wordHasRendered)
        return;

    foreach (BrowserWord *word, m_words) {
        if (word->isSelected())
            return;
    }

    prepareGeometryChange();

    m_wordHasRendered = false;
    m_wordNeeded = false;

    QList<BrowserWord *> t_word = m_words;
    m_words.clear();

    foreach (BrowserWord *word, t_word) {
        word->setParentItem(nullptr);
        scene()->removeItem(word);
        delete word;
    }
}

/**
 * @brief BrowserPage::scaleWords
 *  改变文字缩放
 */
void BrowserPage::scaleWords()
{
    if (!m_wordHasRendered || m_words.count() <= 0)
        return;

    prepareGeometryChange();

    if (!qFuzzyCompare(m_wordScaleFactor, m_scaleFactor)) {
        m_wordScaleFactor = m_scaleFactor;
        foreach (BrowserWord *word, m_words) {
            word->setScaleFactor(m_scaleFactor);
        }
    }
}

/**
 * @brief BrowserPage::reloadAnnotations
 * 重新加载页中所有注释
 */
void BrowserPage::reloadAnnotations()
{
    qDeleteAll(m_annotations);

    qDeleteAll(m_annotationItems);
    m_annotationItems.clear();

    m_annotations = m_page->annotations();
    if (m_renderPages.count() >= 4) {
        qDeleteAll(m_annotations0);
        m_annotations0 = m_renderPages[0]->annotations();

        qDeleteAll(m_annotations1);
        m_annotations1 = m_renderPages[1]->annotations();

        qDeleteAll(m_annotations2);
        m_annotations2 = m_renderPages[2]->annotations();

        qDeleteAll(m_annotations3);
        m_annotations3 = m_renderPages[3]->annotations();
    }

    for (int i = 0; i < m_annotations.count(); ++i) {
        m_annotations[i]->page = m_index + 1;
        if (m_annotations[i]->uniqueName().isEmpty()) {
            m_annotations[i]->setUniqueName(QUuid::createUuid().toString());
        }

        foreach (QRectF rect, m_annotations[i]->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, m_annotations[i]);
            m_annotationItems.append(annotationItem);
        }
    }

    m_hasLoadedAnnotation = true;
}

/**
 * @brief BrowserPage::annotations
 * 当前页中所有注释
 * @return 注释列表
 */
QList<deepin_reader::Annotation *> BrowserPage::annotations()
{
    return m_annotations;
}

/**
 * @brief BrowserPage::updateAnnotation
 * 更新注释内容,颜色
 * @param annotation 当前注释
 * @param text 注释内容
 * @param color 注释高亮颜色
 * @return true:更新成功  false:更新失败
 */
bool BrowserPage::updateAnnotation(deepin_reader::Annotation *annotation, const QString text, const QColor color)
{
    if (nullptr == annotation)
        return false;

    if (!m_annotations.contains(annotation))
        return false;

    int updateIndex = m_annotations.indexOf(annotation);

    if (m_annotations0.count() > updateIndex && m_annotations1.count() > updateIndex &&
            m_annotations2.count() > updateIndex && m_annotations3.count() > updateIndex) {
        m_annotations0[updateIndex]->updateAnnotation(text, color);
        m_annotations1[updateIndex]->updateAnnotation(text, color);
        m_annotations2[updateIndex]->updateAnnotation(text, color);
        m_annotations3[updateIndex]->updateAnnotation(text, color);
    }

    if (!annotation->updateAnnotation(text, color))
        return false;

    updatePageFull();

    return true;
}

/**
 * @brief BrowserPage::addHighlightAnnotation
 * 添加文本高亮注释接口
 * @param text  高亮注释内容
 * @param color 高亮注释颜色
 * @return      当前添加高亮注释
 */
Annotation *BrowserPage::addHighlightAnnotation(QString text, QColor color)
{
    if (nullptr == m_page)
        return nullptr;

    Annotation *highLightAnnot{nullptr};
    QList<QRectF> boundarys;
    QRectF rect;
    QRectF recboundary;
    int index{0};
    qreal curwidth = m_page->sizeF().width();
    qreal curheight = m_page->sizeF().height();

    //加载文档文字无旋转情况下的文字(即旋转0度时的所有文字)
    QList<deepin_reader::Word> twords = m_page->words(Dr::RotateBy0);

    for (index = 0; index < m_words.count(); index++) {
        if (m_words.at(index) && m_words.at(index)->isSelected()) {
            m_words.at(index)->setSelected(false);
            if (index >= 0 && index < twords.count()) {
                deepin_reader::Word tword = twords.at(index);

                rect = tword.wordBoundingRect();

                recboundary.setTopLeft(QPointF(rect.left() / curwidth,
                                               rect.top() / curheight));
                recboundary.setTopRight(QPointF(rect.right() / curwidth,
                                                rect.top() / curheight));
                recboundary.setBottomLeft(QPointF(rect.left() / curwidth,
                                                  rect.bottom() / curheight));
                recboundary.setBottomRight(QPointF(rect.right() / curwidth,
                                                   rect.bottom() / curheight));
                boundarys << recboundary;
            }
        }
    }

    if (boundarys.count()) {
        loadAnnotations();

        highLightAnnot = m_page->addHighlightAnnotation(boundarys, text, color);
        if (highLightAnnot == nullptr)
            return nullptr;
        highLightAnnot->page = m_index + 1;
        m_annotations.append(highLightAnnot);

        if (m_renderPages.count() == 4) {
            Annotation *highLightAnnotOther{nullptr};
            for (int index = 0; index < 4; index++) {
                highLightAnnotOther = m_renderPages[index]->addHighlightAnnotation(boundarys, text, color);
                if (0 == index) {
                    m_annotations0.append(highLightAnnotOther);
                } else if (1 == index) {
                    m_annotations1.append(highLightAnnotOther);
                } else if (2 == index) {
                    m_annotations2.append(highLightAnnotOther);
                } else {
                    m_annotations3.append(highLightAnnotOther);
                }
                highLightAnnotOther = nullptr;
            }
        }

        foreach (QRectF rect, highLightAnnot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, highLightAnnot);
            m_annotationItems.append(annotationItem);
        }
    }

    updatePageFull();

    return highLightAnnot;
}

/**
 * @brief BrowserPage::hasAnnotation
 * 当前页注释中是否有这个注释
 * @param annotation 当前注释
 * @return true:有       false:没有
 */
bool BrowserPage::hasAnnotation(deepin_reader::Annotation *annotation)
{
    return m_annotations.contains(annotation);
}

/**
 * @brief BrowserPage::setSelectIconRect
 * 设置图标注释选择框
 * @param draw true:绘制  false:取消绘制
 * @param iconAnnot 图标注释
 */
void BrowserPage::setSelectIconRect(const bool draw, Annotation *iconAnnot)
{
    QList<QRectF> rectList;

    if (iconAnnot) {
        foreach (BrowserAnnotation *annotation, m_annotationItems) {
            if (annotation && annotation->isSame(iconAnnot)) {
                m_lastClickIconAnnotation = annotation;
                if (iconAnnot->type() == 1)
                    m_lastClickIconAnnotation->setDrawSelectRect(draw);
                m_lastClickIconAnnotation->setScaleFactor(m_scaleFactor);
            }
        }
    } else {
        if (m_lastClickIconAnnotation)
            m_lastClickIconAnnotation->setDrawSelectRect(draw);
    }
}

/**
 * @brief BrowserPage::setDrawMoveIconRect
 * 绘制图标注释选择框
 * @param draw true:绘制  false:取消绘制
 */
void BrowserPage::setDrawMoveIconRect(const bool draw)
{
    m_drawMoveIconRect = draw;

    update();
}

/**
 * @brief BrowserPage::setIconMovePos
 * 图标注释选择框移动位置
 * @param movePoint 移动位置
 */
void BrowserPage::setIconMovePos(const QPointF movePoint)
{
    m_drawMoveIconPoint = movePoint;

    update();
}

/**
 * @brief BrowserPage::translate2LocalPos
 * 将非0度下的坐标转换成0度坐标
 * @param point 坐标
 * @return 0度坐标
 */
QPointF BrowserPage::translate2LocalPos(const QPointF point)
{
    QPointF tPoint = point;

    switch (m_rotation) {
    case Dr::RotateBy90: {
        tPoint = QPointF(point.y(), this->bookmarkRect().width() - point.x());
    }
    break;
    case Dr::RotateBy180: {
        tPoint = QPointF(this->bookmarkRect().width() - point.x(), this->boundingRect().height() - point.y());
    }
    break;
    case Dr::RotateBy270: {
        tPoint = QPointF(this->boundingRect().height() - point.y(), point.x());
    }
    break;
    default: break;
    }

    return tPoint;
}

/**
 * @brief BrowserPage::deleteNowSelectIconAnnotation
 * 删除当前选中的图标注释
 * @return
 */
QString BrowserPage::deleteNowSelectIconAnnotation()
{
    if (nullptr == m_lastClickIconAnnotation)
        return "";

    QString iconAnnotationContains{""};

    m_lastClickIconAnnotation->setDrawSelectRect(false);

    iconAnnotationContains = m_lastClickIconAnnotation->annotationText();

    removeAnnotation(m_lastClickIconAnnotation->annotation());

    m_lastClickIconAnnotation = nullptr;

    return iconAnnotationContains;
}

/**
 * @brief BrowserPage::moveIconAnnotation
 * 移动注释图标位置
 * @return true:移动成功   false:移动失败
 */
bool BrowserPage::moveIconAnnotation(const QRectF moveRect)
{
    if (nullptr == m_page || nullptr == m_lastClickIconAnnotation)
        return false;


    Annotation *annot{nullptr};
    QString containtStr = m_lastClickIconAnnotation->annotationText();

    int annotIndex = m_annotations.indexOf(m_lastClickIconAnnotation->annotation());

    m_annotationItems.removeAll(m_lastClickIconAnnotation);
    annot = m_page->moveIconAnnotation(m_lastClickIconAnnotation->annotation(), moveRect);

    if (m_renderPages.count() == 4) {
        m_renderPages[0]->moveIconAnnotation(m_annotations0.at(annotIndex), moveRect);
        m_renderPages[1]->moveIconAnnotation(m_annotations1.at(annotIndex), moveRect);
        m_renderPages[2]->moveIconAnnotation(m_annotations2.at(annotIndex), moveRect);
        m_renderPages[3]->moveIconAnnotation(m_annotations3.at(annotIndex), moveRect);
    }

    if (annot && m_annotations.contains(annot)) {
        delete m_lastClickIconAnnotation;
        m_lastClickIconAnnotation = nullptr;
        annot->page = m_index + 1;
        foreach (QRectF rect, annot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, annot);
            m_annotationItems.append(annotationItem);
            m_lastClickIconAnnotation = annotationItem;
        }

        if (annot->type() == 1) {
            if (m_lastClickIconAnnotation) {
                m_lastClickIconAnnotation->setScaleFactor(m_scaleFactor);
                m_lastClickIconAnnotation->setDrawSelectRect(true);
            }
        }
    }

    updatePageFull();

    return true;
}

/**
 * @brief BrowserPage::removeAllAnnotation
 * 删除当前页中所有注释
 * @return ture:删除成功       false:删除失败
 */
bool BrowserPage::removeAllAnnotation()
{
    m_lastClickIconAnnotation = nullptr;

    if (m_annotations.isEmpty())
        return false;

    for (int index = 0; index < m_annotations.size(); index++) {
        deepin_reader::Annotation *annota = m_annotations.at(index);
        if (!m_annotations.contains(annota) || (annota && annota->contents().isEmpty()))
            continue;

        int annotIndex = m_annotations.indexOf(annota);

        if (m_renderPages.count() == 4) {
            m_renderPages[0]->removeAnnotation(m_annotations0.at(annotIndex));
            m_annotations0.removeAt(annotIndex);
            m_renderPages[1]->removeAnnotation(m_annotations1.at(annotIndex));
            m_annotations1.removeAt(annotIndex);
            m_renderPages[2]->removeAnnotation(m_annotations2.at(annotIndex));
            m_annotations2.removeAt(annotIndex);
            m_renderPages[3]->removeAnnotation(m_annotations3.at(annotIndex));
            m_annotations3.removeAt(annotIndex);
        }

        if (!m_page->removeAnnotation(annota))
            continue;

        m_annotations.removeAt(index);
        index--;

        foreach (BrowserAnnotation *annotation, m_annotationItems) {
            if (annotation && annotation->isSame(annota)) {
                if (m_lastClickIconAnnotation && m_lastClickIconAnnotation->isSame(annota))
                    m_lastClickIconAnnotation = nullptr;
                m_annotationItems.removeAll(annotation);
                delete annotation;
                annotation = nullptr;
            }
        }
    }

    m_hasLoadedAnnotation = false;

    updatePageFull();

    return true;
}

/**
 * @brief BrowserPage::jump2Link
 * 跳转到相应link处(如果是目录跳到文档正文处,如果是网址会在浏览器中打开)
 * @param point 当前鼠标点
 * @return true:跳转成功    false:跳转失败
 */
bool BrowserPage::jump2Link(const QPointF point)
{
    QPointF localPoint = point;

    if (nullptr == m_page)
        return false;

    QList<Link *> linkList = m_page->links();

    if (linkList.count() < 1)
        return false;

    foreach (Link *link, linkList) {
        if (link) {
            if (link->boundary.boundingRect().contains(localPoint)) {
                QString urlStr = link->urlOrFileName;
                if (urlStr.isEmpty()) {
                    int page = link->page;
                    if (m_parent) {
                        m_parent->setCurrentPage(page);
                        return true;
                    }
                } else {
                    QDesktopServices::openUrl(QUrl(urlStr, QUrl::TolerantMode));
                    return true;
                }
            }
        }
    }

    return false;
}

/**
 * @brief BrowserPage::inLink
 * 当前鼠标位置是否有link
 * @param pos 当前鼠标位置
 * @return true:有  false:没有
 */
bool BrowserPage::inLink(const QPointF pos)
{
    QPointF localPoint = pos;

    if (nullptr == m_page)
        return false;

    QList<Link *> linkList = m_page->links();

    if (linkList.count() < 1)
        return false;

    foreach (Link *link, linkList) {
        if (link) {
            if (link->boundary.boundingRect().contains(localPoint)) {
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief BrowserPage::setPageBookMark
 * 在书签附近文字时,有时添加/删除书签无效,特意在brower中先处理
 * @param clickPoint 鼠标点击位置
 */
void BrowserPage::setPageBookMark(const QPointF clickPoint)
{
    if (bookmarkMouseRect().contains(clickPoint)) {
        m_bookmarkState = 2;
        if (nullptr != m_parent) {
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
}

/**
 * @brief BrowserPage::removeAnnotation
 * 删除某个注释
 * @param annota 具体哪个注释
 * @return true:删除成功, false:删除失败
 */
bool BrowserPage::removeAnnotation(deepin_reader::Annotation *annota)
{
    if (nullptr == annota)
        return false;

    if (!m_annotations.contains(annota))
        return false;

    int annotIndex = m_annotations.indexOf(annota);

    m_annotations.removeAll(annota);

    if (m_renderPages.count() == 4) {
        m_renderPages[0]->removeAnnotation(m_annotations0.at(annotIndex));
        m_annotations0.removeAt(annotIndex);
        m_renderPages[1]->removeAnnotation(m_annotations1.at(annotIndex));
        m_annotations1.removeAt(annotIndex);
        m_renderPages[2]->removeAnnotation(m_annotations2.at(annotIndex));
        m_annotations2.removeAt(annotIndex);
        m_renderPages[3]->removeAnnotation(m_annotations3.at(annotIndex));
        m_annotations3.removeAt(annotIndex);
    }

    if (!m_page->removeAnnotation(annota))
        return false;

    foreach (BrowserAnnotation *annotation, m_annotationItems) {
        if (annotation && annotation->isSame(annota)) {
            if (m_lastClickIconAnnotation && m_lastClickIconAnnotation->isSame(annota))
                m_lastClickIconAnnotation = nullptr;
            m_annotationItems.removeAll(annotation);
            delete annotation;
            annotation = nullptr;
        }
    }

    updatePageFull();

    return true;
}

/**
 * @brief BrowserPage::removeAnnotationByUniqueName
 * 根据序列号删除注释
 * @param uniqueName 序列号
 * @return true:删除成功, false:删除失败
 */
bool BrowserPage::removeAnnotationByUniqueName(QString uniqueName)
{
    foreach (deepin_reader::Annotation *annotation, m_annotations) {
        if (annotation->uniqueName() == uniqueName) {
            return removeAnnotation(annotation);
        }
    }

    return false;
}

/**
 * @brief BrowserPage::addIconAnnotation
 * 添加图标注释
 * @param rect 图标rect
 * @param text 注释内容
 * @return 添加之后的注释,添加失败返回nullptr,反之不为nullptr
 */
Annotation *BrowserPage::addIconAnnotation(const QRectF rect, const QString text)
{
    if (nullptr == m_page)
        return nullptr;

    Annotation *annot{nullptr};

    annot = m_page->addIconAnnotation(rect, text);

    if (annot) {
        annot->page = m_index + 1;

        m_annotations.append(annot);

        foreach (QRectF rect, annot->boundary()) {
            BrowserAnnotation *annotationItem = new BrowserAnnotation(this, rect, annot);
            m_annotationItems.append(annotationItem);
            m_lastClickIconAnnotation = annotationItem;
        }

        if (annot->type() == 1) {
            if (m_lastClickIconAnnotation) {
                m_lastClickIconAnnotation->setScaleFactor(m_scaleFactor);
                m_lastClickIconAnnotation->setDrawSelectRect(true);
            }
        }
    }

    if (m_renderPages.count() == 4) {
        Annotation *annotOther{nullptr};
        for (int index = 0; index < 4; index++) {
            annotOther = m_renderPages[index]->addIconAnnotation(rect, text);
            if (0 == index) {
                m_annotations0.append(annotOther);
            } else if (1 == index) {
                m_annotations1.append(annotOther);
            } else if (2 == index) {
                m_annotations2.append(annotOther);
            } else {
                m_annotations3.append(annotOther);
            }
            annotOther = nullptr;
        }
    }

    updatePageFull();

    return annot;
}

/**
 * @brief BrowserPage::mouseClickIconAnnot
 * 当前鼠标点击位置是否有注释
 * @param clickPoint 鼠标点击位置
 * @return true:有  false:没有
 */
bool BrowserPage::mouseClickIconAnnot(QPointF &clickPoint)
{
    if (nullptr == m_page)
        return false;

    return m_page->mouseClickIconAnnot(clickPoint);
}

/**
 * @brief BrowserPage::translate2NormalPoint
 * 将文字范围转换成文档旋转0度范围(暂时弃用)
 */
void BrowserPage::translate2NormalRect(QRectF &wordRect)
{
    QPointF rectPoint = QPointF(wordRect.x(), wordRect.y());
    qreal wordWidth = wordRect.width();
    qreal wordHeight = wordRect.height();

    qreal pageWidth = this->boundingRect().width();
    qreal pageHeight = this->boundingRect().height();

    switch (m_rotation) {
    case Dr::RotateBy0:
        break;
    case Dr::RotateBy90: {
        rectPoint = QPointF(rectPoint.y(), pageWidth - rectPoint.x());
        wordRect.setWidth(wordHeight);
        wordRect.setHeight(wordWidth);
        wordRect.setX(rectPoint.x());
        wordRect.setY(rectPoint.y());
    }
    break;
    case Dr::RotateBy180: {
        rectPoint = QPointF(pageWidth - rectPoint.x(), pageHeight - rectPoint.y());
    }
    break;
    case Dr::RotateBy270: {
        rectPoint = QPointF(pageHeight - rectPoint.y(),  rectPoint.x());
        wordRect.setX(rectPoint.x());
        wordRect.setY(rectPoint.y());
        wordRect.setWidth(wordHeight);
        wordRect.setHeight(wordWidth);
    }
    break;
    default: break;
    }
}

/**
 * @brief BrowserPage::sceneEvent
 *  画布事件,目前只处理HoverMove事件
 * @param event
 * @return
 */
bool BrowserPage::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneHoverMove) {
//        qInfo() << __LINE__ << __FUNCTION__ << "      current page index :  " << this->m_index;
        QGraphicsSceneHoverEvent *moveevent = dynamic_cast<QGraphicsSceneHoverEvent *>(event);
        if (!m_bookmark && bookmarkMouseRect().contains(moveevent->pos()))
            m_bookmarkState = 1;
        else if (m_bookmark)
            m_bookmarkState = 3;
        else
            m_bookmarkState = 0;
        update();
    } else if (event->type() == QEvent::HideToParent) {
        qDebug() << 11111111111;
    }
    return QGraphicsItem::sceneEvent(event);
}

/**
 * @brief BrowserPage::setSearchHighlightRectf
 * 设置当前搜索选择框,在搜索列表中
 * @param rectflst
 */
void BrowserPage::setSearchHighlightRectf(const QList< QRectF > &rectflst)
{
    if (rectflst.size() > 0) {
        if (m_parent->currentPage() == this->itemIndex() + 1)
            m_searchSelectLighRectf = rectflst.first();
        m_searchLightrectLst = rectflst;
        update();
    }
}

/**
 * @brief BrowserPage::clearSearchHighlightRects
 *结束搜索,清除搜索选择框,清空搜索结果列表
 */
void BrowserPage::clearSearchHighlightRects()
{
    m_searchSelectLighRectf = QRectF(0, 0, 0, 0);
    m_searchLightrectLst.clear();
    update();
}

/**
 * @brief BrowserPage::clearSelectSearchHighlightRects
 * 清除高亮选择框,在搜索结果中
 */
void BrowserPage::clearSelectSearchHighlightRects()
{
    m_searchSelectLighRectf = QRectF(0, 0, 0, 0);
    update();
}

/**
 * @brief BrowserPage::searchHighlightRectSize
 * 当前页中搜索到目标的个数
 * @return 目标的个数
 */
int BrowserPage::searchHighlightRectSize()
{
    return m_searchLightrectLst.size();
}

/**
 * @brief BrowserPage::findSearchforIndex
 * 得到搜索元素的rect,根据编号
 * @param index 编号
 * @return 编号对应的rect
 */
QRectF BrowserPage::findSearchforIndex(int index)
{
    if (index >= 0 && index < m_searchLightrectLst.size()) {
        m_searchSelectLighRectf = m_searchLightrectLst[index];
        update();
        return m_searchSelectLighRectf;
    }
    return QRectF(-1, -1, -1, -1);
}

/**
 * @brief BrowserPage::getNorotateRecr
 * 得到没有旋转rect
 * @param rect
 * @return
 */
QRectF BrowserPage::getNorotateRecr(const QRectF &rect)
{
    QRectF newrect;
    newrect.setX(rect.x()*m_scaleFactor);
    newrect.setY(rect.y()*m_scaleFactor);
    newrect.setWidth(rect.width()*m_scaleFactor);
    newrect.setHeight(rect.height()*m_scaleFactor);
    return newrect;
}

/**
 * @brief BrowserPage::translatePoint
 * 将非0度(旋转角度)坐标点转换成0度坐标点
 * @param point 转换之前坐标点
 * @return 转换之后坐标点
 */
QPoint BrowserPage::translatePoint(const QPoint &point)
{
    QPoint newpoint = point;
    switch (m_rotation) {
    case Dr::RotateBy90: {
        newpoint.setX(static_cast<int>(boundingRect().height() - point.y()));
        newpoint.setY(point.x());
        break;
    }
    case Dr::RotateBy180: {
        newpoint.setX(static_cast<int>(boundingRect().width() - point.x()));
        newpoint.setY(static_cast<int>(boundingRect().height() - point.y()));
        break;
    }
    case Dr::RotateBy270: {
        newpoint.setX(point.y());
        newpoint.setY(static_cast<int>(boundingRect().width() - point.x()));
        break;
    }
    default:
        break;
    }
    return  newpoint;
}

/**
 * @brief BrowserPage::translateRect
 * 将非0度(旋转角度)rect转换成0度rect
 * @param rect 转换之前rect
 * @return 转换之后rect
 */
QRectF BrowserPage::translateRect(const QRectF &rect)
{
    //旋转角度逆时针增加
    QRectF newrect;
    switch (m_rotation) {
    case Dr::RotateBy0: {
        newrect.setX(rect.x()*m_scaleFactor);
        newrect.setY(rect.y()*m_scaleFactor);
        newrect.setWidth(rect.width()*m_scaleFactor);
        newrect.setHeight(rect.height()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy90: {
        newrect.setX((m_page->sizeF().height() - rect.y() - rect.height())*m_scaleFactor - boundingRect().height());
        newrect.setY(rect.x()*m_scaleFactor);
        newrect.setWidth(rect.height()*m_scaleFactor);
        newrect.setHeight(rect.width()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy180: {
        newrect.setX((m_page->sizeF().width() - rect.x() - rect.width())*m_scaleFactor - boundingRect().width());
        newrect.setY((m_page->sizeF().height() - rect.y() - rect.height())*m_scaleFactor - boundingRect().height());
        newrect.setWidth(rect.width()*m_scaleFactor);
        newrect.setHeight(rect.height()*m_scaleFactor);
        break;
    }
    case Dr::RotateBy270: {
        newrect.setX(rect.y()*m_scaleFactor);
        newrect.setY((m_page->sizeF().width() - rect.x() - rect.width())*m_scaleFactor - boundingRect().width());
        newrect.setWidth(rect.height()*m_scaleFactor);
        newrect.setHeight(rect.width()*m_scaleFactor);
        break;
    }
    default:
        break;
    }
    return  newrect;
}

/**
 * @brief BrowserPage::getBrowserAnnotation
 * 获得注释,根据坐标点
 * @param point 坐标点
 * @return 坐标点下的注释
 */
BrowserAnnotation *BrowserPage::getBrowserAnnotation(const QPoint &point)
{
    BrowserAnnotation *item = nullptr;
    const QList<QGraphicsItem *> &itemlst = scene()->items(this->mapToScene(point));
    for (QGraphicsItem *itemIter : itemlst) {
        item = dynamic_cast<BrowserAnnotation *>(itemIter);
        if (item != nullptr) {
            return item;
        }
    }

    return nullptr;
}

/**
 * @brief BrowserPage::getBrowserWord
 * 根据坐标点获得当前的文字
 * @param point 当前坐标点
 * @return 坐标点下的文字
 */
BrowserWord *BrowserPage::getBrowserWord(const QPoint &point)
{
    BrowserWord *item = nullptr;
    const QList<QGraphicsItem *> &itemlst = scene()->items(this->mapToScene(point));
    for (QGraphicsItem *itemIter : itemlst) {
        item = dynamic_cast<BrowserWord *>(itemIter);
        if (item != nullptr) {
            return item;
        }
    }

    return nullptr;
}

/**
 * @brief BrowserPage::updatePageFull
 * 更新整页
 */
void BrowserPage::updatePageFull()
{
    //m_pixmap = QPixmap();

    m_pixmapScaleFactor = -1;

    render(m_scaleFactor, m_rotation, true, true);

    renderViewPort(false);
}
