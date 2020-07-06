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
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
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
#include "SheetBrowserPDFL.h"
#include "document/PDFModel.h"
#include "SheetBrowserPDFLItem.h"
#include "DocSheetPDFL.h"
#include "SheetBrowserPDFLWord.h"
#include "SheetBrowserPDFLAnnotation.h"
#include "widgets/TipsWidget.h"
#include "SheetBrowserPdflMenu.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>
#include <QBitmap>
#include <DMenu>
#include <DGuiApplicationHelper>

DWIDGET_USE_NAMESPACE

SheetBrowserPDFL::SheetBrowserPDFL(DocSheetPDFL *parent) : DGraphicsView(parent), m_sheet(parent)
{
    setScene(new QGraphicsScene());

    setFrameShape(QFrame::NoFrame);

    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("QGraphicsView{background-color:white}");     //由于DTK bug不响应WA_TranslucentBackground参数写死了颜色，这里加样式表让dtk style失效

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScrollBarValueChanged(int)));

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onHorizontalScrollBarValueChanged(int)));

    m_tipsWidget = new TipsWidget(this);
}

SheetBrowserPDFL::~SheetBrowserPDFL()
{
    qDeleteAll(m_items);

    if (nullptr != m_document)
        delete m_document;
}

bool SheetBrowserPDFL::openFilePath(const QString &filePath)
{
    m_document = deepin_reader::PDFDocument::loadDocument(filePath);

    if (nullptr == m_document)
        return false;

    return true;
}

bool SheetBrowserPDFL::loadPages(DocOperation &operation, const QSet<int> &bookmarks)
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

        SheetBrowserPDFLItem *item = new SheetBrowserPDFLItem(this, page);
        item->setItemIndex(i);

        if (bookmarks.contains(i))
            item->setBookmark(true);

        m_items.append(item);
        scene()->addItem(item);
    }

    setMouseShape(operation.mouseShape);
    deform(operation);
    m_initPage = operation.currentPage;
    m_hasLoaded = true;

    return true;
}

void SheetBrowserPDFL::setMouseShape(const Dr::MouseShape &shape)
{
    if (Dr::MouseShapeHand == shape) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        foreach (SheetBrowserPDFLItem *item, m_items) {
            item->setWordSelectable(false);
        }
    } else if (Dr::MouseShapeNormal == shape) {
        setDragMode(QGraphicsView::RubberBandDrag);
        foreach (SheetBrowserPDFLItem *item, m_items)
            item->setWordSelectable(true);
    }
}

void SheetBrowserPDFL::setBookMark(int index, int state)
{
    if (m_items.count() > index) {
        m_items.at(index)->setBookmark(state);
    }
}

void SheetBrowserPDFL::onVerticalScrollBarValueChanged(int)
{
    wordsChangedLater();

    QList<QGraphicsItem *> items = scene()->items(mapToScene(this->rect()));

    foreach (QGraphicsItem *item, items) {
        SheetBrowserPDFLItem *pdfItem = static_cast<SheetBrowserPDFLItem *>(item);

        if (!m_items.contains(pdfItem))
            continue;

        if (nullptr == pdfItem)
            continue;

        pdfItem->renderViewPort();
    }

    emit sigPageChanged(currentPage());
}

void SheetBrowserPDFL::onHorizontalScrollBarValueChanged(int value)
{
    QList<QGraphicsItem *> items = scene()->items(mapToScene(this->rect()));

    foreach (QGraphicsItem *item, items) {
        SheetBrowserPDFLItem *pdfItem = static_cast<SheetBrowserPDFLItem *>(item);

        if (!m_items.contains(pdfItem))
            continue;

        if (nullptr == pdfItem)
            continue;

        pdfItem->renderViewPort();
    }
}

