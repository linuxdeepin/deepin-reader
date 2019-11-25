#include "DocummentFileHelper.h"
#include <DApplication>
#include <QClipboard>
#include <QDesktopServices>
#include "subjectObserver/MsgHeader.h"
#include "controller/DataManager.h"
#include <DFileDialog>
#include "utils/utils.h"
#include <DDialog>
#include <QBitmap>
#include "subjectObserver/ModuleHeader.h"
#include "controller/DBManager.h"
#include "controller/MsgSubject.h"
#include "controller/NotifySubject.h"
#include "FileFormatHelper.h"

DocummentFileHelper::DocummentFileHelper(QObject *parent)
    : QObject(parent)
{
    m_pDocummentProxy = DocummentProxy::instance();

    initConnections();
}

bool DocummentFileHelper::save(const QString &filepath, bool withChanges)
{
    return m_pDocummentProxy->save(filepath, withChanges);
}

bool DocummentFileHelper::closeFile()
{
    return m_pDocummentProxy->closeFile();
}

void DocummentFileHelper::docBasicInfo(stFileInfo &info)
{
    m_pDocummentProxy->docBasicInfo(info);
}

bool DocummentFileHelper::mouseSelectText(const QPoint &start, const QPoint &stop)
{
    return m_pDocummentProxy->mouseSelectText(start, stop);
}

bool DocummentFileHelper::mouseBeOverText(const QPoint &point)
{
    return m_pDocummentProxy->mouseBeOverText(point);
}

Page::Link *DocummentFileHelper::mouseBeOverLink(const QPoint &point)
{
    return m_pDocummentProxy->mouseBeOverLink(point);
}

void DocummentFileHelper::mouseSelectTextClear()
{
    m_pDocummentProxy->mouseSelectTextClear();
}

bool DocummentFileHelper::getSelectTextString(QString &st)
{
    return m_pDocummentProxy->getSelectTextString(st);
}

//  保存
void DocummentFileHelper::slotSaveFile()
{
    bool rl = m_pDocummentProxy->save(m_szFilePath, true);
    qDebug() << "slotSaveFile" << rl;
    if (rl) {
        //  保存需要保存 数据库记录
        qDebug() << "DocummentFileHelper::slotSaveFile saveBookMark";
        DBManager::instance()->saveBookMark();
        DataManager::instance()->setBIsUpdate(false);
    }
}

//  另存为
void DocummentFileHelper::slotSaveAsFile()
{
    QString sFilter = FFH::getFileFilter(m_nCurDocType);

    if (sFilter != "") {
        DFileDialog dialog;
        dialog.selectFile(m_szFilePath);
        QString filePath = dialog.getSaveFileName(nullptr, tr("Save File"), m_szFilePath, sFilter);
        if (filePath != "") {
            QString sFilePath = FFH::getFilePath(filePath, m_nCurDocType);

            bool rl = m_pDocummentProxy->saveas(sFilePath, true);
            if (rl) {
                //insert a new bookmark record to bookmarktabel
                DBManager::instance()->saveasBookMark(m_szFilePath, sFilePath);
                DataManager::instance()->setStrOnlyFilePath(sFilePath);

                DataManager::instance()->setBIsUpdate(false);

                m_szFilePath = sFilePath;
                QFileInfo info(m_szFilePath);
                setAppShowTitle(info.baseName());
            }
        }
    }
}

//  打开　文件路径
void DocummentFileHelper::slotOpenFile(const QString &filePaths)
{
    //  已经打开了文件，　询问是否需要保存当前打开的文件
    if (m_szFilePath != "") {

        QStringList fileList = filePaths.split(Constant::sQStringSep,  QString::SkipEmptyParts);
        int nSize = fileList.size();
        if (nSize > 0) {
            QString sPath = fileList.at(0);

            if (m_szFilePath == sPath) {
                notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("file is opened."));
                return;
            }
        }
        //  是否有操作
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            DDialog dlg(tr("Save File"), tr("Do you need to save the file opened?"));
            dlg.setIcon(QIcon::fromTheme("deepin-reader"));
            dlg.addButtons(QStringList() <<  tr("Cancel") << tr("Not Save") <<  tr("Save"));
            int nRes = dlg.exec();
            if (nRes == 0) {    //  取消打开该文件
                return;
            }

            if (nRes == 2) {    // 保存已打开文件
                m_pDocummentProxy->save(m_szFilePath, true);
                //  保存 书签数据
                DBManager::instance()->saveBookMark();
            }
        }
        notifyMsg(MSG_OPERATION_OPEN_FILE_START);
        m_pDocummentProxy->closeFile();
        notifyMsg(MSG_CLOSE_FILE);
    } else {
        notifyMsg(MSG_OPERATION_OPEN_FILE_START);
    }

    QStringList fileList = filePaths.split(Constant::sQStringSep,  QString::SkipEmptyParts);
    int nSize = fileList.size();
    if (nSize > 0) {
        QString sPath = fileList.at(0);

        QFileInfo info(sPath);

        QString sCompleteSuffix = info.completeSuffix();
        m_nCurDocType = FFH::setCurDocuType(sCompleteSuffix);

        m_szFilePath = sPath;
        DataManager::instance()->setStrOnlyFilePath(sPath);

        bool rl = m_pDocummentProxy->openFile(m_nCurDocType, sPath);
        if (!rl) {
            m_szFilePath = "";
            DataManager::instance()->setStrOnlyFilePath("");

            notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("File not supported"));
        }
    }
}

