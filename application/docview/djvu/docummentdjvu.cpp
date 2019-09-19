#include "docummentdjvu.h"
#include "pagedjvu.h"
#include "docview/publicfunc.h"
#include <DScrollBar>
#include <QImage>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDebug>

void waitForMessageTag(ddjvu_context_t *context, ddjvu_message_tag_t tag)
{
    ddjvu_message_wait(context);

    while (true) {
        ddjvu_message_t *message = ddjvu_message_peek(context);

        if (message != 0) {
            if (message->m_any.tag == tag) {
                break;
            }

            ddjvu_message_pop(context);
        } else {
            break;
        }
    }
}

DocummentDJVU::DocummentDJVU(DWidget *parent): DocummentBase(parent),
    document(nullptr),
    m_listsearch(),
    m_fileinfo(),
    m_pageByName(),
    m_titleByIndex()
{
}

bool DocummentDJVU::openFile(QString filepath)
{
    m_context = ddjvu_context_create("deepin_reader");

    if (m_context == 0) {
        return false;
    }

#if DDJVUAPI_VERSION >= 19

    document = ddjvu_document_create_by_filename_utf8(m_context, filepath.toUtf8(), FALSE);

#else

    document = ddjvu_document_create_by_filename(context, QFile::encodeName(filepath), FALSE);

#endif // DDJVUAPI_VERSION

    if (document == 0) {
        ddjvu_context_release(m_context);

        return false;
    }

    waitForMessageTag(m_context, DDJVU_DOCINFO);

    if (ddjvu_document_decoding_error(document)) {
        ddjvu_document_release(document);
        ddjvu_context_release(m_context);

        return false;
    }
    unsigned int mask[] = {0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000};

    m_format = ddjvu_format_create(DDJVU_FORMAT_RGBMASK32, 4, mask);
    ddjvu_format_set_row_order(m_format, 1);
    ddjvu_format_set_y_direction(m_format, 1);
    setBasicInfo(filepath);
    donotneedreloaddoc = true;
    m_pages.clear();
    qDebug() << "djvu numPages :" << ddjvu_document_get_pagenum(document);
    for (int i = 0; i < ddjvu_document_get_pagenum(document); i++) {
        DWidget *qwidget = new DWidget(this);
        QHBoxLayout *qhblayout = new QHBoxLayout(qwidget);
        qhblayout->setAlignment(qwidget, Qt::AlignCenter);
        qwidget->setLayout(qhblayout);
        m_vboxLayout.addWidget(qwidget);
        //        m_vboxLayout.addWidget(m_pages.at(i));
        m_vboxLayout.setAlignment(&m_widget, Qt::AlignCenter);
        qwidget->setMouseTracking(true);
        m_widgets.append(qwidget);

        PageDJVU *page = new PageDJVU(this);
        page->setPage(i);
        m_pages.append((PageBase *)page);
    }

    for (int i = 0; i < m_pages.size(); i++) {
        m_pages.at(i)->setScaleAndRotate(m_scale, m_rotate);
    }
    setViewModeAndShow(m_viewmode);
    donotneedreloaddoc = false;
    if (m_threadloaddoc.isRunning())
        m_threadloaddoc.setRestart();
    else
        m_threadloaddoc.start();
    if (m_threadloadwords.isRunning())
        m_threadloadwords.setRestart();
    else
        m_threadloadwords.start();

    return true;
}

bool DocummentDJVU::loadPages()
{
    if (!document && m_pages.size() == ddjvu_document_get_pagenum(document))
        return false;
    qDebug() << "loadPages";
    //    for (int i = 0; i < m_pages.size(); i++) {
    int startnum = m_currentpageno - 3;
    if (startnum < 0) {
        startnum = 0;
    }
    int endnum = startnum + 7;
    if (endnum > m_pages.size()) {
        endnum = m_pages.size();
    }
    for (int i = startnum; i < endnum; i++) {
        m_pages.at(i)->showImage(m_scale, m_rotate);
    }
    return true;
}

bool DocummentDJVU::loadWords()
{
    if (!document && m_pages.size() == ddjvu_document_get_pagenum(document))
        return false;
    qDebug() << "loadWords";
    for (int i = 0; i < m_pages.size(); i++) {
        PageDJVU *pdjvu = (PageDJVU *)m_pages.at(i);
        pdjvu->loadWords();
        pdjvu->loadLinks();
    }
    return true;
}

