#ifndef VIEWSETTINGS_H
#define VIEWSETTINGS_H

#include <QColor>
#include <QIcon>
#include <QKeySequence>
#include <QObject>

#include "global.h"

class QSettings;

class ViewSettings: public QObject
{
    Q_OBJECT

public:
    static ViewSettings* instance();
    ~ViewSettings();

    // page item

    class PageItem
    {
    public:
        void sync();

        int cacheSize() const { return m_cacheSize; }
        void setCacheSize(int cacheSize);

        bool useTiling() const { return m_useTiling; }
        void setUseTiling(bool useTiling);

        int tileSize() const { return m_tileSize; }

        const QIcon& progressIcon() const { return m_progressIcon; }
        void setProgressIcon(const QIcon& progressIcon) { m_progressIcon = progressIcon; }

        const QIcon& errorIcon() const { return m_errorIcon; }
        void setErrorIcon(const QIcon& errorIcon) { m_errorIcon = errorIcon; }

        bool keepObsoletePixmaps() const { return m_keepObsoletePixmaps; }
        void setKeepObsoletePixmaps(bool keepObsoletePixmaps);

        bool useDevicePixelRatio() const { return m_useDevicePixelRatio; }
        void setUseDevicePixelRatio(bool useDevicePixelRatio);

        bool decoratePages() const { return m_decoratePages; }
        void setDecoratePages(bool decoratePages);

        bool decorateLinks() const { return m_decorateLinks; }
        void setDecorateLinks(bool decorateLinks);

        bool decorateFormFields() const { return m_decorateFormFields; }
        void setDecorateFormFields(bool decorateFormFields);

        const QColor& backgroundColor() const { return m_backgroundColor; }
        void setBackgroundColor(const QColor& backgroundColor);

        const QColor& paperColor() const { return m_paperColor; }
        void setPaperColor(const QColor& paperColor);

        const QColor& highlightColor() const { return m_highlightColor; }
        void setHighlightColor(const QColor& highlightColor);

        QColor annotationColor() const;
        void setAnnotationColor(const QColor& annotationColor);

        Qt::KeyboardModifiers copyToClipboardModifiers() const;
        void setCopyToClipboardModifiers(Qt::KeyboardModifiers modifiers);

        Qt::KeyboardModifiers addAnnotationModifiers() const;
        void setAddAnnotationModifiers(Qt::KeyboardModifiers modifiers);

        Qt::KeyboardModifiers zoomToSelectionModifiers() const;
        void setZoomToSelectionModifiers(Qt::KeyboardModifiers modifiers);

        Qt::KeyboardModifiers openInSourceEditorModifiers() const;
        void setOpenInSourceEditorModifiers(Qt::KeyboardModifiers modifiers);

        bool annotationOverlay() const;
        void setAnnotationOverlay(bool overlay);

        bool formFieldOverlay() const;
        void setFormFieldOverlay(bool overlay);

    private:
        PageItem(QSettings* settings);
        friend class ViewSettings;

        QSettings* m_settings;
        int m_cacheSize;
        int m_tileSize;
        QIcon m_progressIcon;
        QIcon m_errorIcon;
        bool m_useTiling;
        bool m_keepObsoletePixmaps;
        bool m_useDevicePixelRatio;
        bool m_decoratePages;
        bool m_decorateLinks;
        bool m_decorateFormFields;
        QColor m_backgroundColor;
        QColor m_paperColor;
        QColor m_highlightColor;

    };

    // document view

    class DocumentView
    {
    public:
        void sync();

        bool openUrl() const;
        void setOpenUrl(bool openUrl);

        bool autoRefresh() const;
        void setAutoRefresh(bool autoRefresh);

        int autoRefreshTimeout() const;

        bool prefetch() const { return m_prefetch; }
        void setPrefetch(bool prefetch);

        int prefetchDistance() const { return m_prefetchDistance; }
        void setPrefetchDistance(int prefetchDistance);

        int prefetchTimeout() const;

        int pagesPerRow() const { return m_pagesPerRow; }
        void setPagesPerRow(int pagesPerRow);

        bool minimalScrolling() const { return m_minimalScrolling; }
        void setMinimalScrolling(bool minimalScrolling);

        bool highlightCurrentThumbnail() const { return m_highlightCurrentThumbnail; }
        void setHighlightCurrentThumbnail(bool highlightCurrentThumbnail);

        bool limitThumbnailsToResults() const { return m_limitThumbnailsToResults; }
        void setLimitThumbnailsToResults(bool limitThumbnailsToResults);

        qreal minimumScaleFactor() const;
        qreal maximumScaleFactor() const;

        qreal zoomFactor() const;
        void setZoomFactor(qreal zoomFactor);

        qreal pageSpacing() const { return m_pageSpacing; }
        void setPageSpacing(qreal pageSpacing);

        qreal thumbnailSpacing() const { return m_thumbnailSpacing; }
        void setThumbnailSpacing(qreal thumbnailSpacing);

        qreal thumbnailSize() const { return m_thumbnailSize; }
        void setThumbnailSize(qreal thumbnailSize);

        bool matchCase() const;
        void setMatchCase(bool matchCase);

        bool wholeWords() const;
        void setWholeWords(bool wholeWords);

        bool parallelSearchExecution() const;
        void setParallelSearchExecution(bool parallelSearchExecution);

        int highlightDuration() const;
        void setHighlightDuration(int highlightDuration);

        QString sourceEditor() const;
        void setSourceEditor(const QString& sourceEditor);

