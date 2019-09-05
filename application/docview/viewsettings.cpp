#include "viewsettings.h"

#include <QApplication>
#include <QSettings>

namespace
{

inline QStringList trimmed(const QStringList& list)
{
    QStringList trimmedList;

    foreach(const QString& item, list)
    {
        trimmedList.append(item.trimmed());
    }

    return trimmedList;
}

inline int toInt(const QString& text, int defaultValue)
{
    bool ok = false;
    const int value = text.toInt(&ok);
    return ok ? value : defaultValue;
}

int dataSize(const QSettings* settings, const QString& key, int defaultValue)
{
    QString text = settings->value(key, QString("%1K").arg(defaultValue)).toString().trimmed();

    if(text.endsWith('M'))
    {
        text.chop(1);

        return toInt(text, defaultValue / 1024) * 1024;
    }
    else if(text.endsWith('K'))
    {
        text.chop(1);

        return toInt(text, defaultValue);
    }
    else
    {
        return toInt(text, defaultValue * 1024) / 1024;
    }
}

void setDataSize(QSettings* settings, const QString& key, int value)
{
    settings->setValue(key, QString("%1K").arg(value));
}

} // anonymous


ViewSettings* ViewSettings::s_instance = 0;
ViewSettings *ViewSettings::instance()
{
    if(s_instance == 0)
    {
        s_instance = new ViewSettings(qApp);
        s_instance->sync();
    }

    return s_instance;
}

ViewSettings::~ViewSettings()
{

}

void ViewSettings::sync()
{
    m_settings->sync();
    m_pageItem.sync();
    m_documentView.sync();
}

ViewSettings::ViewSettings(QObject *parent) : QObject(parent),
    m_settings(new QSettings("qpdfview", "qpdfview", this)),
    m_pageItem(m_settings),
    m_documentView(m_settings)
{

}

void ViewSettings::PageItem::sync()
{
    m_cacheSize = dataSize(m_settings, "pageItem/cacheSize", DefaultsSettings::PageItem::cacheSize());

    m_useTiling = m_settings->value("pageItem/useTiling", DefaultsSettings::PageItem::useTiling()).toBool();
    m_tileSize = m_settings->value("pageItem/tileSize", DefaultsSettings::PageItem::tileSize()).toInt();

    m_keepObsoletePixmaps = m_settings->value("pageItem/keepObsoletePixmaps", DefaultsSettings::PageItem::keepObsoletePixmaps()).toBool();
    m_useDevicePixelRatio = m_settings->value("pageItem/useDevicePixelRatio", DefaultsSettings::PageItem::useDevicePixelRatio()).toBool();

    m_decoratePages = m_settings->value("pageItem/decoratePages", DefaultsSettings::PageItem::decoratePages()).toBool();
    m_decorateLinks = m_settings->value("pageItem/decorateLinks", DefaultsSettings::PageItem::decorateLinks()).toBool();
    m_decorateFormFields = m_settings->value("pageItem/decorateFormFields", DefaultsSettings::PageItem::decorateFormFields()).toBool();

    m_backgroundColor = m_settings->value("pageItem/backgroundColor", DefaultsSettings::PageItem::backgroundColor()).value< QColor >();
    m_paperColor = m_settings->value("pageItem/paperColor", DefaultsSettings::PageItem::paperColor()).value< QColor >();

    m_highlightColor = m_settings->value("pageItem/highlightColor", DefaultsSettings::PageItem::highlightColor()).value< QColor >();
}

void ViewSettings::PageItem::setCacheSize(int cacheSize)
{
    if(cacheSize >= 0)
    {
        m_cacheSize = cacheSize;
        setDataSize(m_settings, "pageItem/cacheSize", cacheSize);
    }
}

void ViewSettings::PageItem::setUseTiling(bool useTiling)
{
    m_useTiling = useTiling;
    m_settings->setValue("pageItem/useTiling", useTiling);
}