void DocummentDJVU::removeAllAnnotation()
{
    if (!document)return;
//    for (int i = 0; i < document->numPages(); ++i) {
//        QList<Poppler::Annotation *> listannote = document->page(i)->annotations();
//        foreach (Poppler::Annotation *atmp, listannote) {
//            document->page(i)->removeAnnotation(atmp);
//        }
//    }
//    scaleAndShow(m_scale, m_rotate);
}

QString DocummentDJVU::removeAnnotation(const QPoint &startpos)
{
    //暂时只处理未旋转
    QPoint pt = startpos;
    int page = pointInWhichPage(pt);
    if (page < 0) return "";
    return static_cast<PageDJVU *>(m_pages.at(page))->removeAnnotation(pt);
}

void DocummentDJVU::removeAnnotation(const QString &struuid)
{
    return static_cast<PageDJVU *>(m_pages.at(currentPageNo()))->removeAnnotation(struuid);
}

QString DocummentDJVU::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    QPoint pt = startpos;
    int page = pointInWhichPage(pt);
    if (page < 0) return "";
    return static_cast<PageDJVU *>(m_pages.at(page))->addAnnotation(pt);
}

void DocummentDJVU::search(const QString &strtext, QMap<int, stSearchRes> &resmap, QColor color)
{
//    //先清理
//    if (m_listsearch.size() > 0) {
//        clearSearch();
//    }

//    for (int i = 0; i < document->numPages(); ++i) {
//        stSearchRes stres;
//        searchHightlight(document->page(i), strtext, stres, color);
//        if (stres.listtext.size() > 0)
//            resmap.insert(i, stres);
//    }
//    static_cast<PageDJVU *>(m_pages.at(currentPageNo()))->showImage(m_scale, m_rotate);
//    scaleAndShow(m_scale, m_rotate); //全部刷新
}

