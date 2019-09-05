#include <QInputDialog>
#include <QFileInfo>
#include <QScrollBar>
#include <QtMath>
#include <QDebug>


#include "documentview.h"
#include "loadhandler.h"
#include "pageitem.h"
#include "model.h"
#include "renderparam.h"
#include "documentlayout.h"
#include "viewsettings.h"

inline void adjustScaleFactor(RenderParam& renderParam, qreal scaleFactor)
{
    if(!qFuzzyCompare(renderParam.scaleFactor(), scaleFactor))
    {
        renderParam.setScaleFactor(scaleFactor);
    }
}

inline void setValueIfNotVisible(QScrollBar* scrollBar, int value)
{
    if(value < scrollBar->value() || value > scrollBar->value() + scrollBar->pageStep())
    {
        scrollBar->setValue(value);
    }
}


ViewSettings* DocumentView::s_settings = nullptr;

DocumentView::DocumentView(QWidget* parent) : QGraphicsView(parent),
    m_document(nullptr),
    m_pages(),
    m_currentPage(-1),
    m_fileInfo(),
    m_pageItems(),
    m_scaleMode(ScaleFactorMode),
    m_rotation(RotateBy0),
    m_renderFlags(0),
    m_rightToLeftMode(false),
    m_continuousMode(false)
{
    if(s_settings == 0)
    {
        s_settings = ViewSettings::instance();
    }

    setScene(new QGraphicsScene(this));
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(false);
    setDragMode(QGraphicsView::ScrollHandDrag);

    //setting
    m_continuousMode = s_settings->documentView().continuousMode();
    m_layout.reset(DocumentLayout::fromLayoutMode(s_settings->documentView().layoutMode()));
    m_rightToLeftMode = s_settings->documentView().rightToLeftMode();

    m_scaleMode = s_settings->documentView().scaleMode();
    m_scaleFactor = s_settings->documentView().scaleFactor();
    m_rotation = s_settings->documentView().rotation();

    if(s_settings->documentView().invertColors())
    {
        m_renderFlags |= InvertColors;
    }

    if(s_settings->documentView().convertToGrayscale())
    {
        m_renderFlags |= ConvertToGrayscale;
    }

    if(s_settings->documentView().trimMargins())
    {
        m_renderFlags |= TrimMargins;
    }

    switch(s_settings->documentView().compositionMode())
    {
    default:
    case DefaultCompositionMode:
        break;
    case DarkenWithPaperColorMode:
        m_renderFlags |= DarkenWithPaperColor;
        break;
    case LightenWithPaperColorMode:
        m_renderFlags |= LightenWithPaperColor;
        break;
    }

}
DocumentView:: ~DocumentView()
{

}

bool DocumentView::open(const QString &filePath)
{
    qDebug() << " open " << filePath;

    ModelDocument *document = LoadHandler::instance()->loadDocument(filePath);
    if (document != 0) {
        QVector< ModelPage * > pages;
        if (!checkDocument(filePath, document, pages)) {
            delete document;
            qDeleteAll(pages);
            return false;
        }
        qreal left = 0.0, top = 0.0;
        m_currentPage = 1;
        m_fileInfo.setFile(filePath);
        //  saveLeftAndTop(left, top);
        prepareDocument(document, pages);
        loadDocumentDefaults();
        adjustScrollBarPolicy();
        prepareScene();
        prepareView();


    }
    return document != 0;
}

void DocumentView::prepareDocument(ModelDocument *document, const QVector<ModelPage *> &pages)
{
    qDeleteAll(m_pageItems);
    qDeleteAll(m_pages);
    m_pages = pages;
    delete m_document;
    m_document = document;

    m_document->setPaperColor(s_settings->pageItem().paperColor());
    preparePages();
    prepareBackground();

}

void DocumentView::preparePages()
{
    m_pageItems.clear();
    m_pageItems.reserve(m_pages.count());
    for(int index = 0; index < m_pages.count(); ++index)
    {
        PageItem* page = new PageItem(m_pages.at(index), index);
        // page->setRubberBandMode(m_rubberBandMode);
        QGraphicsScene *pscene=scene();
        if(pscene==nullptr)
        {
            int a=0;
        }
        scene()->addItem(page);
        m_pageItems.append(page);
    }
}

void DocumentView::prepareBackground()
{
    QColor backgroundColor;

    if(s_settings->pageItem().decoratePages())
    {
        backgroundColor = s_settings->pageItem().backgroundColor();
    }
    else
    {
        backgroundColor = s_settings->pageItem().paperColor();

        if(invertColors())
        {
            backgroundColor.setRgb(~backgroundColor.rgb());
        }
    }

    scene()->setBackgroundBrush(QBrush(backgroundColor));
}

