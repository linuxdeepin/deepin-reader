#include "docummentproxy.h"
#include "docummentfactory.h"
#include "publicfunc.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QMutex>
static QMutex mutexlockgetimage;
DocummentProxy *DocummentProxy::s_pDocProxy = nullptr;
QObject *DocummentProxy::m_curobj = nullptr;
QMap<QObject *, stCurDocProxy> DocummentProxy::m_proxymap;
DocummentProxy::DocummentProxy(QObject *parent)
    : QObject(parent),
      m_path(""),
      m_documment(nullptr),
      bcloseing(false)
{
    qwfather = (DWidget *)parent;
}

bool DocummentProxy::CreateInstance(QObject *parent)
{
    bool bsucess = false;
    if (parent) {
        stCurDocProxy st;
        st.pwgt = (DWidget *)parent;
        m_proxymap.insert(parent, st);
        m_curobj = parent;
    }
    return  bsucess;
}

DocummentProxy *DocummentProxy::instance(QObject *parent)
{
    if (nullptr == parent && nullptr == s_pDocProxy) {
        return nullptr;
    }
    if (nullptr != parent && nullptr == s_pDocProxy) {
        s_pDocProxy = new DocummentProxy(parent);
    }
//    return  s_pDocProxy;
//    if (m_proxymap.size() <= 0)
//        return nullptr;
//    return  m_proxymap.take(m_curobj).proxy;
}

bool DocummentProxy::openFile(DocType_EM type, QString filepath, unsigned int ipage, RotateType_EM rotatetype, double scale, ViewMode_EM viewmode)
{
    QMutexLocker locker(&mutexlockgetimage);
    bool bre = false;
    m_path = filepath;
    if (nullptr != m_documment) {
        bcloseing = true;
        m_documment->stopLoadPageThread();
        delete m_documment;
        m_documment = nullptr;
    }

    m_documment = DocummentFactory::creatDocumment(type, qwfather);
    if (m_documment) {
        connect(m_documment, SIGNAL(signal_pageChange(int)), this, SLOT(slot_pageChange(int)));
        connect(this, SIGNAL(signal_pageJump(int)), m_documment, SLOT(pageJump(int)));
        connect(m_documment, SIGNAL(signal_searchRes(stSearchRes)), this, SIGNAL(signal_searchRes(stSearchRes)));
        connect(m_documment, SIGNAL(signal_searchover()), this, SIGNAL(signal_searchover()));
//      connect(this, SIGNAL(signal_mouseSelectText(QPoint, QPoint)), m_documment, SLOT(mouseSelectText(QPoint, QPoint)));
        connect(this, SIGNAL(signal_setScaleRotateViewModeAndShow(double, RotateType_EM, ViewMode_EM)), m_documment, SLOT(setScaleRotateViewModeAndShow(double, RotateType_EM, ViewMode_EM)));
        connect(this, SIGNAL(signal_scaleAndShow(double, RotateType_EM)), m_documment, SLOT(scaleAndShow(double, RotateType_EM)));
        connect(this, SIGNAL(signal_setViewModeAndShow(ViewMode_EM)), m_documment, SLOT(setViewModeAndShow(ViewMode_EM)));
        connect(m_documment, &DocummentBase::signal_bookMarkStateChange, this, [ = ](int page, bool state) {
            emit signal_bookMarkStateChange(page, state);
        });
        connect(m_documment, &DocummentBase::signal_openResult, this, [ = ](bool result) {
            emit signal_openResult(result);
        });
        connect(m_documment, &DocummentBase::signal_autoplaytoend, this, &DocummentProxy::signal_autoplaytoend);
        bre = m_documment->openFile(m_path, ipage, rotatetype, scale, viewmode);
    }
    bcloseing = false;
    return bre;
//    return startOpenFile();
}

bool DocummentProxy::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    if (!m_documment || bcloseing)
        return false;
    //qDebug() << "setSelectTextStyle";
    return m_documment->setSelectTextStyle(paintercolor, pencolor, penwidth);
}