void SheetBrowserPDFL::onWordsChanged()
{
    //改变当前可读取字体
    QList<QGraphicsItem *> items = scene()->items(mapToScene(this->rect()));
    foreach (QGraphicsItem *item, items) {
        if (item->data(0).toString() != "SheetBrowserPDFLItem")
            continue;

        SheetBrowserPDFLItem *pdfItem = static_cast<SheetBrowserPDFLItem *>(item);

        if (!m_items.contains(pdfItem))
            continue;

        if (nullptr == pdfItem)
            continue;

        pdfItem->loadWords();
    }
}

void SheetBrowserPDFL::popMenu(const QPoint &point)
{
    closeMagnifier();

    SheetBrowserPDFLItem *item  = static_cast<SheetBrowserPDFLItem *>(itemAt(QPoint(point)));
    if (nullptr == item)
        return;

    int index = m_items.indexOf(item);

    QMenu menu(this);
    if (m_sheet->hasBookMark(index))
        menu.addAction(tr("Remove bookmark"), [this, index]() {
        m_sheet->setBookMark(index, false);
    });
    else
        menu.addAction(tr("Add bookmark"), [this, index]() {
        m_sheet->setBookMark(index, true);
    });

    menu.addSeparator();

    QAction *firstPageAction = new QAction("First Page", &menu);
    connect(firstPageAction, &QAction::triggered, [this]() {
        this->emit sigNeedPageFirst();
    });
    menu.addAction(firstPageAction);
    if (index == 0) {
        firstPageAction->setDisabled(true);
    }

    QAction *previousPageAction = new QAction("Previous Page", &menu);
    connect(previousPageAction, &QAction::triggered, [this]() {
        this->emit sigNeedPagePrev();
    });
    menu.addAction(previousPageAction);
    if (index == 0) {
        previousPageAction->setDisabled(true);
    }

    QAction *nextPageAction = new QAction("Next Page", &menu);
    connect(nextPageAction, &QAction::triggered, [this]() {
        this->emit sigNeedPageNext();
    });
    menu.addAction(nextPageAction);
    if (index == m_items.count() - 1) {
        nextPageAction->setDisabled(true);
    }

    QAction *lastPageAction = new QAction("Last Page", &menu);
    connect(lastPageAction, &QAction::triggered, [this]() {
        this->emit sigNeedPageLast();
    });
    menu.addAction(lastPageAction);
    if (index == m_items.count() - 1) {
        lastPageAction->setDisabled(true);
    }

    menu.move(mapToGlobal(point));
    menu.exec();
}

QString SheetBrowserPDFL::selectedWordsText()
{
    QString text;
    foreach (SheetBrowserPDFLItem *item, m_items)
        text += item->selectedWords();

    return text;
}

void SheetBrowserPDFL::addHighlightAnnotation(QString text, QColor color)
{
    foreach (SheetBrowserPDFLItem *item, m_items)
        item->addHighlightAnnotation(text, color);
}

void SheetBrowserPDFL::removeAnnotation(SheetBrowserPDFLAnnotation *annotation)
{

}

void SheetBrowserPDFL::wordsChangedLater()
{
    foreach (SheetBrowserPDFLItem *item, m_items) {
        item->clearWords();
    }

    if (nullptr == m_scrollTimer) {
        m_scrollTimer = new QTimer(this);
        connect(m_scrollTimer, &QTimer::timeout, this, &SheetBrowserPDFL::onWordsChanged);
        m_scrollTimer->setSingleShot(true);
    }

    if (m_scrollTimer->isActive())
        m_scrollTimer->stop();

    m_scrollTimer->start(100);
}

void SheetBrowserPDFL::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {

        DocSheet *sheet = static_cast<DocSheet *>(parentWidget());
        if (nullptr == sheet)
            return;

        if (event->delta() > 0) {
            emit sigWheelUp();
        } else {
            emit sigWheelDown();
        }
    }

    QGraphicsView::wheelEvent(event);
}

