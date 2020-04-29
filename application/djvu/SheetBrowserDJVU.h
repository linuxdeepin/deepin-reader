#ifndef SheetBrowserDJVU_H
#define SheetBrowserDJVU_H

#include <QFileInfo>
#include <QGraphicsView>
#include <QMap>
#include <QPersistentModelIndex>

class QDomNode;
class QFileSystemWatcher;
class QPrinter;
class QStandardItemModel;

#include "global.h"

namespace qpdfview {

namespace Model {
class Annotation;
class Page;
class Document;
}

class Settings;
class PageItem;
class ThumbnailItem;
class SearchModel;
class SearchTask;
class PresentationView;
class ShortcutHandler;
class MainWindow;
struct DocumentLayout;

class SheetBrowserDJVU : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SheetBrowserDJVU(QWidget *parent = 0);
    ~SheetBrowserDJVU();

    inline const QFileInfo &fileInfo() const { return m_fileInfo; }
    inline bool wasModified() const { return m_wasModified; }

    inline int numberOfPages() const { return m_pages.count(); }
    inline int currentPage() const { return m_currentPage; }

    inline bool hasFrontMatter() const { return m_firstPage > 1; }

    inline int firstPage() const { return m_firstPage; }

    void setFirstPage(int firstPage);

    QString defaultPageLabelFromNumber(int number) const;

    QString pageLabelFromNumber(int number) const;

    int pageNumberFromLabel(const QString &label) const;

    QString title() const;

    static QStringList openFilter();
    QStringList saveFilter() const;

    bool canSave() const;

    inline bool continuousMode() const { return m_continuousMode; }
    void setContinuousMode(bool continuousMode);

    deepin_reader::LayoutMode layoutMode() const;
    void setLayoutMode(deepin_reader::LayoutMode layoutMode);

    inline bool rightToLeftMode() const { return m_rightToLeftMode; }
    void setRightToLeftMode(bool rightToLeftMode);

    inline deepin_reader::ScaleMode scaleMode() const { return m_scaleMode; }
    void setScaleMode(ScaleMode scaleMode);

    inline qreal scaleFactor() const { return m_scaleFactor; }
    void setScaleFactor(qreal scaleFactor);

    inline deepin_reader::Rotation rotation() const { return m_rotation; }
    void setRotation(deepin_reader::Rotation rotation);

    inline bool invertColors() const { return m_invertColors; }
    void setInvertColors(bool invertColors);

    inline bool convertToGrayscale() const { return m_convertToGrayscale; }
    void setConvertToGrayscale(bool convertToGrayscale);

    inline bool highlightAll() const { return m_highlightAll; }
    void setHighlightAll(bool highlightAll);

    inline Qt::Orientation thumbnailsOrientation() const { return m_thumbnailsOrientation; }
    void setThumbnailsOrientation(Qt::Orientation thumbnailsOrientation);

    inline const QVector< ThumbnailItem * > &thumbnailItems() const { return m_thumbnailItems; }
    inline QGraphicsScene *thumbnailsScene() const { return m_thumbnailsScene; }

    inline QStandardItemModel *outlineModel() const { return m_outlineModel; }
    inline QStandardItemModel *propertiesModel() const { return m_propertiesModel; }

    QStandardItemModel *fontsModel() const;

    QString searchText() const;
    bool searchMatchCase() const;

    QString surroundingText(int page, const QRectF &rect) const;

signals:
    void documentChanged();
    void documentModified();
    void numberOfPagesChanged(int numberOfPages);
    void currentPageChanged(int currentPage, bool trackChange = false);
    void continuousModeChanged(bool continuousMode);
    void layoutModeChanged(LayoutMode layoutMode);
    void rightToLeftModeChanged(bool rightToLeftMode);
    void scaleModeChanged(ScaleMode scaleMode);
    void scaleFactorChanged(qreal scaleFactor);
    void rotationChanged(Rotation rotation);
    void linkClicked(int page);
    void linkClicked(bool newTab, const QString &filePath, int page);
    void invertColorsChanged(bool invertColors);
    void convertToGrayscaleChanged(bool convertToGrayscale);
    void highlightAllChanged(bool highlightAll);
    void searchFinished();
    void searchProgressChanged(int progress);

public slots:
    void show();
    bool open(const QString &filePath);
    bool save(const QString &filePath, bool withChanges);
    bool print(QPrinter *printer, const PrintOptions &printOptions = PrintOptions());