//  设置  应用显示名称
void DocummentFileHelper::setAppShowTitle(const QString &fileName)
{
    QString sTitle = "";
    m_pDocummentProxy->title(sTitle);
    if (sTitle == "") {
        sTitle = fileName;
    }
    notifyMsg(MSG_OPERATION_OPEN_FILE_TITLE, sTitle);
}

//  复制
void DocummentFileHelper::slotCopySelectContent(const QString &sCopy)
{
    QClipboard *clipboard = DApplication::clipboard();   //获取系统剪贴板指针
    clipboard->setText(sCopy);
}

/**
 * @brief DocummentFileHelper::slotFileSlider
 * @param nFlag 1 开启放映, 0 退出放映
 */
void DocummentFileHelper::slotFileSlider(const int &nFlag)
{
    if (nFlag == 1) {
        bool bSlideModel = m_pDocummentProxy->showSlideModel();    //  开启幻灯片
        if (bSlideModel) {
            m_pDocummentProxy->setAutoPlaySlide(true);
            DataManager::instance()->setCurShowState(FILE_SLIDE);
        }
    } else {
        if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
            bool rl = m_pDocummentProxy->exitSlideModel();
            if (rl) {
                DataManager::instance()->setCurShowState(FILE_NORMAL);
            }
        }
    }
}

void DocummentFileHelper::initConnections()
{
    connect(m_pDocummentProxy, &DocummentProxy::signal_openResult, this, [ = ](bool openresult) {
        if (openresult) {
            notifyMsg(MSG_OPERATION_OPEN_FILE_OK);
            //  通知 其他窗口， 打开文件成功了！！！
            QFileInfo info(m_szFilePath);
            setAppShowTitle(info.baseName());
        } else {
            m_szFilePath = "";
            DataManager::instance()->setStrOnlyFilePath("");
            notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("Please check if the file is damaged"));
        }
    });
}

//  发送 操作消息
void DocummentFileHelper::sendMsg(const int &msgType, const QString &msgContent)
{
    MsgSubject::getInstance()->sendMsg(msgType, msgContent);
}

//  通知消息, 不需要撤回
void DocummentFileHelper::notifyMsg(const int &msgType, const QString &msgContent)
{
    NotifySubject::getInstance()->notifyMsg(msgType, msgContent);
}

//  文档　跳转页码　．　打开浏览器
void DocummentFileHelper::onClickPageLink(Page::Link *pLink)
{
    Page::LinkType_EM linkType = pLink->type;
    if (linkType == Page::LinkType_NULL) {

    } else if (linkType == Page::LinkType_Goto) {
        int page = pLink->page - 1;
        m_pDocummentProxy->pageJump(page);
    } else if (linkType == Page::LinkType_GotoOtherFile) {

    } else if (linkType == Page::LinkType_Browse) {
        QString surlOrFileName = pLink->urlOrFileName;
        QDesktopServices::openUrl(QUrl(surlOrFileName, QUrl::TolerantMode));
    } else if (linkType == Page::LinkType_Execute) {

    }
}

QPoint DocummentFileHelper::global2RelativePoint(const QPoint &globalpoint)
{
    return  m_pDocummentProxy->global2RelativePoint(globalpoint);
}

bool DocummentFileHelper::pageMove(const double &mvx, const double &mvy)
{
    return m_pDocummentProxy->pageMove(mvx, mvy);
}

int DocummentFileHelper::pointInWhichPage(const QPoint &pos)
{
    return m_pDocummentProxy->pointInWhichPage(pos);
}

