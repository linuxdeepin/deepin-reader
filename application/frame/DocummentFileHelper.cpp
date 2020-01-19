/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DocummentFileHelper.h"

#include <DApplication>
#include <QClipboard>
//#include <QDesktopServices>
#include <DFileDialog>
#include <DDialog>
#include <QBitmap>

#include "subjectObserver/ModuleHeader.h"
#include "subjectObserver/MsgHeader.h"

#include "controller/DBManager.h"
#include "controller/NotifySubject.h"

#include "FileFormatHelper.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"

//#include "controller/AppSetting.h"

#include "application.h"

DocummentFileHelper::DocummentFileHelper(QObject *parent)
    : QObject(parent)
{
    initConnections();

    m_pMsgList = { MSG_OPEN_FILE_PATH, MSG_OPEN_FILE_PATH_S, MSG_OPERATION_SAVE_AS_FILE,
                   MSG_OPERATION_TEXT_COPY, MSG_DOC_JUMP_PAGE, MSG_OPERATION_FIRST_PAGE, MSG_OPERATION_PREV_PAGE,
                   MSG_OPERATION_NEXT_PAGE, MSG_OPERATION_END_PAGE
                 };
    m_pKeyMsgList = {KeyStr::g_ctrl_s, KeyStr::g_ctrl_shift_s};

    m_pNotifySubject = g_NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

DocummentFileHelper::~DocummentFileHelper()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

//void DocummentFileHelper::setDocProxy(DocummentProxy *p)
//{
//    DocummentProxy::instance() = p;

//    connect(DocummentProxy::instance(), &DocummentProxy::signal_openResult, this, [ = ](bool openresult) {
//        if (openresult) {
//            notifyMsg(MSG_OPERATION_OPEN_FILE_OK);
//            //  通知 其他窗口， 打开文件成功了！！！
//            QFileInfo info(m_szFilePath);
//            setAppShowTitle(info.baseName());
//        } else {
//            m_szFilePath = "";
//            DataManager::instance()->setStrOnlyFilePath("");
//            notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("Please check if the file is damaged"));
//        }
//    });
//}

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
    } else if (msgType == MSG_DOC_JUMP_PAGE) {              //  请求跳转页面
        __PageJump(msgContent.toInt());
    } else if (msgType == MSG_OPERATION_FIRST_PAGE || msgType == MSG_OPERATION_PREV_PAGE ||
               msgType == MSG_OPERATION_NEXT_PAGE || msgType == MSG_OPERATION_END_PAGE) {
        __PageJumpByMsg(msgType);
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
    notifyMsg(msgType, msgContent);
}

//  通知消息, 不需要撤回
void DocummentFileHelper::notifyMsg(const int &msgType, const QString &msgContent)
{
    m_pNotifySubject->notifyMsg(msgType, msgContent);
}

bool DocummentFileHelper::save(const QString &filepath, bool withChanges)
{
    if (!DocummentProxy::instance()) {
        return false;
    }

    return DocummentProxy::instance()->save(filepath, withChanges);
}

bool DocummentFileHelper::closeFile()
{
    if (!DocummentProxy::instance()) {
        return false;
    }

    return DocummentProxy::instance()->closeFile();
}

void DocummentFileHelper::docBasicInfo(stFileInfo &info)
{
    if (!DocummentProxy::instance()) {
        return ;
    }

    DocummentProxy::instance()->docBasicInfo(info);
}

void DocummentFileHelper::setViewFocus()
{
    if (!DocummentProxy::instance()) {
        return ;
    }

    DocummentProxy::instance()->setViewFocus();
}

//bool DocummentFileHelper::mouseSelectText(const QPoint &start, const QPoint &stop)
//{
//    if (!DocummentProxy::instance()) {
//        return false;
//    }

//    return DocummentProxy::instance()->mouseSelectText(start, stop);
//}

//bool DocummentFileHelper::mouseBeOverText(const QPoint &point)
//{
//    if (!DocummentProxy::instance()) {
//        return false;
//    }