void DocumentView:: prepareScene()
{
    // prepare render parameters and adjust scale factor
    RenderParam renderParam(logicalDpiX(), logicalDpiY(), 1.0,
                            scaleFactor(), rotation(), renderPaperFlags());

    renderParam.setDevicePixelRatio(devicePixelRatioF());

    const qreal visibleWidth = m_layout->visibleWidth(viewport()->width());
    const qreal visibleHeight = m_layout->visibleHeight(viewport()->height());

    foreach(PageItem* page, m_pageItems)
    {
        const QSizeF displayedSize = page->displayedSize(renderParam);

        if(m_scaleMode == FitToPageWidthMode)
        {
            adjustScaleFactor(renderParam, visibleWidth / displayedSize.width());
        }
        else if(m_scaleMode == FitToPageSizeMode)
        {
            adjustScaleFactor(renderParam, qMin(visibleWidth / displayedSize.width(), visibleHeight / displayedSize.height()));
        }

        page->setRenderParam(renderParam);
    }

    //    // prepare layout
    qreal left = 0.0;
    qreal right = 0.0;
    qreal height =s_settings->documentView().pageSpacing();

    m_layout->prepareLayout(m_pageItems, m_rightToLeftMode,
                            left, right, height);

    scene()->setSceneRect(left, 0.0, right - left, height);


}

void DocumentView::prepareView(qreal newLeft, qreal newTop, bool forceScroll, int scrollToPage)
{
    const QRectF sceneRect = scene()->sceneRect();

    qreal top = sceneRect.top();
    qreal height = sceneRect.height();

    int horizontalValue = 0;
    int verticalValue = 0;

    scrollToPage = scrollToPage != 0 ? scrollToPage : m_currentPage;

    const int highlightIsOnPage = 0;//m_currentResult.isValid() ? pageOfResult(m_currentResult) : 0;
    //const bool highlightCurrentThumbnail = s_settings->documentView().highlightCurrentThumbnail();

    for(int index = 0; index < m_pageItems.count(); ++index)
    {
        PageItem* page = m_pageItems.at(index);

        if(m_continuousMode)
        {
            page->setVisible(true);
        }
        else
        {
            if(m_layout->leftIndex(index) == m_currentPage - 1)
            {
                page->setVisible(true);

                const QRectF boundingRect = page->boundingRect().translated(page->pos());

                top = boundingRect.top() - s_settings->documentView().pageSpacing();
                height = boundingRect.height() + 2.0 * s_settings->documentView().pageSpacing();
            }
            else
            {
                page->setVisible(false);
            //            m_highlight->setTransform(page->transform());

            //            page->stackBefore(m_highlight);
            }
        }

        //  m_thumbnailItems.at(index)->setHighlighted(highlightCurrentThumbnail && (index == m_currentPage - 1));
    }

    setSceneRect(sceneRect.left(), top, sceneRect.width(), height);

    if(!forceScroll && s_settings->documentView().minimalScrolling())
    {
        setValueIfNotVisible(horizontalScrollBar(), horizontalValue);
        setValueIfNotVisible(verticalScrollBar(), verticalValue);
    }
    else
    {
        horizontalScrollBar()->setValue(horizontalValue);
        verticalScrollBar()->setValue(verticalValue);
    }

    viewport()->update();
}

bool DocumentView::checkDocument(const QString &filePath, ModelDocument *document, QVector<ModelPage *> &pages)
{
    if(document->isLocked())
    {
        QString password = QInputDialog::getText(this, tr("Unlock %1").arg(QFileInfo(filePath).completeBaseName()), tr("Password:"), QLineEdit::Password);

        if(document->unlock(password))
        {
            return false;
        }
    }
    const int numberOfPages = document->numberOfPages();
    if(numberOfPages == 0)
    {
        qWarning() << "No pages were found in document at" << filePath;
        return false;
    }
    pages.reserve(numberOfPages);
    for(int index = 0; index < numberOfPages; ++index)
    {
        ModelPage* page = document->page(index);
        if(page == 0)
        {
            qWarning() << "No page" << index << "was found in document at" << filePath;

            return false;
        }
        pages.append(page);
    }

    return true;
}

void DocumentView::saveLeftAndTop(qreal &left, qreal &top) const
{
    const PageItem* page = m_pageItems.at(m_currentPage - 1);
    const QRectF boundingRect = page->uncroppedBoundingRect().translated(page->pos());

    const QPointF topLeft = mapToScene(viewport()->rect().topLeft());

    left = (topLeft.x() - boundingRect.x()) / boundingRect.width();
    top = (topLeft.y() - boundingRect.y()) / boundingRect.height();
}

void DocumentView::adjustScrollBarPolicy()
{
    switch(m_scaleMode)
    {
    default:
    case ScaleFactorMode:
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        break;
    case FitToPageWidthMode:
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        break;
    case FitToPageSizeMode:
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(m_continuousMode ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
        break;
    }
}

void DocumentView::loadDocumentDefaults()
{
    if(m_document->wantsContinuousMode())
    {
        m_continuousMode = true;
    }

    if(m_document->wantsSinglePageMode())
    {
        m_layout.reset(new SinglePageLayout);
    }
    //    else if(m_document->wantsTwoPagesMode())
    //    {
    //        m_layout.reset(new TwoPagesLayout);
    //    }
    //    else if(m_document->wantsTwoPagesWithCoverPageMode())
    //    {
    //        m_layout.reset(new TwoPagesWithCoverPageLayout);
    //    }

    //    if(m_document->wantsRightToLeftMode())
    //    {
    //        m_rightToLeftMode = true;
    //    }
}
