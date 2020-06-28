#include "docummentproxy.h"
#include "docummentfactory.h"
#include "publicfunc.h"
#include "DocSheet.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QMutex>

static QMutex mutexlockgetimage;

DocummentProxy::DocummentProxy(DocSheet *sheet, QObject *parent)
    : QObject(parent)
    , m_sheet(sheet)
    , m_path("")
    , m_documment(nullptr)
    , bcloseing(false)
{
    pwgt = static_cast<DWidget *>(parent);
}

bool DocummentProxy::openFile(Dr::FileType type, QString filepath, unsigned int ipage, RotateType_EM rotatetype, double scale, ViewMode_EM viewmode)
{
    bool bre = false;
    m_path = filepath;
    if (nullptr != m_documment) {
        bcloseing = true;
        m_documment->stopLoadPageThread();
        delete m_documment;
        m_documment = nullptr;
    }

    m_documment = DocummentFactory::creatDocumment(type, pwgt);
    if (m_documment) {
        connect(m_documment, SIGNAL(signal_pageChange(int)), this, SLOT(slot_pageChange(int)));
        connect(this, SIGNAL(signal_pageJump(int)), m_documment, SLOT(pageJump(int)));
        connect(m_documment, SIGNAL(signal_searchRes(stSearchRes)), this, SIGNAL(signal_searchRes(stSearchRes)));
        connect(m_documment, SIGNAL(signal_searchover()), this, SIGNAL(signal_searchover()));
        connect(this, SIGNAL(signal_scaleAndShow(double, RotateType_EM)), m_documment, SLOT(scaleAndShow(double, RotateType_EM)));
        connect(this, SIGNAL(signal_setViewModeAndShow(ViewMode_EM)), m_documment, SLOT(setViewModeAndShow(ViewMode_EM)));
        connect(m_documment, SIGNAL(sigPageBookMarkButtonClicked(int, bool)), this, SIGNAL(sigPageBookMarkButtonClicked(int, bool)));
        connect(m_documment, SIGNAL(signal_openResult(bool)), this, SIGNAL(signal_openResult(bool)));
        bre = m_documment->openFile(m_path, ipage, rotatetype, scale, viewmode);
    }
    bcloseing = false;
    return bre;
}

bool DocummentProxy::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->setSelectTextStyle(paintercolor, pencolor, penwidth);
}

bool DocummentProxy::mouseSelectText(QPoint start, QPoint stop)
{
    if (!m_documment || bcloseing)
        return false;
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
    return m_documment->mouseBeOverText(point);
}

void DocummentProxy::setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode)
{
    if (!m_documment || bcloseing)
        return;
    mouseSelectTextClear();
    m_documment->setScaleRotateViewModeAndShow(scale, rotate, viewmode);
}
void DocummentProxy::scaleRotateAndShow(double scale, RotateType_EM rotate)
{
    if (!m_documment || bcloseing)
        return;
    mouseSelectTextClear();
    emit signal_scaleAndShow(scale, rotate);
}

QPoint DocummentProxy::global2RelativePoint(QPoint globalpoint)
{
    if (!m_documment || bcloseing)
        return QPoint();
    return m_documment->global2RelativePoint(globalpoint);
}

bool DocummentProxy::getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode)
{
    bool bre = false;
    QMutexLocker locker(&mutexlockgetimage);
    if (!m_documment || bcloseing) {
        return bre;
    }
    bre = m_documment->getImage(index, image, width, height, mode);
    return bre;
}

int DocummentProxy::getPageSNum()
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->getPageSNum();
}

bool DocummentProxy::setViewModeAndShow(ViewMode_EM viewmode)
{
    if (!m_documment || bcloseing)
        return false;
    mouseSelectTextClear();
    emit signal_setViewModeAndShow(viewmode);
    return true;
}

bool DocummentProxy::showMagnifier(QPoint point)
{
    if (!m_documment || bcloseing)
        return false;
    return  m_documment->showMagnifier(point);
}

bool DocummentProxy::closeMagnifier()
{
    if (!m_documment || bcloseing)
        return false;
    m_documment->magnifierClear();
    return true;
}

bool DocummentProxy::setMagnifierStyle(int magnifierradius, int magnifierringwidth, double magnifierscale)
{
    if (!m_documment || bcloseing)
        return false;
    return  m_documment->setMagnifierStyle(magnifierradius, magnifierringwidth, magnifierscale);
}
QString DocummentProxy::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    QString strres;
    if (!m_documment || bcloseing)
        return QString("");
    strres = m_documment->addAnnotation(startpos, endpos, color);
    return strres;
}

QString DocummentProxy::addIconAnnotation(const QPoint &pos, const QColor &color, TextAnnoteType_Em type)
{
    QString strres;
    if (!m_documment || bcloseing)
        return strres;
    strres = m_documment->addTextAnnotation(pos, color, type);
    return strres;
}

