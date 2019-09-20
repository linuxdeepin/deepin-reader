#ifndef XPSAPI_H
#define XPSAPI_H

#include <QColor>
#include <QDomDocument>
#include <QFontDatabase>
#include <QImage>
#include <QXmlStreamReader>
#include <QXmlDefaultHandler>
#include <QStack>
#include <QVariant>
#include <QLoggingCategory>

#include <KZip>

typedef enum {abtCommand, abtNumber, abtComma, abtEOF} AbbPathTokenType;

class AbbPathToken
{
public:
    QString data;
    int curPos;

    AbbPathTokenType type;
    char command;
    double number;
};

/**
    Holds information about xml element during SAX parsing of page
*/
class XpsRenderNode
{
public:
    QString name;
    QVector<XpsRenderNode> children;
    QXmlAttributes attributes;
    QVariant data;

    XpsRenderNode *findChild( const QString &name );
    QVariant getRequiredChildData( const QString &name );
    QVariant getChildData( const QString &name );
};

struct XpsGradient {
    XpsGradient( double o, const QColor &c )
        : offset( o ), color( c )
    {}

    double offset;
    QColor color;
};

/**
    Types of data in XpsRenderNode::data. Name of each type consist of Xps and
    name of xml element which data it holds
*/
typedef QTransform XpsMatrixTransform;
typedef QTransform XpsRenderTransform;
typedef QBrush XpsFill;
struct XpsPathFigure {
    XpsPathFigure( const QPainterPath &_path, bool filled )
        : path( _path ), isFilled( filled )
    {}

    QPainterPath path;
    bool isFilled;
};
struct XpsPathGeometry {
    XpsPathGeometry()
        : fillRule( Qt::OddEvenFill )
    {}
    ~XpsPathGeometry()
    {
        qDeleteAll( paths );
    }

    XpsPathGeometry(const XpsPathGeometry &) = delete;
    XpsPathGeometry &operator=(const XpsPathGeometry &) = delete;

    QList< XpsPathFigure * > paths;
    Qt::FillRule fillRule;
    XpsMatrixTransform transform;
};

class XpsPage;
class XpsFile;

class XpsHandler: public QXmlDefaultHandler
{
public:
    explicit XpsHandler( XpsPage *page );
    ~XpsHandler();

    bool startElement( const QString &nameSpace,
                       const QString &localName,
                       const QString &qname,
                       const QXmlAttributes &atts ) override;
    bool endElement( const QString &nameSpace,
                     const QString &localName,
                     const QString &qname ) override;
    bool startDocument() override;

protected:
    XpsPage *m_page;

    void processStartElement( XpsRenderNode &node );
    void processEndElement( XpsRenderNode &node );

    // Methods for processing of different xml elements
    void processGlyph( XpsRenderNode &node );
    void processPath( XpsRenderNode &node );
    void processPathData( XpsRenderNode &node );
    void processFill( XpsRenderNode &node );
    void processStroke( XpsRenderNode &node );
    void processImageBrush (XpsRenderNode &node );
    void processPathGeometry( XpsRenderNode &node );
    void processPathFigure( XpsRenderNode &node );

    QPainter *m_painter;

    QImage m_image;

    QStack<XpsRenderNode> m_nodes;

    friend class XpsPage;
};
class PageXPS;
class XpsPage
{
public:
    XpsPage(XpsFile *file, const QString &fileName);
    ~XpsPage();

    XpsPage(const XpsPage &) = delete;
    XpsPage &operator=(const XpsPage &) = delete;

    QSizeF size() const;
    bool renderToImage( QImage *p );
    bool renderToPainter( QPainter *painter );
//    Okular::TextPage *textPage();
    bool loadPageWords(void *words);

    QImage loadImageFromFile( const QString &filename );
    QString fileName() const
    {
        return m_fileName;
    }

private:
    XpsFile *m_file;
    const QString m_fileName;

    QSizeF m_pageSize;


    QString m_thumbnailFileName;
    bool m_thumbnailMightBeAvailable;
    QImage m_thumbnail;
    bool m_thumbnailIsLoaded;

    QImage *m_pageImage;
    bool m_pageIsRendered;

    friend class XpsHandler;
    friend class XpsTextExtractionHandler;
};

/**
   Represents one of the (perhaps the only) documents in an XpsFile
*/
class XpsDocument
{
public:
    XpsDocument(XpsFile *file, const QString &fileName);
    ~XpsDocument();

    XpsDocument(const XpsDocument &) = delete;
    XpsDocument &operator=(const XpsDocument &) = delete;

    /**
       the total number of pages in this document
    */
    int numPages() const;

    /**
       obtain a certain page from this document

       \param pageNum the number of the page to return

       \note page numbers are zero based - they run from 0 to
       numPages() - 1
    */
    XpsPage *page(int pageNum) const;

    /**
      whether this document has a Document Structure
    */
    bool hasDocumentStructure();

    /**
      the document structure for this document, if available
    */
//    const Okular::DocumentSynopsis *documentStructure();

private:
    void parseDocumentStructure( const QString &documentStructureFileName );

    QList<XpsPage *> m_pages;
    XpsFile *m_file;
    bool m_haveDocumentStructure;
//    Okular::DocumentSynopsis *m_docStructure;
    QMap<QString, int> m_docStructurePageMap;
};

/**
   Represents the contents of a Microsoft XML Paper Specification
   format document.
*/
class XpsFile
{
public:
    XpsFile();
    ~XpsFile();

    XpsFile(const XpsFile &) = delete;
    XpsFile &operator=(const XpsFile &) = delete;

    bool loadDocument( const QString &fileName );
    bool closeDocument();

//    Okular::DocumentInfo generateDocumentInfo() const;

    QImage thumbnail();

    /**
       the total number of XpsDocuments with this file
    */
    int numDocuments() const;

    /**
       the total number of pages in all the XpsDocuments within this
       file
    */
    int numPages() const;

    /**
       a page from the file

        \param pageNum the page number of the page to return

        \note page numbers are zero based - they run from 0 to
        numPages() - 1
    */
    XpsPage *page(int pageNum) const;

    /**
       obtain a certain document from this file

       \param documentNum the number of the document to return

       \note document numbers are zero based - they run from 0 to
       numDocuments() - 1
    */
    XpsDocument *document(int documentNum) const;

    QFont getFontByName( const QString &fontName, float size );

    KZip *xpsArchive();


private:
    int loadFontByName( const QString &fontName );

    QList<XpsDocument *> m_documents;
    QList<XpsPage *> m_pages;

    QString m_thumbnailFileName;
    bool m_thumbnailMightBeAvailable;
    QImage m_thumbnail;
    bool m_thumbnailIsLoaded;

    QString m_corePropertiesFileName;

    QString m_signatureOrigin;

    KZip *m_xpsArchive;

    QMap<QString, int> m_fontCache;
    QFontDatabase m_fontDatabase;
};


#endif // XPSAPI_H