bool DocummentProxy::mouseSelectText(QPoint start, QPoint stop)
{
    if (!m_documment || bcloseing)
        return false;
    // qDebug() << "mouseSelectText";
    mouseSelectTextClear();
    return m_documment->mouseSelectText(start, stop);
}

void DocummentProxy::mouseSelectTextClear()
{
    m_documment->mouseSelectTextClear();
}

bool DocummentProxy::mouseBeOverText(QPoint point)
{
    if (!m_documment || bcloseing)
        return false;
    //qDebug() << "mouseBeOverText";
    return m_documment->mouseBeOverText(point);
}

void DocummentProxy::setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode)
{
    if (!m_documment || bcloseing)
        return;
    qDebug() << "setScaleRotateViewModeAndShow";
    mouseSelectTextClear();
    emit signal_setScaleRotateViewModeAndShow(scale, rotate, viewmode);
}
void DocummentProxy::scaleRotateAndShow(double scale, RotateType_EM rotate)
{
    if (!m_documment || bcloseing)
        return;
    mouseSelectTextClear();
//    m_documment->scaleAndShow(scale, rotate);
    qDebug() << "scaleRotateAndShow";
    emit signal_scaleAndShow(scale, rotate);
}

QPoint DocummentProxy::global2RelativePoint(QPoint globalpoint)
{
    if (!m_documment || bcloseing)
        return QPoint();
    return m_documment->global2RelativePoint(globalpoint);
}

bool DocummentProxy::getImage(int pagenum, QImage &image, double width, double height)
{
    bool bre = false;
    QMutexLocker locker(&mutexlockgetimage);
    if (!m_documment || bcloseing) {
        return bre;
    }
    bre = m_documment->getImage(pagenum, image, width, height);
    return bre;
}

int DocummentProxy::getPageSNum()
{
    if (!m_documment || bcloseing)
        return false;
    // qDebug() << "getPageSNum";
    return m_documment->getPageSNum();
}

bool DocummentProxy::setViewModeAndShow(ViewMode_EM viewmode)
{
    if (!m_documment || bcloseing)
        return false;
    mouseSelectTextClear();
    qDebug() << "DocummentProxy::setViewModeAndShow";
    emit signal_setViewModeAndShow(viewmode);
    return true;
}

bool DocummentProxy::showMagnifier(QPoint point)
{
    if (!m_documment || bcloseing)
        return false;
    // qDebug() << "showMagnifier";
    return  m_documment->showMagnifier(point);
}

bool DocummentProxy::closeMagnifier()
{
    if (!m_documment || bcloseing)
        return false;
    // qDebug() << "closeMagnifier";
    m_documment->magnifierClear();
    return true;
}

bool DocummentProxy::setMagnifierStyle(QColor magnifiercolor, int magnifierradius, int magnifierringwidth, double magnifierscale)
{
    if (!m_documment || bcloseing)
        return false;
    //qDebug() << "setMagnifierStyle";
    return  m_documment->setMagnifierStyle(magnifiercolor, magnifierradius, magnifierringwidth, magnifierscale);
}
QString DocummentProxy::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    if (!m_documment || bcloseing)
        return QString("");
    return m_documment->addAnnotation(startpos, endpos, color);
}

QString DocummentProxy::addIconAnnotation(const QPoint &pos, const QColor &color, TextAnnoteType_Em type)
{
    if (!m_documment || bcloseing)
        return QString("");

    return m_documment->addTextAnnotation(pos, color, type);
}

void DocummentProxy::moveIconAnnotation(const QString &uuid, const QPoint &pos)
{
    if (!m_documment || bcloseing)
        return ;
    // m_documment->moveIconAnnotation(uuid, pos);
}

bool DocummentProxy::iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->iconAnnotationClicked(pos, strtext, struuid);
}

bool DocummentProxy::mouseovericonAnnotation(const QPoint &pos)
{
    return  false;
    if (!m_documment || bcloseing)
        return false;
    QString strtext, struuid;
    return m_documment->iconAnnotationClicked(pos, strtext, struuid);
}

QString DocummentProxy::pagenum2label(int index)
{
    if (!m_documment)
        return  QString();
    return m_documment->pagenum2label(index);
}