void SheetBrowserPDFL::deform(DocOperation &operation)
{
    m_lastScaleFactor = operation.scaleFactor;

    switch (operation.rotation) {
    default:
    case Dr::RotateBy0:
    case Dr::RotateBy180:
        if (Dr::FitToPageWidthMode == operation.scaleMode)
            operation.scaleFactor = ((double)this->width() - 25.0) / (double) m_maxWidth / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
        else if (Dr::FitToPageHeightMode == operation.scaleMode)
            operation.scaleFactor = (double)this->height() / (double) m_maxHeight;
        else if (Dr::FitToPageDefaultMode == operation.scaleMode)
            operation.scaleFactor = 1.0 ;
        else if (Dr::FitToPageWorHMode == operation.scaleMode) {
            qreal scaleFactor = ((double)this->width() - 25.0) / (double) m_maxWidth / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
            if (scaleFactor * m_maxHeight > this->height())
                scaleFactor = (double)this->height() / (double) m_maxHeight;
            operation.scaleFactor = scaleFactor;
        } else
            operation.scaleMode = Dr::ScaleFactorMode;
        break;
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        if (Dr::FitToPageWidthMode == operation.scaleMode)
            operation.scaleFactor = ((double)this->width() - 25.0) / (double) m_maxHeight / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
        else if (Dr::FitToPageHeightMode == operation.scaleMode)
            operation.scaleFactor = (double)this->height() / (double) m_maxWidth;
        else if (Dr::FitToPageDefaultMode == operation.scaleMode)
            operation.scaleFactor = 1.0 ;
        else if (Dr::FitToPageWorHMode == operation.scaleMode) {
            qreal scaleFactor = ((double)this->width() - 25.0) / (double) m_maxHeight / (Dr::TwoPagesMode == operation.layoutMode ? 2 : 1);
            if (scaleFactor * m_maxWidth > this->height())
                scaleFactor = (double)this->height() / (double) m_maxWidth;
            operation.scaleFactor = scaleFactor;
        } else
            operation.scaleMode = Dr::ScaleFactorMode;
        break;
    }

    for (int i = 0; i < m_items.count(); ++i) {
        m_items.at(i)->render(operation.scaleFactor, operation.rotation, true);
    }

    //开始调整位置
    int page = currentPage();

    int diff = 0;

    if (page > 0 && page <= m_items.count())
        diff = verticalScrollBar()->value() - m_items.at(page - 1)->pos().y();

    int width = 0;
    int height = 0;

    if (Dr::SinglePageMode == operation.layoutMode) {
        for (int i = 0; i < m_items.count(); ++i) {
            m_items.at(i)->clearWords();

            m_items.at(i)->setPos(0, height);

            height += m_items.at(i)->boundingRect().height() + 5;

            if (m_items.at(i)->boundingRect().width() > width)
                width = m_items.at(i)->boundingRect().width();
        }
    } else if (Dr::TwoPagesMode == operation.layoutMode) {
        for (int i = 0; i < m_items.count(); ++i) {
            m_items.at(i)->clearWords();
        }

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
                    width = m_items.at(i)->boundingRect().width() + m_items.at(i + 1)->boundingRect().width();
            } else {
                if (m_items.at(i)->boundingRect().width() > width)
                    width = m_items.at(i)->boundingRect().width();
            }
        }
    }

    setSceneRect(0, 0, width, height);

    if (page > 0 && page <= m_items.count())
        verticalScrollBar()->setValue(m_items[page - 1]->pos().y() + diff);

    wordsChangedLater();
}

bool SheetBrowserPDFL::hasLoaded()
{
    return m_hasLoaded;
}

void SheetBrowserPDFL::resizeEvent(QResizeEvent *event)
{
    if (hasLoaded()) {
        if (nullptr == m_resizeTimer) {
            m_resizeTimer = new QTimer(this);
            connect(m_resizeTimer, &QTimer::timeout, this, &SheetBrowserPDFL::sigSizeChanged);
            m_resizeTimer->setSingleShot(true);
        }

        if (m_resizeTimer->isActive())
            m_resizeTimer->stop();

        m_resizeTimer->start(10);
    }

    QGraphicsView::resizeEvent(event);
}