//    return DocummentProxy::instance()->mouseBeOverText(point);
//}

//Page::Link *DocummentFileHelper::mouseBeOverLink(const QPoint &point)
//{
//    if (!DocummentProxy::instance()) {
//        return nullptr;
//    }

//    return DocummentProxy::instance()->mouseBeOverLink(point);
//}

//void DocummentFileHelper::mouseSelectTextClear()
//{
//    if (!DocummentProxy::instance()) {
//        return ;
//    }

//    DocummentProxy::instance()->mouseSelectTextClear();
//}

bool DocummentFileHelper::getSelectTextString(QString &st)
{
    if (!DocummentProxy::instance()) {
        return false;
    }

    return DocummentProxy::instance()->getSelectTextString(st);
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
            dApp->dbM->saveBookMark();

            DataManager::instance()->setBIsUpdate(false);
            notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Saved successfully"));
        } else {
            notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Saved failed"));
        }
    } else {
        notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("No changes"));
    }
    //insert msg to FileFontTable
    saveFileFontMsg(m_szFilePath);
}

//  另存为
void DocummentFileHelper::onSaveAsFile()
{
    if (!DocummentProxy::instance()) {
        return;
    }
    QString sFilter = FFH::getFileFilter(m_nCurDocType);

    if (sFilter != "") {
        QFileDialog dialog;
        dialog.selectFile(m_szFilePath);
        QString filePath = dialog.getSaveFileName(nullptr, tr("Save as"), m_szFilePath, sFilter);

        if (filePath.endsWith("/.pdf")) {
            DDialog dlg("", tr("Invalid file name"));
            QIcon icon(PF::getIconPath("exception-logo"));
            dlg.setIcon(icon /*QIcon(":/resources/exception-logo.svg")*/);
            dlg.addButtons(QStringList() << tr("OK"));
            QMargins mar(0, 0, 0, 30);
            dlg.setContentLayoutContentsMargins(mar);
            dlg.exec();
            return;
        }
        if (filePath != "") {
            if (m_szFilePath == filePath) {
                onSaveFile();
            } else {
                QString sFilePath = FFH::getFilePath(filePath, m_nCurDocType);

                bool rl = DocummentProxy::instance()->saveas(sFilePath, true);
                if (rl) {
                    //insert a new bookmark record to bookmarktabel
                    dApp->dbM->saveasBookMark(m_szFilePath, sFilePath);
                    DataManager::instance()->setStrOnlyFilePath(sFilePath);
                    //insert msg to FileFontTable
                    saveFileFontMsg(filePath);
                    DataManager::instance()->setBIsUpdate(false);

                    m_szFilePath = sFilePath;

                    setAppShowTitle();
                }
            }
        }
    }
}

/**
 * @brief DocummentFileHelper::saveFileFontMsg
 * 存储文件的字号信息到表FileFontTable
 * @param filePath
 */
void DocummentFileHelper::saveFileFontMsg(const QString &filePath)
{
    st_fileHistoryMsg msg;
    msg = DataManager::instance()->getHistoryMsg();
    dApp->histroyDb->insertFileFontMsg(msg, filePath);
}

//  跳转页面
void DocummentFileHelper::__PageJump(const int &pagenum)
{
    if (DocummentProxy::instance()) {
        int nPageSize = getPageSNum();      //  总页数
        if (pagenum < 0 || pagenum == nPageSize) {
            return;
        }

        int nCurPage = currentPageNo();
        if (nCurPage != pagenum) {
            DocummentProxy::instance()->pageJump(pagenum);
        }
    }
}

//  前一页\第一页\后一页\最后一页 操作
void DocummentFileHelper::__PageJumpByMsg(const int &iType)
{
    int iPage = -1;
    if (iType == MSG_OPERATION_FIRST_PAGE) {
        iPage = 0;
    } else if (iType == MSG_OPERATION_PREV_PAGE) {
        int nCurPage = currentPageNo();
        iPage = nCurPage - 1;
    } else if (iType == MSG_OPERATION_NEXT_PAGE) {
        int nCurPage = currentPageNo();
        iPage = nCurPage + 1;
    } else if (iType == MSG_OPERATION_END_PAGE) {
        int nCurPage = getPageSNum();
        iPage = nCurPage - 1;
    }

    __PageJump(iPage);
}