        Qt::KeyboardModifiers zoomModifiers() const;
        void setZoomModifiers(Qt::KeyboardModifiers zoomModifiers);

        Qt::KeyboardModifiers rotateModifiers() const;
        void setRotateModifiers(Qt::KeyboardModifiers rotateModifiers);

        Qt::KeyboardModifiers scrollModifiers() const;
        void setScrollModifiers(Qt::KeyboardModifiers scrollModifiers);

        // per-tab settings

        bool continuousMode() const;
        void setContinuousMode(bool continuousMode);

        LayoutMode layoutMode() const;
        void setLayoutMode(LayoutMode layoutMode);

        bool rightToLeftMode() const;
        void setRightToLeftMode(bool rightToLeftMode);

        ScaleMode scaleMode() const;
        void setScaleMode(ScaleMode scaleMode);

        qreal scaleFactor() const;
        void setScaleFactor(qreal scaleFactor);

        Rotation rotation() const;
        void setRotation(Rotation rotation);

        bool invertColors() const;
        void setInvertColors(bool invertColors);

        bool convertToGrayscale() const;
        void setConvertToGrayscale(bool convertToGrayscale);

        bool trimMargins() const;
        void setTrimMargins(bool trimMargins);

        CompositionMode compositionMode() const;
        void setCompositionMode(CompositionMode compositionMode);

        bool highlightAll() const;
        void setHighlightAll(bool highlightAll);

    private:
        DocumentView(QSettings* settings);
        friend class ViewSettings;

        QSettings* m_settings;

        bool m_prefetch;
        int m_prefetchDistance;
        int m_pagesPerRow;
        bool m_minimalScrolling;
        bool m_highlightCurrentThumbnail;
        bool m_limitThumbnailsToResults;

        qreal m_pageSpacing;
        qreal m_thumbnailSpacing;
        qreal m_thumbnailSize;

    };


    void sync();

    PageItem& pageItem() { return m_pageItem; }
    DocumentView& documentView() { return m_documentView; }
private:
    Q_DISABLE_COPY(ViewSettings)

    static ViewSettings* s_instance;
    ViewSettings(QObject* parent = 0);

    QSettings* m_settings;
    PageItem m_pageItem;
    DocumentView m_documentView;

};

// defaults

class DefaultsSettings
{
public:
    class PageItem
    {
    public:
        static int cacheSize() { return 32 * 1024; }

        static bool useTiling() { return false; }
        static int tileSize() { return 1024; }

        static bool keepObsoletePixmaps() { return false; }
        static bool useDevicePixelRatio() { return false; }

        static bool decoratePages() { return true; }
        static bool decorateLinks() { return true; }
        static bool decorateFormFields() { return true; }

        static QColor backgroundColor() { return Qt::darkGray; }
        static QColor paperColor() { return Qt::white; }

        static QColor highlightColor() { return Qt::yellow; }
        static QColor annotationColor() { return Qt::yellow; }

        static Qt::KeyboardModifiers copyToClipboardModifiers() { return Qt::ShiftModifier; }
        static Qt::KeyboardModifiers addAnnotationModifiers() { return Qt::ControlModifier; }
        static Qt::KeyboardModifiers zoomToSelectionModifiers() { return Qt::ShiftModifier | Qt::ControlModifier; }
        static Qt::KeyboardModifiers openInSourceEditorModifiers() { return Qt::NoModifier; }

        static bool annotationOverlay() { return false; }
        static bool formFieldOverlay() { return true; }

    private:
        PageItem() {}

    };

    class DocumentView
    {
    public:
        static bool openUrl() { return false; }

        static bool autoRefresh() { return false; }

        static int autoRefreshTimeout() { return 750; }

        static bool prefetch() { return false; }
        static int prefetchDistance() { return 1; }

        static int prefetchTimeout() { return 250; }

        static int pagesPerRow() { return 3; }

        static bool minimalScrolling() { return false; }

        static bool highlightCurrentThumbnail() { return false; }
        static bool limitThumbnailsToResults() { return false; }

        static qreal minimumScaleFactor() { return 0.1; }
        static qreal maximumScaleFactor() { return 50.0; }

        static qreal zoomFactor() { return 1.1; }

        static qreal pageSpacing() { return 5.0; }
        static qreal thumbnailSpacing() { return 3.0; }

        static qreal thumbnailSize() { return 150.0; }

        static CompositionMode compositionMode() { return DefaultCompositionMode; }

        static bool matchCase() { return false; }
        static bool wholeWords() { return false; }
        static bool parallelSearchExecution() { return false; }

        static int highlightDuration() { return 5 * 1000; }
        static QString sourceEditor() { return QString(); }

        static Qt::KeyboardModifiers zoomModifiers() { return Qt::ControlModifier; }
        static Qt::KeyboardModifiers rotateModifiers() { return Qt::ShiftModifier; }
        static Qt::KeyboardModifiers scrollModifiers() { return Qt::AltModifier; }

        // per-tab defaults

        static bool continuousMode() { return false; }
        static LayoutMode layoutMode() { return SinglePageMode; }
        static bool rightToLeftMode();

        static ScaleMode scaleMode() { return ScaleFactorMode; }
        static qreal scaleFactor() { return 1.0; }
        static Rotation rotation() { return RotateBy0; }

        static bool invertColors() { return false; }
        static bool convertToGrayscale() { return false; }
        static bool trimMargins() { return false; }

        static bool highlightAll() { return false; }

    private:
        DocumentView() {}

    };



};

#endif // VIEWSETTINGS_H
