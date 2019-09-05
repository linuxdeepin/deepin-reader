#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QGraphicsView>
#include <QFileInfo>

#include "global.h"
#include "renderparam.h"

class ModelAnnotation;//
class ModelPage;
class ModelDocument;
class PageItem;
class ViewSettings;

struct DocumentLayout;

class DocumentView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DocumentView(QWidget* parent = 0);
    ~DocumentView();

    ScaleMode scaleMode() const { return m_scaleMode; }
    // void setScaleMode(ScaleMode scaleMode);

    qreal scaleFactor() const { return m_scaleFactor; }
    // void setScaleFactor(qreal scaleFactor);

    Rotation rotation() const { return m_rotation; }
    // void setRotation(Rotation rotation);

    RenderPaperFlags renderPaperFlags() const { return m_renderFlags; }
    // void setRenderPaperFlags(RenderPaperFlags renderFlags);
    //void setRenderPaperFlag(RenderPaperFlag renderFlag, bool enabled = true);

    bool invertColors() const { return m_renderFlags.testFlag(InvertColors); }
   // void setInvertColors(bool invertColors) { setRenderFlag(InvertColors, invertColors); }

public slots:
    bool open(const QString& filePath);
private:
    Q_DISABLE_COPY(DocumentView)
    static ViewSettings* s_settings;

    void prepareDocument(ModelDocument* document, const QVector< ModelPage* >& pages);
    void preparePages();
    void prepareBackground();
    void prepareScene();
    void prepareView(qreal newLeft = 0.0, qreal newTop = 0.0, bool forceScroll = true, int scrollToPage = 0);

    bool checkDocument(const QString& filePath, ModelDocument* document, QVector< ModelPage* >& pages);
    void saveLeftAndTop(qreal& left, qreal& top) const;
    void adjustScrollBarPolicy();
    void loadDocumentDefaults();

private:
    ModelDocument* m_document;
    QFileInfo m_fileInfo;
    QVector< ModelPage* > m_pages;
    QVector< PageItem* > m_pageItems;
    ScaleMode m_scaleMode;
    qreal m_scaleFactor;
    Rotation m_rotation;
    RenderPaperFlags m_renderFlags;
    bool m_wasModified;
    bool m_rightToLeftMode;
    bool m_continuousMode;
    int m_currentPage;
    int m_firstPage;
    QScopedPointer< DocumentLayout > m_layout;
};

#endif // DOCUMENTVIEW_H