void DocummentProxy::moveIconAnnotation(const QString &, const QPoint &)
{
    if (!m_documment || bcloseing)
        return ;
}

void DocummentProxy::setActive(const bool &active)
{
    if (!m_documment || bcloseing)
        return;
    m_documment->setActive(active);
}

bool DocummentProxy::iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->iconAnnotationClicked(pos, strtext, struuid);
}

bool DocummentProxy::mouseovericonAnnotation(const QPoint &pos)
{
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

void DocummentProxy::setFocus()
{
    if (!m_documment)
        return;
    m_documment->setFocus();
}

bool DocummentProxy::save(const QString &filepath)
{
    if (!m_documment || bcloseing)
        return false;

    if (!m_documment->save(filepath)) {
        return false;
    }

    return m_documment->freshFile(filepath);        //需要内部重新打开,不然第二次保存会失败
}

bool DocummentProxy::saveas(const QString &filepath, bool withChanges)
{
    clearsearch();

    if (m_documment && !bcloseing && m_documment->saveas(filepath, withChanges)) {
        return m_documment->freshFile(filepath);
    }
    return false;
}

void DocummentProxy::search(const QString &strtext, const QColor &color)
{
    if (!m_documment || bcloseing)
        return ;
    m_documment->search(strtext, color);
}

void DocummentProxy::clearsearch()
{
    if (!m_documment || bcloseing)
        return ;
    m_documment->clearSearch();
}

int DocummentProxy::currentPageNo()
{
    if (!m_documment || bcloseing)
        return -1;
    return m_documment->getCurrentPageNo();
}

bool DocummentProxy::pageJump(int index)
{
    if (!m_documment || bcloseing)
        return false;
    emit signal_pageJump(index);
    return true;
}

void DocummentProxy::docBasicInfo(stFileInfo &info)
{
    if (!m_documment || bcloseing)
        return ;
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
    m_documment->pageMove(mvx, mvy);
    return true;
}

void DocummentProxy::title(QString &title)
{
    if (!m_documment || bcloseing)
        return;
    m_documment->title(title);
}

Page::Link *DocummentProxy::mouseBeOverLink(QPoint point)
{
    if (!m_documment || bcloseing)
        return nullptr;
    return m_documment->mouseBeOverLink(point);
}

bool DocummentProxy::getSelectTextString(QString &st)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->getSelectTextString(st);
}

bool DocummentProxy::getSelectTextString(QString &st, int &page)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->getSelectTextString(st, page);
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
        m_documment->setAnnotationText(ipage, struuid, strtext);
    }
}

void DocummentProxy::getAnnotationText(const QString &struuid, QString &strtext, int ipage)
{
    if (m_documment || !bcloseing) {
        m_documment->getAnnotationText(struuid, strtext, ipage);
    }
}

double DocummentProxy::adaptWidthAndShow(double width)
{
    if (!m_documment || bcloseing)
        return -1;

    return m_documment->adaptWidthAndShow(width);
}

double DocummentProxy::adaptHeightAndShow(double height)
{
    if (!m_documment || bcloseing)
        return -1;

    return m_documment->adaptHeightAndShow(height);
}

bool DocummentProxy::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
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

void DocummentProxy::jumpToHighLight(const QString &uuid, int index)
{
    if (m_documment) {
        m_documment->jumpToHighLight(uuid, index);
    }
}

void DocummentProxy::jumpToOutline(const qreal &realleft, const qreal &realtop, unsigned int ipage)
{
    if (m_documment)
        m_documment->jumpToOutline(realleft, realtop, static_cast<int>(ipage));
}

bool DocummentProxy::closeFile()
{
    QMutex mutexlockclose;
    QMutexLocker locker(&mutexlockclose);
    if (!m_documment || bcloseing)
        return false;
    bcloseing = true;
    QTime tm;
    tm.start();
    m_documment->stopLoadPageThread();
    return true;
}

void DocummentProxy::closeFileAndWaitThreadClearEnd()
{
    QMutex mutexlockclose;
    QMutexLocker locker(&mutexlockclose);
    if (!bcloseing)
        closeFile();
    if (!m_documment)
        return;

    bcloseing = true;
    m_documment->waitThreadquit();
    if (nullptr != m_documment) {
        delete m_documment;
        m_documment = nullptr;
    }
}

bool DocummentProxy::setBookMarkState(int page, bool state)
{
    if (!m_documment || bcloseing)
        return false;
    bool bsuccess = m_documment->setBookMarkState(page, state);
    return bsuccess;
}

void DocummentProxy::changeAnnotationColor(int ipage, const QString uuid, const QColor &color)
{
    if (m_documment) {
        m_documment->changeAnnotationColor(ipage, uuid, color);
    }
}

void DocummentProxy::selectAllText()
{
    if (m_documment)
        m_documment->selectAllText();
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