    void previousPage();
    void nextPage();
    void firstPage();
    void lastPage();

    void jumpToPage(int page, bool trackChange = true, qreal changeLeft = 0.0, qreal changeTop = 0.0);

    void temporaryHighlight(int page, const QRectF &highlight);

    void startSearch(const QString &text, bool matchCase = true);
    void cancelSearch();

    void clearResults();

    void findPrevious();
    void findNext();
    void findResult(const QModelIndex &index);

    void zoomIn();
    void zoomOut();
    void originalSize();

    void rotateLeft();
    void rotateRight();

    void startPresentation();

protected slots:
    void on_verticalScrollBar_valueChanged();

    void on_autoRefresh_timeout();
    void on_prefetch_timeout();

    void on_temporaryHighlight_timeout();

    void on_searchTask_progressChanged(int progress);
    void on_searchTask_resultsReady(int index, const QList< QRectF > &results);

    void on_pages_cropRectChanged();
    void on_thumbnails_cropRectChanged();

    void on_pages_linkClicked(bool newTab, int page, qreal left, qreal top);
    void on_pages_linkClicked(bool newTab, const QString &fileName, int page);
    void on_pages_linkClicked(const QString &url);

    void on_pages_rubberBandFinished();

    void on_pages_editSourceRequested(int page, const QPointF &pos);
    void on_pages_zoomToSelectionRequested(int page, const QRectF &rect);

    void on_pages_wasModified();

protected:
    void resizeEvent(QResizeEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void contextMenuEvent(QContextMenuEvent *event);

private:
    Q_DISABLE_COPY(SheetBrowserDJVU)

    static Settings *s_settings;
    static ShortcutHandler *s_shortcutHandler;

    QFileSystemWatcher *m_autoRefreshWatcher;
    QTimer *m_autoRefreshTimer;

    QTimer *m_prefetchTimer;

    Model::Document *m_document;
    QVector< Model::Page * > m_pages;

    QFileInfo m_fileInfo;
    bool m_wasModified;

    int m_currentPage;
    int m_firstPage;

#ifdef WITH_CUPS

    bool printUsingCUPS(QPrinter *printer, const PrintOptions &printOptions, int fromPage, int toPage);

#endif // WITH_CUPS

    bool printUsingQt(QPrinter *printer, const PrintOptions &printOptions, int fromPage, int toPage);

    void saveLeftAndTop(qreal &left, qreal &top) const;

    QScopedPointer< DocumentLayout > m_layout;

    bool m_continuousMode;
    bool m_rightToLeftMode;
    deepin_reader::ScaleMode m_scaleMode;
    qreal m_scaleFactor;
    deepin_reader::Rotation m_rotation;

    bool m_invertColors;
    bool m_convertToGrayscale;
    bool m_highlightAll;

    QVector< PageItem * > m_pageItems;
    QVector< ThumbnailItem * > m_thumbnailItems;

    QGraphicsRectItem *m_highlight;

    Qt::Orientation m_thumbnailsOrientation;
    QGraphicsScene *m_thumbnailsScene;

    QStandardItemModel *m_outlineModel;
    QStandardItemModel *m_propertiesModel;

    bool checkDocument(const QString &filePath, Model::Document *document, QVector< Model::Page * > &pages);

    void loadFallbackOutline();
    void loadDocumentDefaults();

    void adjustScrollBarPolicy();

    void prepareDocument(Model::Document *document, const QVector< Model::Page * > &pages);
    void preparePages();
    void prepareThumbnails();
    void prepareBackground();

    void prepareScene();
    void prepareView(qreal changeLeft = 0.0, qreal changeTop = 0.0, int visiblePage = 0);

    void prepareThumbnailsScene();

    void prepareHighlight(int index, const QRectF &highlight);

    static SearchModel *s_searchModel;
    QPersistentModelIndex m_currentResult;

    void checkResult();
    void applyResult();

};

} // qpdfview

#endif // SheetBrowserDJVU_H