int DocummentFileHelper::getPageSNum()
{
    return m_pDocummentProxy->getPageSNum();
}

int DocummentFileHelper::currentPageNo()
{
    return m_pDocummentProxy->currentPageNo();
}

bool DocummentFileHelper::pageJump(const int &pagenum)
{
    return m_pDocummentProxy->pageJump(pagenum);
}

bool DocummentFileHelper::getImage(const int &pagenum, QImage &image, const double &width, const double &height)
{
    return  m_pDocummentProxy->getImage(pagenum, image, width, height);
}

QImage DocummentFileHelper::roundImage(const QPixmap &img_in, const int &radius)
{
    if (img_in.isNull()) {
        return QPixmap().toImage();
    }
    QSize size(img_in.size());
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(mask.rect(), radius, radius);
    QPixmap image = img_in;
    image.setMask(mask);
    return image.toImage();
}

bool DocummentFileHelper::showMagnifier(const QPoint &point)
{
    return  m_pDocummentProxy->showMagnifier(point);
}

bool DocummentFileHelper::closeMagnifier()
{
    return m_pDocummentProxy->closeMagnifier();
}

bool DocummentFileHelper::setBookMarkState(const int &page, const bool &state)
{
    return m_pDocummentProxy->setBookMarkState(page, state);
}

void DocummentFileHelper::setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode)
{
    m_pDocummentProxy->setScaleRotateViewModeAndShow(scale, rotate, viewmode);
}

//bool DocummentFileHelper::setViewModeAndShow(const ViewMode_EM &viewmode)
//{
//    return m_pDocummentProxy->setViewModeAndShow(viewmode);
//}

//void DocummentFileHelper::scaleRotateAndShow(const double &scale, const RotateType_EM &rotate)
//{
//    m_pDocummentProxy->scaleRotateAndShow(scale, rotate);
//}

double DocummentFileHelper::adaptWidthAndShow(const double &width)
{
    return m_pDocummentProxy->adaptWidthAndShow(width);
}

double DocummentFileHelper::adaptHeightAndShow(const double &height)
{
    return m_pDocummentProxy->adaptHeightAndShow(height);
}

void DocummentFileHelper::clearsearch()
{
    m_pDocummentProxy->clearsearch();
}

void DocummentFileHelper::getAllAnnotation(QList<stHighlightContent> &listres)
{
    m_pDocummentProxy->getAllAnnotation(listres);
}

QString DocummentFileHelper::addAnnotation(const QPoint &startpos, const QPoint &endpos, const QColor &color)
{
    QString strUuid = m_pDocummentProxy->addAnnotation(startpos, endpos, color);
    if (strUuid != "") {
        DataManager::instance()->setBIsUpdate(true);
    }
    return strUuid;
}

void DocummentFileHelper::changeAnnotationColor(const int &ipage, const QString &uuid, const QColor &color)
{
    DataManager::instance()->setBIsUpdate(true);
    m_pDocummentProxy->changeAnnotationColor(ipage, uuid, color);
}

bool DocummentFileHelper::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    return m_pDocummentProxy->annotationClicked(pos, strtext, struuid);
}

void DocummentFileHelper::removeAnnotation(const QString &struuid, const int &ipage)
{
    DataManager::instance()->setBIsUpdate(true);
    m_pDocummentProxy->removeAnnotation(struuid, ipage);
}

QString DocummentFileHelper::removeAnnotation(const QPoint &pos)
{
    DataManager::instance()->setBIsUpdate(true);
    return m_pDocummentProxy->removeAnnotation(pos);
}

void DocummentFileHelper::setAnnotationText(const int &ipage, const QString &struuid, const QString &strtext)
{
    DataManager::instance()->setBIsUpdate(true);
    m_pDocummentProxy->setAnnotationText(ipage, struuid, strtext);
}

void DocummentFileHelper::getAnnotationText(const QString &struuid, QString &strtext, const int &ipage)
{
    m_pDocummentProxy->getAnnotationText(struuid, strtext, ipage);
}

void DocummentFileHelper::jumpToHighLight(const QString &uuid, const int &ipage)
{
    DataManager::instance()->setBIsUpdate(true);
    m_pDocummentProxy->jumpToHighLight(uuid, ipage);
}

void DocummentFileHelper::search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color)
{
    m_pDocummentProxy->search(strtext, resmap, color);
}

void DocummentFileHelper::findNext()
{
    m_pDocummentProxy->findNext();
}

void DocummentFileHelper::findPrev()
{
    m_pDocummentProxy->findPrev();
}