/**
 * brief DocummentFileHelper::setDBFileFontMsgToAppSet
 * 取数据库中文件的字号信息，保存到全局数据类中
 */
void DocummentFileHelper::setDBFilesMsgToAppSet(st_fileHistoryMsg &historyMsg, const QString &filePath)
{
    dApp->histroyDb->getFileFontMsg(historyMsg, filePath);

    DataManager::instance()->setHistoryMsg(historyMsg);
}

//  打开　文件路径
void DocummentFileHelper::onOpenFile(const QString &filePaths)
{
    if (!DocummentProxy::instance()) {
        return;
    }

    //  已经打开了文件，　询问是否需要保存当前打开的文件
    if (m_szFilePath != "") {
        QStringList fileList = filePaths.split(Constant::sQStringSep,  QString::SkipEmptyParts);
        int nSize = fileList.size();
        if (nSize > 0) {
            QString sPath = fileList.at(0);

            if (m_szFilePath == sPath) {
                notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("The file is already open"));
                return;
            }
        }
        //  是否有操作
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            DDialog dlg(tr("Save"), tr("Do you want to save the changes?"));
            dlg.setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
            dlg.addButtons(QStringList() <<  tr("Cancel") << tr("Discard") <<  tr("Save"));
            int nRes = dlg.exec();
            if (nRes == 0) {    //  取消打开该文件
                return;
            }

            if (nRes == 2) {    // 保存已打开文件
                save(m_szFilePath, true);
                //  保存 书签数据
                dApp->dbM->saveBookMark();
                //insert msg to FileFontTable
                saveFileFontMsg(m_szFilePath);
            }
        }
        notifyMsg(MSG_OPERATION_OPEN_FILE_START);
        DocummentProxy::instance()->closeFile();
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
        //从数据库中获取文件的字号信息
        QString ssscale = "";
        QString doubPage = "";
        QString fit = "";
        QString rotate = "";
        QString curPage = "";

        st_fileHistoryMsg historyMsg;

        setDBFilesMsgToAppSet(historyMsg, sPath);

        ssscale = historyMsg.m_strScale;
        doubPage = historyMsg.m_strDoubPage;
        fit = historyMsg.m_strFit;
        rotate = historyMsg.m_strRotate;
        curPage = historyMsg.m_strCurPage;

        qDebug() << __FUNCTION__ << " scale:" << ssscale
                 << "  doubPage:" << doubPage
                 << "  fit:" << fit
                 << "  rotate:" << rotate
                 << "  curPage:" << curPage;

        int iscale = ssscale.toInt();
        iscale = (iscale > 500 ? 500 : iscale) <= 0 ? 100 : iscale;
        double scale = iscale / 100.0;
        RotateType_EM rotatetype = static_cast<RotateType_EM>((rotate.toInt() / 90) + 1);
        ViewMode_EM viewmode = static_cast<ViewMode_EM>(doubPage.toInt());

        int ipage = curPage.toInt();//AppSetting::instance()->getKeyValue(KEY_PAGENUM).toInt();
