#include "documentiff.h"
#include "pagetiff.h"
#include <QFileInfo>

tsize_t tiffReadProc( thandle_t handle, tdata_t buf, tsize_t size )
{
    QIODevice *device = static_cast< QIODevice * >( handle );
    return device->isReadable() ? device->read( static_cast< char * >( buf ), size ) : -1;
}

tsize_t tiffWriteProc( thandle_t handle, tdata_t buf, tsize_t size )
{
    QIODevice *device = static_cast< QIODevice * >( handle );
    return device->write( static_cast< char * >( buf ), size );
}

toff_t tiffSeekProc( thandle_t handle, toff_t offset, int whence )
{
    QIODevice *device = static_cast< QIODevice * >( handle );
    switch ( whence ) {
    case SEEK_SET:
        device->seek( offset );
        break;
    case SEEK_CUR:
        device->seek( device->pos() + offset );
        break;
    case SEEK_END:
        device->seek( device->size() + offset );
        break;
    }

    return device->pos();
}

int tiffCloseProc( thandle_t handle )
{
    Q_UNUSED( handle )
    return 0;
}

toff_t tiffSizeProc( thandle_t handle )
{
    QIODevice *device = static_cast< QIODevice * >( handle );
    return device->size();
}

int tiffMapProc( thandle_t, tdata_t *, toff_t * )
{
    return 0;
}

void tiffUnmapProc( thandle_t, tdata_t, toff_t )
{
}

static void adaptSizeToResolution( TIFF *tiff, ttag_t whichres, double dpi, uint32 *size )
{
    float resvalue = 1.0;
    uint16 resunit = 0;
    if ( !TIFFGetField( tiff, whichres, &resvalue )
            || !TIFFGetFieldDefaulted( tiff, TIFFTAG_RESOLUTIONUNIT, &resunit ) )
        return;

    float newsize = *size / resvalue;
    switch ( resunit ) {
    case RESUNIT_INCH:
        *size = (uint32)( newsize * dpi );
        break;
    case RESUNIT_CENTIMETER:
        *size = (uint32)( newsize * 10.0 / 25.4 * dpi );
        break;
    case RESUNIT_NONE:
        break;
    }
}

class DocummentTiffPrivate: public DocummentBasePrivate
{
//    Q_OBJECT
public:
    DocummentTiffPrivate(DocummentTiff *parent): DocummentBasePrivate(parent)
    {
        document = nullptr;
    }

    ~DocummentTiffPrivate()
    {
        if (nullptr != document) {
            TIFFClose(document);
            document = nullptr;
        }
    }

//    stFileInfo m_fileinfo;
    TIFF *document;
    Q_DECLARE_PUBLIC(DocummentTiff)
protected slots:
    void loadDocumment(QString filepath) override;
private:
    void setBasicInfo(const QString &filepath);
};

void DocummentTiffPrivate::loadDocumment(QString filepath)
{
    Q_Q(DocummentTiff);

    m_pages.clear();
    QFile *qfile = new QFile( filepath );
    if (!qfile->open( QIODevice::ReadOnly)) return;

    QIODevice *dev = qfile;
    QByteArray data = QFile::encodeName( QFileInfo( *qfile ).fileName());
    setBasicInfo(filepath);
    document = TIFFClientOpen( data.constData(), "r", dev,
                               tiffReadProc, tiffWriteProc, tiffSeekProc,
                               tiffCloseProc, tiffSizeProc,
                               tiffMapProc, tiffUnmapProc );
    if (document) {
        tdir_t dirs = TIFFNumberOfDirectories(document);
        for (int i = 0; i < dirs; i++) {
            PageTiff *page = new PageTiff(q);
            page->setPage(i, document);
            m_pages.append((PageBase *)page);
        }
    }

    if (m_pages.size() > 0) {
        m_imagewidth = m_pages.at(0)->getOriginalImageWidth();
        m_imageheight = m_pages.at(0)->getOriginalImageHeight();
    }
    setBasicInfo(filepath);
    emit signal_docummentLoaded(true);
}