void ViewSettings::PageItem::setKeepObsoletePixmaps(bool keepObsoletePixmaps)
{
    m_keepObsoletePixmaps = keepObsoletePixmaps;
    m_settings->setValue("pageItem/keepObsoletePixmaps", keepObsoletePixmaps);
}

void ViewSettings::PageItem::setUseDevicePixelRatio(bool useDevicePixelRatio)
{
    m_useDevicePixelRatio = useDevicePixelRatio;
    m_settings->setValue("pageItem/useDevicePixelRatio", useDevicePixelRatio);
}

void ViewSettings::PageItem::setDecoratePages(bool decoratePages)
{
    m_decoratePages = decoratePages;
    m_settings->setValue("pageItem/decoratePages", decoratePages);
}

void ViewSettings::PageItem::setDecorateLinks(bool decorateLinks)
{
    m_decorateLinks = decorateLinks;
    m_settings->setValue("pageItem/decorateLinks", decorateLinks);
}

void ViewSettings::PageItem::setDecorateFormFields(bool decorateFormFields)
{
    m_decorateFormFields = decorateFormFields;
    m_settings->setValue("pageItem/decorateFormFields", decorateFormFields);
}

void ViewSettings::PageItem::setBackgroundColor(const QColor &backgroundColor)
{
    m_backgroundColor = backgroundColor;
    m_settings->setValue("pageItem/backgroundColor", backgroundColor);
}

void ViewSettings::PageItem::setPaperColor(const QColor &paperColor)
{
    m_paperColor = paperColor;
    m_settings->setValue("pageItem/paperColor", paperColor);
}

void ViewSettings::PageItem::setHighlightColor(const QColor &highlightColor)
{
    m_highlightColor = highlightColor;
    m_settings->setValue("pageItem/highlightColor", highlightColor);
}

QColor ViewSettings::PageItem::annotationColor() const
{
    return m_settings->value("pageItem/annotationColor", DefaultsSettings::PageItem::annotationColor()).value< QColor >();
}

void ViewSettings::PageItem::setAnnotationColor(const QColor &annotationColor)
{
    m_settings->setValue("pageItem/annotationColor", annotationColor);
}

Qt::KeyboardModifiers ViewSettings::PageItem::copyToClipboardModifiers() const
{
    return static_cast< Qt::KeyboardModifiers >(m_settings->value("pageItem/copyToClipboardModifiers", static_cast< int >(DefaultsSettings::PageItem::copyToClipboardModifiers())).toInt());
}

void ViewSettings::PageItem::setCopyToClipboardModifiers(Qt::KeyboardModifiers modifiers)
{
    m_settings->setValue("pageItem/copyToClipboardModifiers", static_cast< int >(modifiers));
}

Qt::KeyboardModifiers ViewSettings::PageItem::addAnnotationModifiers() const
{
    return static_cast< Qt::KeyboardModifiers >(m_settings->value("pageItem/addAnnotationModifiers", static_cast< int >(DefaultsSettings::PageItem::addAnnotationModifiers())).toInt());
}

void ViewSettings::PageItem::setAddAnnotationModifiers(Qt::KeyboardModifiers modifiers)
{
    m_settings->setValue("pageItem/addAnnotationModifiers", static_cast< int >(modifiers));
}

Qt::KeyboardModifiers ViewSettings::PageItem::zoomToSelectionModifiers() const
{
    return static_cast< Qt::KeyboardModifiers>(m_settings->value("pageItem/zoomToSelectionModifiers", static_cast< int >(DefaultsSettings::PageItem::zoomToSelectionModifiers())).toInt());
}

void ViewSettings::PageItem::setZoomToSelectionModifiers(Qt::KeyboardModifiers modifiers)
{
    m_settings->setValue("pageItem/zoomToSelectionModifiers", static_cast< int >(modifiers));
}

Qt::KeyboardModifiers ViewSettings::PageItem::openInSourceEditorModifiers() const
{
    return static_cast< Qt::KeyboardModifiers >(m_settings->value("pageItem/openInSourceEditorModifiers", static_cast< int >(DefaultsSettings::PageItem::openInSourceEditorModifiers())).toInt());
}

