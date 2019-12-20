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
#include "utils/PublicFunction.h"

DocummentFileHelper::DocummentFileHelper(QObject *parent)
    : QObject(parent)
{
    initConnections();

    m_pMsgList = { MSG_OPEN_FILE_PATH, MSG_OPEN_FILE_PATH_S, MSG_OPERATION_SAVE_AS_FILE,
                   MSG_OPERATION_TEXT_COPY
                 };
    m_pKeyMsgList = {KeyStr::g_ctrl_s, KeyStr::g_ctrl_shift_s};

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }

    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

DocummentFileHelper::~DocummentFileHelper()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

void DocummentFileHelper::setDocProxy(DocummentProxy *p)
{
    m_pDocummentProxy = p;

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

void DocummentFileHelper::initConnections()
{
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)),
            SLOT(slotDealWithData(const int &, const QString &)));
    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)),
            SLOT(slotDealWithKeyMsg(const QString &)));

    connect(this, SIGNAL(sigFileSlider(const int &)), SLOT(slotFileSlider(const int &)));
    connect(this, SIGNAL(sigFileCtrlContent()), SLOT(slotFileCtrlContent()));
}

//  处理 应用推送的消息数据
int DocummentFileHelper::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return  ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_OPERATION_SLIDE) {
        emit sigFileSlider(1);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (m_pKeyMsgList.contains(msgContent)) {
            emit sigDealWithKeyMsg(msgContent);
            return ConstantMsg::g_effective_res;
        }

        if (KeyStr::g_esc == msgContent) {
            emit sigFileSlider(0);
        } else if (KeyStr::g_ctrl_c == msgContent) {
            emit sigFileCtrlContent();
        }

    }
    return 0;
}

void DocummentFileHelper::slotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPEN_FILE_PATH) {            //  打开单个文件
        onOpenFile(msgContent);
    } else if (msgType == MSG_OPEN_FILE_PATH_S) {   //  打开多个文件
        onOpenFiles(msgContent);
    } else if (msgType == MSG_OPERATION_SAVE_AS_FILE) { //  另存为文件
        onSaveAsFile();
    } else if (msgType == MSG_OPERATION_TEXT_COPY) {    //  复制
        slotCopySelectContent(msgContent);
    }
}

void DocummentFileHelper::slotDealWithKeyMsg(const QString &msgContent)
{
    if (msgContent == KeyStr::g_ctrl_s) {
        onSaveFile();
    } else if (msgContent == KeyStr::g_ctrl_shift_s) {
        onSaveAsFile();
    }
}


//  发送 操作消息
void DocummentFileHelper::sendMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(msgType, msgContent);
}

//  通知消息, 不需要撤回
void DocummentFileHelper::notifyMsg(const int &msgType, const QString &msgContent)
{
    m_pNotifySubject->notifyMsg(msgType, msgContent);
}

bool DocummentFileHelper::save(const QString &filepath, bool withChanges)
{
    if (!m_pDocummentProxy) {
        return false;
    }

    return m_pDocummentProxy->save(filepath, withChanges);
}

bool DocummentFileHelper::closeFile()
{
    if (!m_pDocummentProxy) {
        return false;
    }

    return m_pDocummentProxy->closeFile();
}

void DocummentFileHelper::docBasicInfo(stFileInfo &info)
{
    if (!m_pDocummentProxy) {
        return ;
    }

    m_pDocummentProxy->docBasicInfo(info);
}

void DocummentFileHelper::setViewFocus()
{
    if (!m_pDocummentProxy) {
        return ;
    }

    m_pDocummentProxy->setViewFocus();
}

bool DocummentFileHelper::mouseSelectText(const QPoint &start, const QPoint &stop)
{
    if (!m_pDocummentProxy) {
        return false;
    }

    return m_pDocummentProxy->mouseSelectText(start, stop);
}

bool DocummentFileHelper::mouseBeOverText(const QPoint &point)
{
    if (!m_pDocummentProxy) {
        return false;
    }

    return m_pDocummentProxy->mouseBeOverText(point);
}

Page::Link *DocummentFileHelper::mouseBeOverLink(const QPoint &point)
{
    if (!m_pDocummentProxy) {
        return nullptr;
    }

    return m_pDocummentProxy->mouseBeOverLink(point);
}

void DocummentFileHelper::mouseSelectTextClear()
{
    if (!m_pDocummentProxy) {
        return ;
    }

    m_pDocummentProxy->mouseSelectTextClear();
}

bool DocummentFileHelper::getSelectTextString(QString &st)
{
    if (!m_pDocummentProxy) {
        return false;
    }

    return m_pDocummentProxy->getSelectTextString(st);
}