int DocummentProxy::label2pagenum(QString label)
{
    if (!m_documment)
        return  -1;
    return  m_documment->label2pagenum(label);
}

bool DocummentProxy::haslabel()
{
    if (!m_documment)
        return  false;
    return m_documment->haslabel();
}

bool DocummentProxy::save(const QString &filepath, bool withChanges)
{
    if (!m_documment || bcloseing)
        return false;

    if (!m_documment->save(filepath, withChanges)) {
        return false;
    }
    return m_documment->freshFile(m_path);
//        return m_documment->save(filepath, withChanges);
}

bool DocummentProxy::saveas(const QString &filepath, bool withChanges)
{
    if (m_documment && !bcloseing && m_documment->saveas(filepath, withChanges)) {
//        qDebug() << "saveas success";
//        return openFile(DocType_PDF, filepath);
        return m_documment->freshFile(filepath);
    }
//    qDebug() << "saveas failed";
    return false;
}

void DocummentProxy::search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color)
{
    if (!m_documment || bcloseing)
        return ;
//    qDebug() << "search";
    m_documment->search(strtext, color);
}

void DocummentProxy::clearsearch()
{
    if (!m_documment || bcloseing)
        return ;
    // qDebug() << "clearsearch";
    m_documment->clearSearch();
}

int DocummentProxy::currentPageNo()
{
    if (!m_documment || bcloseing)
        return -1;
    //qDebug() << "currentPageNo";
    return m_documment->getCurrentPageNo();
}

bool DocummentProxy::pageJump(int pagenum)
{
    if (!m_documment || bcloseing)
        return false;
//    qDebug() << "pageJump";
    emit signal_pageJump(pagenum);
    return true;
}

void DocummentProxy::docBasicInfo(stFileInfo &info)
{
    if (!m_documment || bcloseing)
        return ;
    //qDebug() << "docBasicInfo";
    m_documment->docBasicInfo(info);
}

QString DocummentProxy::removeAnnotation(const QPoint &startpos, AnnoteType_Em type)
{
    if (!m_documment || bcloseing)
        return "";

    QString uuid = m_documment->removeAnnotation(startpos, type);
    return uuid;
}

void DocummentProxy::removeAnnotation(const QString &struuid, int ipage)
{
    if (!m_documment || bcloseing)
        return ;
    m_documment->removeAnnotation(struuid, ipage);
}

void DocummentProxy::slot_pageChange(int pageno)
{
    emit signal_pageChange(pageno);
}

bool DocummentProxy::pageMove(double mvx, double mvy)
{
    if (!m_documment || bcloseing)
        return false;
    // qDebug() << "pageMove";
    m_documment->pageMove(mvx, mvy);
    return true;
}

void DocummentProxy::title(QString &title)
{
    if (!m_documment || bcloseing)
        return;
//    qDebug() << "title";
    m_documment->title(title);
}

Page::Link *DocummentProxy::mouseBeOverLink(QPoint point)
{
    if (!m_documment || bcloseing)
        return nullptr;
    // qDebug() << "mouseBeOverLink";
    return m_documment->mouseBeOverLink(point);
}

bool DocummentProxy::getSelectTextString(QString &st)
{
    if (!m_documment || bcloseing)
        return false;
    //qDebug() << "getSelectTextString";
    return m_documment->getSelectTextString(st);
}


bool DocummentProxy::showSlideModel()
{
    if (!m_documment || bcloseing)
        return false;
//    qDebug() << "showSlideModel";
    return m_documment->showSlideModel();
}
bool DocummentProxy::exitSlideModel()
{
    if (!m_documment || bcloseing)
        return false;
//    qDebug() << "exitSlideModel";
    return m_documment->exitSlideModel();
}

void DocummentProxy::findNext()
{
    if (m_documment || !bcloseing) {
        m_documment->findNext();
    }
}

void DocummentProxy::findPrev()
{
    if (m_documment || !bcloseing) {
        m_documment->findPrev();
    }
}

void DocummentProxy::setAnnotationText(int ipage, const QString &struuid, const QString &strtext)
{
    if (m_documment || !bcloseing) {
//        qDebug() << "setAnnotationText";
        m_documment->setAnnotationText(ipage, struuid, strtext);
    }
}