void ViewSettings::PageItem::setOpenInSourceEditorModifiers(Qt::KeyboardModifiers modifiers)
{
    m_settings->setValue("pageItem/openInSourceEditorModifiers", static_cast< int >(modifiers));
}

bool ViewSettings::PageItem::annotationOverlay() const
{
    return m_settings->value("pageItem/annotationOverlay", DefaultsSettings::PageItem::annotationOverlay()).toBool();
}

void ViewSettings::PageItem::setAnnotationOverlay(bool overlay)
{
    m_settings->setValue("pageItem/annotationOverlay", overlay);
}

bool ViewSettings::PageItem::formFieldOverlay() const
{
    return m_settings->value("pageItem/formFieldOverlay", DefaultsSettings::PageItem::formFieldOverlay()).toBool();
}

void ViewSettings::PageItem::setFormFieldOverlay(bool overlay)
{
    m_settings->setValue("pageItem/formFieldOverlay", overlay);
}

ViewSettings::PageItem::PageItem(QSettings *settings):
    m_settings(settings),
    m_cacheSize(DefaultsSettings::PageItem::cacheSize()),
    m_progressIcon(),
    m_errorIcon(),
    m_keepObsoletePixmaps(DefaultsSettings::PageItem::keepObsoletePixmaps()),
    m_useDevicePixelRatio(true),
    m_decoratePages(DefaultsSettings::PageItem::decoratePages()),
    m_decorateLinks(DefaultsSettings::PageItem::decorateLinks()),
    m_decorateFormFields(DefaultsSettings::PageItem::decorateFormFields()),
    m_backgroundColor(DefaultsSettings::PageItem::backgroundColor()),
    m_paperColor(DefaultsSettings::PageItem::paperColor()),
    m_highlightColor(DefaultsSettings::PageItem::highlightColor())
{

}




//DocumentView
void ViewSettings::DocumentView::sync()
{
    m_prefetch = m_settings->value("documentView/prefetch", DefaultsSettings::DocumentView::prefetch()).toBool();
    m_prefetchDistance = m_settings->value("documentView/prefetchDistance", DefaultsSettings::DocumentView::prefetchDistance()).toInt();

    m_pagesPerRow = m_settings->value("documentView/pagesPerRow", DefaultsSettings::DocumentView::pagesPerRow()).toInt();

    m_minimalScrolling = m_settings->value("documentView/minimalScrolling", DefaultsSettings::DocumentView::minimalScrolling()).toBool();

    m_highlightCurrentThumbnail = m_settings->value("documentView/highlightCurrentThumbnail", DefaultsSettings::DocumentView::highlightCurrentThumbnail()).toBool();
    m_limitThumbnailsToResults = m_settings->value("documentView/limitThumbnailsToResults", DefaultsSettings::DocumentView::limitThumbnailsToResults()).toBool();

    m_pageSpacing = m_settings->value("documentView/pageSpacing", DefaultsSettings::DocumentView::pageSpacing()).toReal();
    m_thumbnailSpacing = m_settings->value("documentView/thumbnailSpacing", DefaultsSettings::DocumentView::thumbnailSpacing()).toReal();

    m_thumbnailSize = m_settings->value("documentView/thumbnailSize", DefaultsSettings::DocumentView::thumbnailSize()).toReal();
}

bool ViewSettings::DocumentView::openUrl() const
{
    return m_settings->value("documentView/openUrl", DefaultsSettings::DocumentView::openUrl()).toBool();
}

void ViewSettings::DocumentView::setOpenUrl(bool openUrl)
{
    m_settings->setValue("documentView/openUrl", openUrl);
}

bool ViewSettings::DocumentView::autoRefresh() const
{
    return m_settings->value("documentView/autoRefresh", DefaultsSettings::DocumentView::autoRefresh()).toBool();
}

void ViewSettings::DocumentView::setAutoRefresh(bool autoRefresh)
{
    m_settings->setValue("documentView/autoRefresh", autoRefresh);
}