//        qDebug()  << ipage << AppSetting::instance()->getKeyValue(KEY_ROTATE).toInt() << AppSetting::instance()->getKeyValue(KEY_DOUBPAGE).toInt();
        bool rl = DocummentProxy::instance()->openFile(m_nCurDocType, sPath, static_cast<unsigned int>(ipage), rotatetype, scale, viewmode);

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
            notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("The file is already open"));
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
void DocummentFileHelper::setAppShowTitle()
{
    if (!DocummentProxy::instance()) {
        return;
    }

    QString sTitle = "";
    DocummentProxy::instance()->title(sTitle);
    if (sTitle == "") {
        QFileInfo info(m_szFilePath);
        sTitle = info.baseName();
    }
    notifyMsg(MSG_OPERATION_OPEN_FILE_OK, sTitle);
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
    if (!DocummentProxy::instance()) {
        return;
    }

    if (nFlag == 1) {
        bool bSlideModel = DocummentProxy::instance()->showSlideModel();    //  开启幻灯片
        if (bSlideModel) {
            DocummentProxy::instance()->setAutoPlaySlide(true);
            DataManager::instance()->setCurShowState(FILE_SLIDE);
        }
    } else {
        if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
            bool rl = DocummentProxy::instance()->exitSlideModel();
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

void DocummentFileHelper::setSzFilePath(const QString &szFilePath)
{
    m_szFilePath = szFilePath;
}

//DocummentProxy *DocummentFileHelper::getDocummentProxy() const
//{
//    return DocummentProxy::instance();
//}


////  文档　跳转页码　．　打开浏览器
//void DocummentFileHelper::onClickPageLink(Page::Link *pLink)
//{
////    if (!DocummentProxy::instance()) {
////        return;
////    }
//    Page::LinkType_EM linkType = pLink->type;
//    if (linkType == Page::LinkType_NULL) {

//    } else if (linkType == Page::LinkType_Goto) {
//        int page = pLink->page - 1;
//        __PageJump(page);
//    } else if (linkType == Page::LinkType_GotoOtherFile) {

//    } else if (linkType == Page::LinkType_Browse) {
//        QString surlOrFileName = pLink->urlOrFileName;
//        QDesktopServices::openUrl(QUrl(surlOrFileName, QUrl::TolerantMode));
//    } else if (linkType == Page::LinkType_Execute) {

//    }
//}

//QPoint DocummentFileHelper::global2RelativePoint(const QPoint &globalpoint)
//{
//    if (!DocummentProxy::instance()) {
//        return QPoint(0, 0);
//    }
//    return  DocummentProxy::instance()->global2RelativePoint(globalpoint);
//}

//bool DocummentFileHelper::pageMove(const double &mvx, const double &mvy)
//{
//    if (!DocummentProxy::instance()) {
//        return false;
//    }
//    return DocummentProxy::instance()->pageMove(mvx, mvy);
//}

//int DocummentFileHelper::pointInWhichPage(const QPoint &pos)
//{
//    if (!DocummentProxy::instance()) {
//        return -1;
//    }
//    return DocummentProxy::instance()->pointInWhichPage(pos);
//}

int DocummentFileHelper::getPageSNum()
{
    if (!DocummentProxy::instance()) {
        return -1;
    }
    return DocummentProxy::instance()->getPageSNum();
}

int DocummentFileHelper::currentPageNo()
{
    if (!DocummentProxy::instance()) {
        return -1;
    }
    return DocummentProxy::instance()->currentPageNo();
}

bool DocummentFileHelper::getImage(const int &pagenum, QImage &image, const double &width, const double &height)
{
    if (!DocummentProxy::instance()) {
        return false;
    }
    return  DocummentProxy::instance()->getImage(pagenum, image, width, height);
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

//bool DocummentFileHelper::showMagnifier(const QPoint &point)
//{
//    if (!DocummentProxy::instance()) {
//        return false;
//    }
//    return  DocummentProxy::instance()->showMagnifier(point);
//}

//bool DocummentFileHelper::closeMagnifier()
//{
//    if (!DocummentProxy::instance()) {
//        return false;
//    }
//    return DocummentProxy::instance()->closeMagnifier();
//}

bool DocummentFileHelper::setBookMarkState(const int &page, const bool &state)
{
    if (!DocummentProxy::instance()) {
        return false;
    }
    return DocummentProxy::instance()->setBookMarkState(page, state);
}

void DocummentFileHelper::setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode)
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DocummentProxy::instance()->setScaleRotateViewModeAndShow(scale, rotate, viewmode);
}

//bool DocummentFileHelper::setViewModeAndShow(const ViewMode_EM &viewmode)
//{
//    return DocummentProxy::instance()->setViewModeAndShow(viewmode);
//}

//void DocummentFileHelper::scaleRotateAndShow(const double &scale, const RotateType_EM &rotate)
//{
//    DocummentProxy::instance()->scaleRotateAndShow(scale, rotate);
//}

//double DocummentFileHelper::adaptWidthAndShow(const double &width)
//{
//    if (!DocummentProxy::instance()) {
//        return 0.0;
//    }
//    return DocummentProxy::instance()->adaptWidthAndShow(width);
//}

//double DocummentFileHelper::adaptHeightAndShow(const double &height)
//{
//    if (!DocummentProxy::instance()) {
//        return 0.0;
//    }
//    return DocummentProxy::instance()->adaptHeightAndShow(height);
//}

void DocummentFileHelper::clearsearch()
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DocummentProxy::instance()->clearsearch();
}