void DocummentProxy::getAnnotationText(const QString &struuid, QString &strtext, int ipage)
{
    if (m_documment || !bcloseing) {
//        qDebug() << "getAnnotationText";
        m_documment->getAnnotationText(struuid, strtext, ipage);
    }
}

double DocummentProxy::adaptWidthAndShow(double width)
{
    if (!m_documment || bcloseing)
        return -1;
    qDebug() << "adaptWidthAndShow width:" << width;
    return m_documment->adaptWidthAndShow(width);
}

double DocummentProxy::adaptHeightAndShow(double height)
{
    if (!m_documment || bcloseing)
        return -1;
    qDebug() << "adaptWidthAndShow height:" << height;
    return m_documment->adaptHeightAndShow(height);
}

bool DocummentProxy::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
//    qDebug() << "DocummentProxy::annotationClicked" << pos;
    if (m_documment) {
        return m_documment->annotationClicked(pos, strtext, struuid);
    }
    return false;
}

void DocummentProxy::getAllAnnotation(QList<stHighlightContent> &listres)
{
    if (m_documment) {
        return m_documment->getAllAnnotation(listres);
    }
}

int DocummentProxy::pointInWhichPage(QPoint pos)
{
    if (m_documment) {
        return  m_documment->pointInWhichPage(pos);
    }
    return  -1;
}

void DocummentProxy::jumpToHighLight(const QString &uuid, int ipage)
{
//    qDebug() << "DocummentProxy::jumpToHighLight";
    if (m_documment) {
        m_documment->jumpToHighLight(uuid, ipage);
    }
}

void DocummentProxy::jumpToOutline(const qreal &realleft, const qreal &realtop, unsigned int ipage)
{
    if (m_documment)
        m_documment->jumpToOutline(realleft, realtop, ipage);
}

bool DocummentProxy::closeFile()
{
    QMutexLocker locker(&mutexlockgetimage);
//    qDebug() << "closeFile";
    if (!m_documment || bcloseing)
        return false;
    bcloseing = true;
    m_documment->stopLoadPageThread();
    delete m_documment;
    m_documment = nullptr;
//    if (m_documment->isWordsBeLoad()) {
//        bclosefile = true;
//        if (!threadwaitloadwordsend.isRunning()) {
//            threadwaitloadwordsend.setDoc(m_documment);
//            threadwaitloadwordsend.start();
//        }
//        return true;
//    }
//    bcloseing = false;
    return true;
}

void DocummentProxy::closeFileAndWaitThreadClearEnd()
{
    QMutexLocker locker(&mutexlockgetimage);
    if (!m_documment)
        return;

    bcloseing = true;
    m_documment->stopLoadPageThread();
    if (nullptr != m_documment) {
        delete m_documment;
        m_documment = nullptr;
    }
}

bool DocummentProxy::setBookMarkState(int page, bool state)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->setBookMarkState(page, state);
}

void DocummentProxy::changeAnnotationColor(int ipage, const QString uuid, const QColor &color)
{
    if (m_documment)
        m_documment->changeAnnotationColor(ipage, uuid, color);
}

void DocummentProxy::selectAllText()
{
    if (m_documment)
        m_documment->selectAllText();
}


void DocummentProxy::setAutoPlaySlide(bool autoplay, int timemsec)
{
    if (!m_documment || bcloseing)
        return;
    return m_documment->setAutoPlaySlide(autoplay, timemsec);
}
bool DocummentProxy::getAutoPlaySlideStatu()
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->getAutoPlaySlideStatu();
}

bool DocummentProxy::isOpendFile()
{
    bool bopen = false;
    if (m_documment)
        bopen = true;
    return  bopen;
}

void DocummentProxy::setViewFocus()
{
    if (m_documment)
        m_documment->setViewFocus();
}

double DocummentProxy::getMaxZoomratio()
{
    if (m_documment)
        return   m_documment->getMaxZoomratio();
    return  0.0;
}

Outline DocummentProxy::outline()
{
    if (!m_documment || bcloseing)
        return Outline();
    return  m_documment->outline();
}