int ViewSettings::DocumentView::autoRefreshTimeout() const
{
    return m_settings->value("documentView/autoRefreshTimeout", DefaultsSettings::DocumentView::autoRefreshTimeout()).toInt();
}

void ViewSettings::DocumentView::setPrefetch(bool prefetch)
{
    m_prefetch = prefetch;
    m_settings->setValue("documentView/prefetch", prefetch);
}

void ViewSettings::DocumentView::setPrefetchDistance(int prefetchDistance)
{
    if(prefetchDistance > 0)
    {
        m_prefetchDistance = prefetchDistance;
        m_settings->setValue("documentView/prefetchDistance", prefetchDistance);
    }
}

int ViewSettings::DocumentView::prefetchTimeout() const
{
    return m_settings->value("documentView/prefetchTimeout", DefaultsSettings::DocumentView::prefetchTimeout()).toInt();
}

void ViewSettings::DocumentView::setPagesPerRow(int pagesPerRow)
{
    if(pagesPerRow > 0)
    {
        m_pagesPerRow = pagesPerRow;
        m_settings->setValue("documentView/pagesPerRow", pagesPerRow);
    }
}

void ViewSettings::DocumentView::setMinimalScrolling(bool minimalScrolling)
{
    m_minimalScrolling = minimalScrolling;
    m_settings->setValue("documentView/minimalScrolling", minimalScrolling);
}

void ViewSettings::DocumentView::setHighlightCurrentThumbnail(bool highlightCurrentThumbnail)
{
    m_highlightCurrentThumbnail = highlightCurrentThumbnail;
    m_settings->setValue("documentView/highlightCurrentThumbnail", highlightCurrentThumbnail);
}

void ViewSettings::DocumentView::setLimitThumbnailsToResults(bool limitThumbnailsToResults)
{
    m_limitThumbnailsToResults = limitThumbnailsToResults;
    m_settings->setValue("documentView/limitThumbnailsToResults", limitThumbnailsToResults);
}

qreal ViewSettings::DocumentView::minimumScaleFactor() const
{
    return m_settings->value("documentView/minimumScaleFactor", DefaultsSettings::DocumentView::minimumScaleFactor()).toReal();
}

qreal ViewSettings::DocumentView::maximumScaleFactor() const
{
    return m_settings->value("documentView/maximumScaleFactor", DefaultsSettings::DocumentView::maximumScaleFactor()).toReal();
}

qreal ViewSettings::DocumentView::zoomFactor() const
{
     return m_settings->value("documentView/zoomFactor", DefaultsSettings::DocumentView::zoomFactor()).toReal();
}

void ViewSettings::DocumentView::setZoomFactor(qreal zoomFactor)
{
      m_settings->setValue("documentView/zoomFactor", zoomFactor);
}

void ViewSettings::DocumentView::setPageSpacing(qreal pageSpacing)
{
    if(pageSpacing >= 0.0)
    {
        m_pageSpacing = pageSpacing;
        m_settings->setValue("documentView/pageSpacing", pageSpacing);
    }
}

void ViewSettings::DocumentView::setThumbnailSpacing(qreal thumbnailSpacing)
{
    if(thumbnailSpacing >= 0.0)
    {
        m_thumbnailSpacing = thumbnailSpacing;
        m_settings->setValue("documentView/thumbnailSpacing", thumbnailSpacing);
    }
}

void ViewSettings::DocumentView::setThumbnailSize(qreal thumbnailSize)
{
    if(thumbnailSize >= 0.0)
    {
        m_thumbnailSize = thumbnailSize;
        m_settings->setValue("documentView/thumbnailSize", thumbnailSize);
    }
}

bool ViewSettings::DocumentView::matchCase() const
{
    return m_settings->value("documentView/matchCase", DefaultsSettings::DocumentView::matchCase()).toBool();
}

void ViewSettings::DocumentView::setMatchCase(bool matchCase)
{
      m_settings->setValue("documentView/matchCase", matchCase);
}

bool ViewSettings::DocumentView::wholeWords() const
{
   return m_settings->value("documentView/wholeWords", DefaultsSettings::DocumentView::wholeWords()).toBool();
}