//  保存
void DocummentFileHelper::onSaveFile()
{
    if (DataManager::instance()->bIsUpdate()) {
        bool rl = save(m_szFilePath, true);
        qDebug() << "slotSaveFile" << rl;
        if (rl) {
            //  保存需要保存 数据库记录
            qDebug() << "DocummentFileHelper::slotSaveFile saveBookMark";
            DBManager::instance()->saveBookMark();
            DataManager::instance()->setBIsUpdate(false);
            notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Save Success"));
        } else {
            notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Save Failed"));
        }
    } else {
        notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Not Changed"));
    }
}

//  另存为
void DocummentFileHelper::onSaveAsFile()
{
    if (!m_pDocummentProxy) {
        return;
    }
    QString sFilter = FFH::getFileFilter(m_nCurDocType);

    if (sFilter != "") {
        DFileDialog dialog;
        dialog.selectFile(m_szFilePath);
        QString filePath = dialog.getSaveFileName(nullptr, tr("Save As"), m_szFilePath, sFilter);

        if (filePath.endsWith("/.pdf")) {
            DDialog dlg("", tr("Not support save file with illegal name"));
            QIcon icon(PF::getIconPath("exception-logo"));
            dlg.setIcon(icon /*QIcon(":/resources/exception-logo.svg")*/);
            dlg.addButtons(QStringList() << tr("Ok"));
            QMargins mar(0, 0, 0, 30);
            dlg.setContentLayoutContentsMargins(mar);
            dlg.exec();
            return;
        }
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
void DocummentFileHelper::onOpenFile(const QString &filePaths)
{
    if (!m_pDocummentProxy) {
        return;
    }

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
            dlg.setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
            dlg.addButtons(QStringList() <<  tr("Cancel") << tr("Not Save") <<  tr("Save"));
            int nRes = dlg.exec();
            if (nRes == 0) {    //  取消打开该文件
                return;
            }

            if (nRes == 2) {    // 保存已打开文件
                save(m_szFilePath, true);
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

void DocummentFileHelper::onOpenFiles(const QString &filePaths)
{
    bool bisopen = false;
    QStringList canOpenFileList = filePaths.split(Constant::sQStringSep, QString::SkipEmptyParts);
    foreach (auto s, canOpenFileList) {
        QString sOpenPath = DataManager::instance()->strOnlyFilePath();
        if (s == sOpenPath) {
            notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("file is opened."));
        } else {
            QString sRes = s + Constant::sQStringSep;
            QString sOpenPath = DataManager::instance()->strOnlyFilePath(); //  打开文档为空
            if (sOpenPath == "") {
                if (!bisopen)
                    notifyMsg(MSG_OPEN_FILE_PATH, sRes);
                else {
                    if (!Utils::runApp(s))
                        qDebug() << __FUNCTION__ << "process start deepin-reader failed";
                }

                bisopen = true;
            } else {
                if (!Utils::runApp(s))
                    qDebug() << __FUNCTION__ << "process start deepin-reader failed";
            }
        }
    }
}

//  设置  应用显示名称
void DocummentFileHelper::setAppShowTitle(const QString &fileName)
{
    if (!m_pDocummentProxy) {
        return;
    }

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
    if (!m_pDocummentProxy) {
        return;
    }

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

void DocummentFileHelper::slotFileCtrlContent()
{
    QString sSelectText = "";
    getSelectTextString(sSelectText);  //  选择　当前选中下面是否有文字

    if (sSelectText != "") {
        slotCopySelectContent(sSelectText);
    }
}


//  文档　跳转页码　．　打开浏览器
void DocummentFileHelper::onClickPageLink(Page::Link *pLink)
{
    if (!m_pDocummentProxy) {
        return;
    }
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
    if (!m_pDocummentProxy) {
        return QPoint(0, 0);
    }
    return  m_pDocummentProxy->global2RelativePoint(globalpoint);
}

bool DocummentFileHelper::pageMove(const double &mvx, const double &mvy)
{
    if (!m_pDocummentProxy) {
        return false;
    }
    return m_pDocummentProxy->pageMove(mvx, mvy);
}

int DocummentFileHelper::pointInWhichPage(const QPoint &pos)
{
    if (!m_pDocummentProxy) {
        return -1;
    }
    return m_pDocummentProxy->pointInWhichPage(pos);
}

int DocummentFileHelper::getPageSNum()
{
    if (!m_pDocummentProxy) {
        return -1;
    }
    return m_pDocummentProxy->getPageSNum();
}

int DocummentFileHelper::currentPageNo()
{
    if (!m_pDocummentProxy) {
        return -1;
    }
    return m_pDocummentProxy->currentPageNo();
}

bool DocummentFileHelper::pageJump(const int &pagenum)
{
    if (!m_pDocummentProxy) {
        return false;
    }
    return m_pDocummentProxy->pageJump(pagenum);
}

bool DocummentFileHelper::getImage(const int &pagenum, QImage &image, const double &width, const double &height)
{
    if (!m_pDocummentProxy) {
        return false;
    }
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
    if (!m_pDocummentProxy) {
        return false;
    }
    return  m_pDocummentProxy->showMagnifier(point);
}

bool DocummentFileHelper::closeMagnifier()
{
    if (!m_pDocummentProxy) {
        return false;
    }
    return m_pDocummentProxy->closeMagnifier();
}

bool DocummentFileHelper::setBookMarkState(const int &page, const bool &state)
{
    if (!m_pDocummentProxy) {
        return false;
    }
    return m_pDocummentProxy->setBookMarkState(page, state);
}

void DocummentFileHelper::setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode)
{
    if (!m_pDocummentProxy) {
        return ;
    }
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
    if (!m_pDocummentProxy) {
        return 0.0;
    }
    return m_pDocummentProxy->adaptWidthAndShow(width);
}

double DocummentFileHelper::adaptHeightAndShow(const double &height)
{
    if (!m_pDocummentProxy) {
        return 0.0;
    }
    return m_pDocummentProxy->adaptHeightAndShow(height);
}

void DocummentFileHelper::clearsearch()
{
    if (!m_pDocummentProxy) {
        return ;
    }
    m_pDocummentProxy->clearsearch();
}

void DocummentFileHelper::getAllAnnotation(QList<stHighlightContent> &listres)
{
    if (!m_pDocummentProxy) {
        return ;
    }
    m_pDocummentProxy->getAllAnnotation(listres);
}

QString DocummentFileHelper::addAnnotation(const QPoint &startpos, const QPoint &endpos, const QColor &color)
{
    if (!m_pDocummentProxy) {
        return "";
    }
    QString strUuid = m_pDocummentProxy->addAnnotation(startpos, endpos, color);
    if (strUuid != "") {
        DataManager::instance()->setBIsUpdate(true);
    }
    return strUuid;
}

void DocummentFileHelper::changeAnnotationColor(const int &ipage, const QString &uuid, const QColor &color)
{
    if (!m_pDocummentProxy) {
        return ;
    }
    DataManager::instance()->setBIsUpdate(true);
    m_pDocummentProxy->changeAnnotationColor(ipage, uuid, color);
}

bool DocummentFileHelper::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    if (!m_pDocummentProxy) {
        return false;
    }
    return m_pDocummentProxy->annotationClicked(pos, strtext, struuid);
}

void DocummentFileHelper::removeAnnotation(const QString &struuid, const int &ipage)
{
    if (!m_pDocummentProxy) {
        return ;
    }
    DataManager::instance()->setBIsUpdate(true);
    m_pDocummentProxy->removeAnnotation(struuid, ipage);
}

QString DocummentFileHelper::removeAnnotation(const QPoint &pos)
{
    if (!m_pDocummentProxy) {
        return "";
    }
    DataManager::instance()->setBIsUpdate(true);
    return m_pDocummentProxy->removeAnnotation(pos);
}

void DocummentFileHelper::setAnnotationText(const int &ipage, const QString &struuid, const QString &strtext)
{
    if (!m_pDocummentProxy) {
        return;
    }
    DataManager::instance()->setBIsUpdate(true);
    m_pDocummentProxy->setAnnotationText(ipage, struuid, strtext);
}

void DocummentFileHelper::getAnnotationText(const QString &struuid, QString &strtext, const int &ipage)
{
    if (!m_pDocummentProxy) {
        return;
    }
    m_pDocummentProxy->getAnnotationText(struuid, strtext, ipage);
}

void DocummentFileHelper::jumpToHighLight(const QString &uuid, const int &ipage)
{
    if (!m_pDocummentProxy) {
        return ;
    }
    DataManager::instance()->setBIsUpdate(true);
    m_pDocummentProxy->jumpToHighLight(uuid, ipage);
}

void DocummentFileHelper::search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color)
{
    if (!m_pDocummentProxy) {
        return ;
    }
    m_pDocummentProxy->search(strtext, resmap, color);
}

void DocummentFileHelper::findNext()
{
    if (!m_pDocummentProxy) {
        return ;
    }
    m_pDocummentProxy->findNext();
}

void DocummentFileHelper::findPrev()
{
    if (!m_pDocummentProxy) {
        return ;
    }
    m_pDocummentProxy->findPrev();
}

bool DocummentFileHelper::getAutoPlaySlideStatu()
{
    if (!m_pDocummentProxy) {
        return false;
    }
    return m_pDocummentProxy->getAutoPlaySlideStatu();
}

void DocummentFileHelper::setAutoPlaySlide(const bool &autoplay, const int &timemsec)
{
    if (!m_pDocummentProxy) {
        return ;
    }
    m_pDocummentProxy->setAutoPlaySlide(autoplay, timemsec);
}
