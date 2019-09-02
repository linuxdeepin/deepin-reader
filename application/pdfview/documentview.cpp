#include <QInputDialog>
#include <QFileInfo>
#include <QDebug>

#include "documentview.h"
#include "loadhandler.h"


DocumentView::DocumentView(QWidget* parent) : QGraphicsView(parent),
    m_document(nullptr)
{

}
DocumentView:: ~DocumentView()
{

}

bool DocumentView::open(const QString& filePath)
{
    ModelDocument* document =LoadHandler::instance()->loadDocument(filePath);
    if(document != 0)
    {
        QVector< ModelPage* > pages;
        if(!checkDocument(filePath, document, pages))
        {
            delete document;
            qDeleteAll(pages);
            return false;
        }
        qreal left = 0.0, top = 0.0;
        saveLeftAndTop(left, top);


    }
    return document != 0;
}

void DocumentView::prepareDocument(ModelDocument *document, const QVector<ModelPage *> &pages)
{

}

bool DocumentView   ::checkDocument(const QString &filePath, ModelDocument *document, QVector<ModelPage *> &pages)
{
    if(document->isLocked())
    {
        QString password = QInputDialog::getText(this, tr("Unlock %1").arg(QFileInfo(filePath).completeBaseName()), tr("Password:"), QLineEdit::Password);

        if(document->unlock(password))
        {
            return false;
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
    }
    return true;
}

void DocumentView::saveLeftAndTop(qreal &left, qreal &top) const
{
//    const PageItem* page = m_pageItems.at(m_currentPage - 1);
//    const QRectF boundingRect = page->uncroppedBoundingRect().translated(page->pos());

//    const QPointF topLeft = mapToScene(viewport()->rect().topLeft());

//    left = (topLeft.x() - boundingRect.x()) / boundingRect.width();
//    top = (topLeft.y() - boundingRect.y()) / boundingRect.height();
}