void ViewSettings::DocumentView::setWholeWords(bool wholeWords)
{
     m_settings->setValue("documentView/wholeWords", wholeWords);
}

bool ViewSettings::DocumentView::parallelSearchExecution() const
{
     return m_settings->value("documentView/parallelSearchExecution", DefaultsSettings::DocumentView::parallelSearchExecution()).toBool();
}

void ViewSettings::DocumentView::setParallelSearchExecution(bool parallelSearchExecution)
{
   m_settings->setValue("documentView/parallelSearchExecution", parallelSearchExecution);
}

int ViewSettings::DocumentView::highlightDuration() const
{
  return m_settings->value("documentView/highlightDuration", DefaultsSettings::DocumentView::highlightDuration()).toInt();
}

void ViewSettings::DocumentView::setHighlightDuration(int highlightDuration)
{
    if(highlightDuration >= 0)
    {
        m_settings->setValue("documentView/highlightDuration", highlightDuration);
    }
}

QString ViewSettings::DocumentView::sourceEditor() const
{
    return m_settings->value("documentView/sourceEditor", DefaultsSettings::DocumentView::sourceEditor()).toString();
}

void ViewSettings::DocumentView::setSourceEditor(const QString &sourceEditor)
{
    m_settings->setValue("documentView/sourceEditor", sourceEditor);
}

Qt::KeyboardModifiers ViewSettings::DocumentView::zoomModifiers() const
{
      return static_cast< Qt::KeyboardModifiers >(m_settings->value("documentView/zoomModifiers", static_cast< int >(DefaultsSettings::DocumentView::zoomModifiers())).toInt());
}

void ViewSettings::DocumentView::setZoomModifiers(Qt::KeyboardModifiers zoomModifiers)
{
    m_settings->setValue("documentView/zoomModifiers", static_cast< int >(zoomModifiers));
}

Qt::KeyboardModifiers ViewSettings::DocumentView::rotateModifiers() const
{
  return static_cast< Qt::KeyboardModifiers >(m_settings->value("documentView/rotateModifiers", static_cast< int >(DefaultsSettings::DocumentView::rotateModifiers())).toInt());
}

void ViewSettings::DocumentView::setRotateModifiers(Qt::KeyboardModifiers rotateModifiers)
{
      m_settings->setValue("documentView/rotateModifiers", static_cast< int >(rotateModifiers));
}

Qt::KeyboardModifiers ViewSettings::DocumentView::scrollModifiers() const
{
   return static_cast< Qt::KeyboardModifiers >(m_settings->value("documentView/scrollModifiers", static_cast< int >(DefaultsSettings::DocumentView::scrollModifiers())).toInt());
}

void ViewSettings::DocumentView::setScrollModifiers(Qt::KeyboardModifiers scrollModifiers)
{
     m_settings->setValue("documentView/scrollModifiers", static_cast< int >(scrollModifiers));
}

bool ViewSettings::DocumentView::continuousMode() const
{
 return m_settings->value("documentView/continuousMode", DefaultsSettings::DocumentView::continuousMode()).toBool();
}

void ViewSettings::DocumentView::setContinuousMode(bool continuousMode)
{
 m_settings->setValue("documentView/continuousMode", continuousMode);
}

LayoutMode ViewSettings::DocumentView::layoutMode() const
{
  return static_cast< LayoutMode >(m_settings->value("documentView/layoutMode", static_cast< uint >(DefaultsSettings::DocumentView::layoutMode())).toUInt());
}

void ViewSettings::DocumentView::setLayoutMode(LayoutMode layoutMode)
{
 m_settings->setValue("documentView/layoutMode", static_cast< uint >(layoutMode));
}

bool ViewSettings::DocumentView::rightToLeftMode() const
{
 return m_settings->value("documentView/rightToLeftMode", DefaultsSettings::DocumentView::rightToLeftMode()).toBool();
}

void ViewSettings::DocumentView::setRightToLeftMode(bool rightToLeftMode)
{
 m_settings->setValue("documentView/rightToLeftMode", rightToLeftMode);
}