bool DocummentDJVU::save(const QString &filePath, bool withChanges)
{
    // Save document to temporary file...
    QTemporaryFile temporaryFile;
    temporaryFile.setFileTemplate(temporaryFile.fileTemplate() + QLatin1String(".") + QFileInfo(filePath).suffix());
    if (!temporaryFile.open()) {
        return false;
    }

    temporaryFile.close();

    if (!pdfsave(temporaryFile.fileName(), withChanges)) {
        return false;
    }

    // Copy from temporary file to actual file...
    QFile file(filePath);

    if (!temporaryFile.open()) {
        return false;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    if (!PublicFunc::copyFile(temporaryFile, file)) {
        return false;
    }

    if (withChanges) {
        // m_bModified = false;//reset modify status
        setBasicInfo(filePath);
    }

    return true;
}

bool DocummentDJVU::pdfsave(const QString &filePath, bool withChanges) const
{
//    QScopedPointer< Poppler::PDFConverter > pdfConverter(document->pdfConverter());

//    pdfConverter->setOutputFileName(filePath);

//    Poppler::PDFConverter::PDFOptions options = pdfConverter->pdfOptions();

//    if (withChanges) {
//        options |= Poppler::PDFConverter::WithChanges;
//    }

//    pdfConverter->setPDFOptions(options);

//    return pdfConverter->convert();
    return  false;

}

void DocummentDJVU::clearSearch()
{
//    for (int i = 0; i < document->numPages(); ++i) {
//        foreach (Poppler::Annotation *ptmp, m_listsearch) {
//            document->page(i)->removeAnnotation(ptmp);
//        }
//        //refresh(i);
//    }
//    scaleAndShow(m_scale, m_rotate);
}

void DocummentDJVU::searchHightlight(Poppler::Page *page, const QString &strtext, stSearchRes &stres, const QColor &color)
{
    if (nullptr == page) return;
    QList<QRectF> listrect = page->search(strtext);
    if (listrect.size() <= 0)return;

    if (listrect.size() <= 0)return;
    Poppler::Annotation::Style style;
    style.setColor(color);

    Poppler::Annotation::Popup popup;
    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

    Poppler::HighlightAnnotation *annotation = new Poppler::HighlightAnnotation();

    Poppler::HighlightAnnotation::Quad quad;
    QList<Poppler::HighlightAnnotation::Quad> qlistquad;
    QRectF rec, recboundary;
    foreach (rec, listrect) {
        //获取搜索结果附近文字
        QRectF rctext = rec;
        rctext.setX(rctext.x() - 40);
        rctext.setWidth(rctext.width() + 80);
        stres.listtext.push_back(page->text(rctext));
        recboundary.setTopLeft(QPointF(rec.left() / page->pageSizeF().width(),
                                       rec.top() / page->pageSizeF().height()));
        recboundary.setTopRight(QPointF(rec.right() / page->pageSizeF().width(),
                                        rec.top() / page->pageSizeF().height()));
        recboundary.setBottomLeft(QPointF(rec.left() / page->pageSizeF().width(),
                                          rec.bottom() / page->pageSizeF().height()));
        recboundary.setBottomRight(QPointF(rec.right() / page->pageSizeF().width(),
                                           rec.bottom() / page->pageSizeF().height()));

        qDebug() << "**" << rec << "**";
        quad.points[0] = recboundary.topLeft();
        quad.points[1] = recboundary.topRight();
        quad.points[2] = recboundary.bottomRight();
        quad.points[3] = recboundary.bottomLeft();
        qlistquad.append(quad);
    }
    annotation->setHighlightQuads(qlistquad);
    //annotation->setBoundary(recboundary);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    page->addAnnotation(annotation);
    m_listsearch.append(annotation);
}

void DocummentDJVU::refreshOnePage(int ipage)
{
    if (!document)
        return ;
    PageDJVU *ppdf = (PageDJVU *)m_pages.at(ipage);
    ppdf->showImage(m_scale, m_rotate);
}

void DocummentDJVU::setBasicInfo(const QString &filepath)
{
    QFileInfo info(filepath);
    m_fileinfo.size = info.size();
    m_fileinfo.CreateTime = info.birthTime();
    m_fileinfo.ChangeTime = info.lastModified();
    m_fileinfo.strAuther = info.owner();
    m_fileinfo.strFilepath = info.filePath();
    if (document) {
        int major, minor;
        //document->getPdfVersion(&major, &minor);
        m_fileinfo.strFormat.arg("PDF v.%1.%2", major, minor);
//        m_fileinfo.strKeyword = document->info(QStringLiteral("Keywords"));
//        m_fileinfo.strTheme = document->title();
//        m_fileinfo.strProducter = document->producer();
//        m_fileinfo.strCreater = document->creator();
//        m_fileinfo.bsafe = document->isEncrypted();
//        m_fileinfo.iWidth = document->page(0)->pageSize().width();
//        m_fileinfo.iHeight = document->page(0)->pageSize().height();
//        m_fileinfo.iNumpages = document->numPages();

        /*---------djyu code--------*/
        for (int index = 0, count = ddjvu_document_get_filenum(document); index < count; ++index) {
            ddjvu_fileinfo_t fileinfo;

            if (ddjvu_document_get_fileinfo(document, index, &fileinfo) != DDJVU_JOB_OK || fileinfo.type != 'P') {
                continue;
            }

            const QString id = QString::fromUtf8(fileinfo.id);
            const QString name = QString::fromUtf8(fileinfo.name);
            const QString title = QString::fromUtf8(fileinfo.title);

            m_pageByName[id] = m_pageByName[name] = m_pageByName[title] = fileinfo.pageno + 1;

            if (!title.endsWith(".djvu", Qt::CaseInsensitive) && !title.endsWith(".djv", Qt::CaseInsensitive)) {
                m_titleByIndex[fileinfo.pageno] = title;
            }
        }

        m_pageByName.squeeze();
        m_titleByIndex.squeeze();
    }
}

bool DocummentDJVU::bDocummentExist()
{
    if (!document) {
        return false;
    }
    return true;
}

bool DocummentDJVU::getImage(int pagenum, QImage &image, double width, double height)
{
    return m_pages.at(pagenum)->getImage(image, width, height);
}

void DocummentDJVU::docBasicInfo(stFileInfo &info)
{
    info = m_fileinfo;
}

bool DocummentDJVU::annotationClicked(const QPoint &pos, QString &strtext)
{
    QPoint pt(pos);
    int ipage = pointInWhichPage(pt);
    if (ipage < 0) return  false;
    return static_cast<PageDJVU>(m_pages.at(ipage)).annotationClicked(pt, strtext);
}

void DocummentDJVU::title(QString &title)
{
//    title = document->title();
}

ddjvu_document_t *DocummentDJVU::getDocument()
{
    return document;
}
ddjvu_context_t *DocummentDJVU::getContext()
{
    return m_context;
}
ddjvu_format_t *DocummentDJVU::getFormat()
{
    return m_format;
}
QHash< QString, int > DocummentDJVU::getPageByName()
{
    return m_pageByName;
}