void SheetBrowserPDFL::mousePressEvent(QMouseEvent *event)
{
    if (QGraphicsView::NoDrag == dragMode() || QGraphicsView::RubberBandDrag == dragMode()) {
        Qt::MouseButton btn = event->button();
        if (btn == Qt::LeftButton) {
            scene()->setSelectionArea(QPainterPath());

            m_selectPressedPos = mapToScene(event->pos());

        } else if (btn == Qt::RightButton) {
            m_selectPressedPos = QPointF();

            SheetBrowserPDFLItem *item = nullptr;

            SheetBrowserPDFLAnnotation *annotation = nullptr;

            QList<QGraphicsItem *>  list = scene()->items(mapToScene(event->pos()));

            const QString &selectWords = selectedWordsText();

            foreach (QGraphicsItem *baseItem, list) {
                if (nullptr != qgraphicsitem_cast<SheetBrowserPDFLItem *>(baseItem)) {
                    item = qgraphicsitem_cast<SheetBrowserPDFLItem *>(baseItem);
                    continue;
                }

                if (nullptr != qgraphicsitem_cast<SheetBrowserPDFLAnnotation *>(baseItem)) {
                    annotation = qgraphicsitem_cast<SheetBrowserPDFLAnnotation *>(baseItem);
                    continue;
                }
            }

            SheetBrowserPDFLMenu menu;
            connect(&menu, &SheetBrowserPDFLMenu::signalMenuItemClicked, this, &SheetBrowserPDFL::onMenuItemClicked);
            if (nullptr != annotation && annotation->type() == deepin_reader::Annotation::AnnotationText) {
                //文字注释(图标)
                menu.initActions(m_sheet, item->itemIndex(), DocSheetMenuType_e::DOC_MENU_ANNO_ICON);
            } else if (nullptr != annotation && annotation->type() == deepin_reader::Annotation::AnnotationHighlight) {
                //文字高亮注释
                menu.initActions(m_sheet, item->itemIndex(), DocSheetMenuType_e::DOC_MENU_ANNO_HIGHLIGHT);
            }  else if (!selectWords.isEmpty()) {
                //选择文字
                menu.initActions(m_sheet, item->itemIndex(), DocSheetMenuType_e::DOC_MENU_SELECT_TEXT);
            } else if (nullptr != item) {
                //默认
                menu.initActions(m_sheet, item->itemIndex(), DocSheetMenuType_e::DOC_MENU_DEFAULT);
            }
            menu.exec(mapToGlobal(event->pos()));
        }
    } else if (QGraphicsView::ScrollHandDrag == dragMode()) {
        Qt::MouseButton btn = event->button();
        if (btn == Qt::RightButton) {
            popMenu(event->pos());
        }
    }

    DGraphicsView::mousePressEvent(event);
}