void DocummentTiffPrivate::setBasicInfo(const QString &filepath)
{
    QFileInfo info(filepath);
    m_fileinfo->size = info.size();
    m_fileinfo->CreateTime = info.birthTime();
    m_fileinfo->ChangeTime = info.lastModified();
    m_fileinfo->strAuther = info.owner();
    m_fileinfo->strFilepath = info.filePath();
    if (document) {
        int major = 0, minor = 0;
        m_fileinfo->strFormat.arg("Tiff v.%1.%2", major, minor);
    }
}


DocummentTiff::DocummentTiff(DWidget *parent):
    DocummentBase(new DocummentTiffPrivate(this), parent)
{

}

bool DocummentTiff::loadDocumment(QString filepath)
{

    emit signal_loadDocumment(filepath);
    return true;
}

bool DocummentTiff::bDocummentExist()
{
    Q_D(DocummentTiff);
    if (!d->document) {
        return false;
    }
    return true;
}

//bool DocummentTiff::getImage(int pagenum, QImage &image, double width, double height)
//{
//    Q_D(DocummentTiff);
//    return d->m_pages.at(pagenum)->getInterFace()->getImage(image, width, height);
//}

//void DocummentTiff::docBasicInfo(stFileInfo &info)
//{
//    Q_D(DocummentTiff);
//    info = d->m_fileinfo;
//}
//bool DocumenTiff::openFile(QString filepath)
//{
//    m_pages.clear();
//    QFile *qfile = new QFile( filepath );
//    if (!qfile->open( QIODevice::ReadOnly)) return  false;

//    QIODevice *dev = qfile;
//    QByteArray data = QFile::encodeName( QFileInfo( *qfile ).fileName());
//    document = TIFFClientOpen( data.constData(), "r", dev,
//                               tiffReadProc, tiffWriteProc, tiffSeekProc,
//                               tiffCloseProc, tiffSizeProc,
//                               tiffMapProc, tiffUnmapProc );
//    if (document) {
//        tdir_t dirs = TIFFNumberOfDirectories(document);
//        tdir_t realdirs = 0;
//        uint32 width = 0;
//        uint32 height = 0;
//        const QSizeF dpi(76, 76);
//        for (int i = 0; i < dirs; i++) {
//            DWidget *qwidget = new DWidget(this);
//            QHBoxLayout *qhblayout = new QHBoxLayout(qwidget);
//            qhblayout->setAlignment(qwidget, Qt::AlignCenter);
//            qwidget->setLayout(qhblayout);
//            m_vboxLayout.addWidget(qwidget);
//            m_vboxLayout.setAlignment(&m_widget, Qt::AlignCenter);
//            qwidget->setMouseTracking(true);
//            m_widgets.append(qwidget);

//            PageTiff *page = new PageTiff(this);
//            page->setPage(i, document);
//            m_pages.append((PageBase *)page);
//        }
//    }
//    for (int i = 0; i < m_pages.size(); i++) {
//        m_pages.at(i)->setScaleAndRotate(m_scale, m_rotate);
//    }
//    setViewModeAndShow(m_viewmode);
//    initConnect();
//    if (m_threadloaddoc.isRunning())
//        m_threadloaddoc.setRestart();
//    else
//        m_threadloaddoc.start();

//    return  true;
//}

//bool DocumenTiff::loadPages()
//{
//    if (!document && m_pages.size() ==TIFFNumberOfDirectories(document))
//        return false;

//    int startnum = m_currentpageno - 3;
//    if (startnum < 0) {
//        startnum = 0;
//    }
//    int endnum = startnum + 7;
//    if (endnum > m_pages.size()) {
//        endnum = m_pages.size();
//    }
//    for (int i = startnum; i < endnum; i++) {
//        m_pages.at(i)->showImage(m_scale, m_rotate);
//    }
//    return true;
//}