ScaleMode ViewSettings::DocumentView::scaleMode() const
{
return static_cast< ScaleMode >(m_settings->value("documentView/scaleMode", static_cast< uint >(DefaultsSettings::DocumentView::scaleMode())).toUInt());
}

void ViewSettings::DocumentView::setScaleMode(ScaleMode scaleMode)
{
 m_settings->setValue("documentView/scaleMode", static_cast< uint >(scaleMode));
}

qreal ViewSettings::DocumentView::scaleFactor() const
{
 return m_settings->value("documentView/scaleFactor", DefaultsSettings::DocumentView::scaleFactor()).toReal();
}

void ViewSettings::DocumentView::setScaleFactor(qreal scaleFactor)
{
 m_settings->setValue("documentView/scaleFactor", scaleFactor);
}

Rotation ViewSettings::DocumentView::rotation() const
{
   return static_cast< Rotation >(m_settings->value("documentView/rotation", static_cast< uint >(DefaultsSettings::DocumentView::rotation())).toUInt());
}

void ViewSettings::DocumentView::setRotation(Rotation rotation)
{
 m_settings->setValue("documentView/rotation", static_cast< uint >(rotation));
}

bool ViewSettings::DocumentView::invertColors() const
{
 return m_settings->value("documentView/invertColors", DefaultsSettings::DocumentView::invertColors()).toBool();
}

void ViewSettings::DocumentView::setInvertColors(bool invertColors)
{
m_settings->setValue("documentView/invertColors", invertColors);
}

bool ViewSettings::DocumentView::convertToGrayscale() const
{
   return m_settings->value("documentView/convertToGrayscale", DefaultsSettings::DocumentView::convertToGrayscale()).toBool();
}

void ViewSettings::DocumentView::setConvertToGrayscale(bool convertToGrayscale)
{
 m_settings->setValue("documentView/convertToGrayscale", convertToGrayscale);
}

bool ViewSettings::DocumentView::trimMargins() const
{
 return m_settings->value("documentView/trimMargins", DefaultsSettings::DocumentView::trimMargins()).toBool();
}

void ViewSettings::DocumentView::setTrimMargins(bool trimMargins)
{
 m_settings->setValue("documentView/trimMargins", trimMargins);
}

CompositionMode ViewSettings::DocumentView::compositionMode() const
{
 return static_cast< CompositionMode >(m_settings->value("documentView/compositionMode", static_cast< uint >(DefaultsSettings::DocumentView::compositionMode())).toInt());
}

void ViewSettings::DocumentView::setCompositionMode(CompositionMode compositionMode)
{
 m_settings->setValue("documentView/compositionMode", static_cast< uint >(compositionMode));
}

bool ViewSettings::DocumentView::highlightAll() const
{
  return m_settings->value("documentView/highlightAll", DefaultsSettings::DocumentView::highlightAll()).toBool();
}

void ViewSettings::DocumentView::setHighlightAll(bool highlightAll)
{
 m_settings->setValue("documentView/highlightAll", highlightAll);
}

ViewSettings::DocumentView::DocumentView(QSettings *settings):
    m_settings(settings),
    m_prefetch(DefaultsSettings::DocumentView::prefetch()),
    m_prefetchDistance(DefaultsSettings::DocumentView::prefetchDistance()),
    m_pagesPerRow(DefaultsSettings::DocumentView::pagesPerRow()),
    m_highlightCurrentThumbnail(DefaultsSettings::DocumentView::highlightCurrentThumbnail()),
    m_limitThumbnailsToResults(DefaultsSettings::DocumentView::limitThumbnailsToResults()),
    m_pageSpacing(DefaultsSettings::DocumentView::pageSpacing()),
    m_thumbnailSpacing(DefaultsSettings::DocumentView::thumbnailSpacing()),
    m_thumbnailSize(DefaultsSettings::DocumentView::thumbnailSize())
{

}

bool DefaultsSettings::DocumentView::rightToLeftMode()
{
     return QLocale::system().textDirection() == Qt::RightToLeft;
}