void SheetBrowserPDFL::mouseMoveEvent(QMouseEvent *event)
{
    if (m_magnifierLabel) {
        QPoint mousePos = event->pos();

        if (mousePos.y() < 122) {
            verticalScrollBar()->setValue(verticalScrollBar()->value() - (122 - mousePos.y()));
            mousePos.setY(122);
        }

        if (mousePos.x() < 122) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (122 - mousePos.x()));
            mousePos.setX(122);
        }

        if (mousePos.y() > (this->size().height() - 122) && (this->size().height() - 122 > 0)) {
            verticalScrollBar()->setValue(verticalScrollBar()->value() + (mousePos.y() - (this->size().height() - 122)));
            mousePos.setY(this->size().height() - 122);
        }

        if (mousePos.x() > (this->size().width() - 122) && (this->size().width() - 122 > 0)) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + (mousePos.x() - (this->size().width() - 122)));
            mousePos.setX(this->size().width() - 122);
        }

        QImage image;
        QPixmap pix(244, 244);
        pix.fill(Qt::transparent);
        if (getImagePoint(mousePos, m_lastScaleFactor + 2, image) && !image.isNull()) {
            QPainter painter(&pix);
            QPainterPath clippath;
            clippath.addRoundedRect(17, 17, 210, 210, 105, 105);
            painter.setClipPath(clippath);
            painter.drawImage(0, 0, image);
            painter.end();
        } else {
            QPainter painter(&pix);
            QPainterPath clippath;
            clippath.addRoundedRect(17, 17, 210, 210, 105, 105);
            painter.setClipPath(clippath);
            painter.fillRect(0, 0, 244, 244, Qt::white);
            painter.end();
        }

        QPainter painter(&pix);
        painter.drawPixmap(0, 0, 244, 244, QIcon::fromTheme(Pri::g_module + "maganifier").pixmap(QSize(244, 244)));
        m_magnifierLabel->setPixmap(pix);
        m_magnifierLabel->move(QPoint(mousePos.x() - 122, mousePos.y() - 122));

    } else {
        if (m_selectPressedPos.isNull()) {
            QGraphicsItem *item = itemAt(event->pos());
            if (item != nullptr) {
                if (!item->isPanel()) {
                    SheetBrowserPDFLAnnotation *annotation  = dynamic_cast<SheetBrowserPDFLAnnotation *>(item);
                    if (annotation != nullptr) {
                        m_tipsWidget->setText(annotation->annotationText());
                        QPoint showRealPos(QCursor::pos().x(), QCursor::pos().y() + 20);
                        m_tipsWidget->move(showRealPos);
                        m_tipsWidget->show();
                        setCursor(QCursor(Qt::PointingHandCursor));
                    } else {
                        m_tipsWidget->hide();
                        setCursor(QCursor(Qt::IBeamCursor));
                    }
                } else {
                    m_tipsWidget->hide();
                    setCursor(QCursor(Qt::ArrowCursor));
                }
            }
        } else {
            m_tipsWidget->hide();

            QPointF beginPos = m_selectPressedPos;
            QPointF endPos = mapToScene(event->pos());

            //开始的word
            QList<QGraphicsItem *> beginItemList = scene()->items(beginPos);
            SheetBrowserPDFLWord *beginWord = nullptr;
            foreach (QGraphicsItem *item, beginItemList) {
                if (!item->isPanel()) {
                    beginWord = qgraphicsitem_cast<SheetBrowserPDFLWord *>(item);
                    break;
                }
            }

            //结束的word
            QList<QGraphicsItem *> endItemList = scene()->items(endPos);
            SheetBrowserPDFLWord *endWord = nullptr;
            foreach (QGraphicsItem *item, endItemList) {
                if (!item->isPanel()) {
                    endWord = qgraphicsitem_cast<SheetBrowserPDFLWord *>(item);
                    break;
                }
            }

            QList<QGraphicsItem *> words;               //应该只存这几页的words 暂时等于所有的
            words = scene()->items(sceneRect());        //倒序

            if (endWord == nullptr) {
                if (endPos.y() > beginPos.y()) {
                    for (int i = 0; i < words.size(); ++i) {//从后向前检索
                        if (words[i]->mapToScene(QPointF(words[i]->boundingRect().x(), words[i]->boundingRect().y())).y() < endPos.y()) {
                            endWord = qgraphicsitem_cast<SheetBrowserPDFLWord *>(words[i]);
                            break;
                        }
                    }
                } else {
                    for (int i = words.size() - 1; i >= 0; i--) {
                        if (words[i]->mapToScene(QPointF(words[i]->boundingRect().x(), words[i]->boundingRect().y())).y() > endPos.y()) {
                            endWord = qgraphicsitem_cast<SheetBrowserPDFLWord *>(words[i]);
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

                    if (qgraphicsitem_cast<SheetBrowserPDFLWord *>(words[i]) == beginWord || qgraphicsitem_cast<SheetBrowserPDFLWord *>(words[i]) == endWord) {
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

void SheetBrowserPDFL::mouseReleaseEvent(QMouseEvent *event)
{
    Qt::MouseButton btn = event->button();
    if (btn == Qt::LeftButton) {
        m_selectPressedPos = QPointF();
    }

    QGraphicsView::mouseReleaseEvent(event);
}

int SheetBrowserPDFL::allPages()
{
    return m_items.count();
}

int SheetBrowserPDFL::currentPage()
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

int SheetBrowserPDFL::viewPointInIndex(QPoint viewPoint)
{
    SheetBrowserPDFLItem *item  = static_cast<SheetBrowserPDFLItem *>(itemAt(viewPoint));

    return m_items.indexOf(item);
}

void SheetBrowserPDFL::setCurrentPage(int page)
{
    if (page < 1 && page > allPages())
        return;

    verticalScrollBar()->setValue(m_items.at(page - 1)->pos().y());
}

bool SheetBrowserPDFL::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode)
{
    if (m_items.count() <= index)
        return false;

    image = m_items.at(index)->getImage(width, height, mode);

    return true;
}

bool SheetBrowserPDFL::getImagePoint(QPoint viewPoint, double scaleFactor, QImage &image)
{
    SheetBrowserPDFLItem *item  = static_cast<SheetBrowserPDFLItem *>(itemAt(viewPoint));

    if (nullptr == item)
        return false;

    QPointF itemPoint = item->mapFromScene(mapToScene(viewPoint));

    if (!item->contains(itemPoint))
        return false;

    QPoint point = QPoint(itemPoint.x(), itemPoint.y());

    image = item->getImagePoint(scaleFactor, point);

    return true;
}

void SheetBrowserPDFL::openMagnifier()
{
    if (nullptr == m_magnifierLabel) {
        m_magnifierLabel = new QLabel(this);
        m_magnifierLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_magnifierLabel->setWindowFlag(Qt::FramelessWindowHint);
        m_magnifierLabel->setAutoFillBackground(false);
        m_magnifierLabel->setAttribute(Qt::WA_TranslucentBackground);
        m_magnifierLabel->resize(244, 244);
        m_magnifierLabel->show();
        setDragMode(QGraphicsView::RubberBandDrag);
        setMouseTracking(true);
        setCursor(QCursor(Qt::BlankCursor));
    }
}

void SheetBrowserPDFL::closeMagnifier()
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

bool SheetBrowserPDFL::magnifierOpened()
{
    return (nullptr != m_magnifierLabel) ;
}

int SheetBrowserPDFL::maxWidth()
{
    return m_maxWidth;
}

int SheetBrowserPDFL::maxHeight()
{
    return m_maxHeight;
}

void SheetBrowserPDFL::needBookmark(int index, bool state)
{
    emit sigNeedBookMark(index, state);
}

void SheetBrowserPDFL::dragEnterEvent(QDragEnterEvent *event)
{
    event->ignore();
}

void SheetBrowserPDFL::showEvent(QShowEvent *event)
{
    if (1 != m_initPage) {
        setCurrentPage(m_initPage);
        m_initPage = 1;
    }

    QGraphicsView::showEvent(event);
}

void SheetBrowserPDFL::onMenuItemClicked(const QString &objectname, const QVariant &param)
{
    if (objectname == "Copy") {

    } else if (objectname == "RemoveAnnotation") {

    } else if (objectname == "AddAnnotationIcon") {

    } else if (objectname == "AddBookmark") {

    } else if (objectname == "RemoveHighlight") {

    } else if (objectname == "AddAnnotationHighlight") {

    } else if (objectname == "Search") {

    } else if (objectname == "RemoveBookmark") {

    } else if (objectname == "AddAnnotationIcon") {

    } else if (objectname == "Fullscreen") {

    } else if (objectname == "SlideShow") {

    } else if (objectname == "FirstPage") {

    } else if (objectname == "PreviousPage") {

    } else if (objectname == "NextPage") {

    } else if (objectname == "LastPage") {

    } else if (objectname == "RotateLeft") {

    } else if (objectname == "RotateRight") {

    } else if (objectname == "Print") {

    } else if (objectname == "DocumentInfo") {

    }
}