void DocummentFileHelper::getAllAnnotation(QList<stHighlightContent> &listres)
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DocummentProxy::instance()->getAllAnnotation(listres);
}

QString DocummentFileHelper::addAnnotation(const QPoint &startpos, const QPoint &endpos, const QColor &color)
{
    if (!DocummentProxy::instance()) {
        return "";
    }
    QString strUuid = DocummentProxy::instance()->addAnnotation(startpos, endpos, color);
    if (strUuid != "") {
        DataManager::instance()->setBIsUpdate(true);
    }
    return strUuid;
}

void DocummentFileHelper::changeAnnotationColor(const int &ipage, const QString &uuid, const QColor &color)
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DataManager::instance()->setBIsUpdate(true);
    DocummentProxy::instance()->changeAnnotationColor(ipage, uuid, color);
}

//bool DocummentFileHelper::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
//{
//    if (!DocummentProxy::instance()) {
//        return false;
//    }
//    return DocummentProxy::instance()->annotationClicked(pos, strtext, struuid);
//}

void DocummentFileHelper::removeAnnotation(const QString &struuid, const int &ipage)
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DataManager::instance()->setBIsUpdate(true);
    DocummentProxy::instance()->removeAnnotation(struuid, ipage);
}

QString DocummentFileHelper::removeAnnotation(const QPoint &pos)
{
    if (!DocummentProxy::instance()) {
        return "";
    }
    DataManager::instance()->setBIsUpdate(true);
    return DocummentProxy::instance()->removeAnnotation(pos);
}

void DocummentFileHelper::setAnnotationText(const int &ipage, const QString &struuid, const QString &strtext)
{
    if (!DocummentProxy::instance()) {
        return;
    }
    DataManager::instance()->setBIsUpdate(true);
    DocummentProxy::instance()->setAnnotationText(ipage, struuid, strtext);
}

void DocummentFileHelper::getAnnotationText(const QString &struuid, QString &strtext, const int &ipage)
{
    if (!DocummentProxy::instance()) {
        return;
    }
    DocummentProxy::instance()->getAnnotationText(struuid, strtext, ipage);
}

void DocummentFileHelper::jumpToHighLight(const QString &uuid, const int &ipage)
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DataManager::instance()->setBIsUpdate(true);
    DocummentProxy::instance()->jumpToHighLight(uuid, ipage);
}

void DocummentFileHelper::search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color)
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DocummentProxy::instance()->search(strtext, resmap, color);
}

void DocummentFileHelper::findNext()
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DocummentProxy::instance()->findNext();
}

void DocummentFileHelper::findPrev()
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DocummentProxy::instance()->findPrev();
}

bool DocummentFileHelper::getAutoPlaySlideStatu()
{
    if (!DocummentProxy::instance()) {
        return false;
    }
    return DocummentProxy::instance()->getAutoPlaySlideStatu();
}

void DocummentFileHelper::setAutoPlaySlide(const bool &autoplay, const int &timemsec)
{
    if (!DocummentProxy::instance()) {
        return ;
    }
    DocummentProxy::instance()->setAutoPlaySlide(autoplay, timemsec);
}

//Outline DocummentFileHelper::outline()
//{
//    if (!DocummentProxy::instance()) {
//        return Outline();
//    }
//    return  DocummentProxy::instance()->outline();
//}
