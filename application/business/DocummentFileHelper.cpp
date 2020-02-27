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

#include <QClipboard>
#include <DFileDialog>
#include <DDialog>
#include <QJsonObject>

#include "FileFormatHelper.h"

#include "controller/FileDataManager.h"

#include "docview/docummentproxy.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"

DocummentFileHelper::DocummentFileHelper(QObject *parent)
    : QObject(parent)
{
    m_strObserverName = "DocummentFileHelper";
    m_pMsgList = { MSG_OPEN_FILE_PATH, MSG_OPERATION_SAVE_AS_FILE,
                   MSG_OPERATION_TEXT_COPY,
                   MSG_DOC_JUMP_PAGE, MSG_OPERATION_FIRST_PAGE, MSG_OPERATION_PREV_PAGE, MSG_OPERATION_NEXT_PAGE, MSG_OPERATION_END_PAGE
                 };

    m_pKeyMsgList = {KeyStr::g_ctrl_s, KeyStr::g_ctrl_shift_s, KeyStr::g_ctrl_c};

    initConnections();

    dApp->m_pModelService->addObserver(this);
}

void DocummentFileHelper::initConnections()
{
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)),
            SLOT(slotDealWithData(const int &, const QString &)));
    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)),
            SLOT(slotDealWithKeyMsg(const QString &)));

    connect(this, SIGNAL(sigFileSlider(const int &)), SLOT(slotFileSlider(const int &)));
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
        }
    }
    return 0;
}

void DocummentFileHelper::slotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPEN_FILE_PATH) {            //  打开单个文件
        onOpenFile(msgContent);
    } /*else if (msgType == MSG_OPEN_FILE_PATH_S) {   //  打开多个文件
        onOpenFiles(msgContent);
    }*/ else if (msgType == MSG_OPERATION_SAVE_AS_FILE) { //  另存为文件
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
    } else if (msgContent == KeyStr::g_ctrl_c) {
        __FileCtrlCContent();
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
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

//bool DocummentFileHelper::save(const QString &filepath, bool withChanges)
//{
//    if (!DocummentProxy::instance()) {
//        return false;
//    }

//    return DocummentProxy::instance()->save(filepath, withChanges);
//}

void DocummentFileHelper::qRemoveTabFile(const int &iType, const QString &sPath)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        dApp->m_pDBService->qSaveData(sPath, DB_HISTROY);

        if (MSG_TAB_SAVE_FILE == iType || MSG_TAB_NOT_SAVE_FILE == iType) {
            bool bSave = iType == MSG_TAB_SAVE_FILE ? true : false;
            _proxy->save(sPath, bSave);

            if (bSave) {
                dApp->m_pDBService->qSaveData(sPath, DB_BOOKMARK);
            }
        }

        dApp->m_pDataManager->qSaveData(sPath);

        _proxy->closeFile();

        notifyMsg(MSG_CLOSE_FILE, sPath);
    }
}

void DocummentFileHelper::qAppExitFile(const int &iType, const QString &sPath)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        dApp->m_pDBService->qSaveData(sPath, DB_HISTROY);

        if (MSG_EXIT_SAVE_FILE == iType || MSG_EXIT_NOT_SAVE_FILE == iType) {
            bool bSave = iType == MSG_EXIT_SAVE_FILE ? true : false;
            _proxy->save(sPath, bSave);

            if (bSave) {
                dApp->m_pDBService->qSaveData(sPath, DB_BOOKMARK);
            }
        }

        dApp->m_pDataManager->qSaveData(sPath);

        _proxy->closeFile();
    }

    notifyMsg(E_APP_EXIT, sPath);
}

//  保存 数据
void DocummentFileHelper::onSaveFile()
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {

        QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
        FileState fs = dApp->m_pDataManager->qGetFileChange(sCurPath);
        if (fs.isChange) {  //  改变了
            bool rl = _proxy->save(sCurPath, true);
            qDebug() << "slotSaveFile" << rl;
            if (rl) {
                //  保存需要保存 数据库记录
                dApp->m_pDBService->qSaveData(sCurPath, DB_BOOKMARK);

                dApp->m_pDataManager->qInsertFileChange(sCurPath, 1);

                notifyMsg(CENTRAL_SHOW_TIP, tr("Saved successfully"));
            } else {
                notifyMsg(CENTRAL_SHOW_TIP, tr("Saved failed"));
            }
        } else {
            notifyMsg(CENTRAL_SHOW_TIP, tr("No changes"));
        }
        //insert msg to FileFontTable
        dApp->m_pDataManager->qSaveData(sCurPath);
    }
}

//  另存为
void DocummentFileHelper::onSaveAsFile()
{
    if (!DocummentProxy::instance()) {
        return;
    }
    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    QFileInfo info(sCurPath);

    QString sCompleteSuffix = info.completeSuffix();
    DocType_EM nCurDocType = FFH::setCurDocuType(sCompleteSuffix);

    QString sFilter = FFH::getFileFilter(nCurDocType);

    if (sFilter != "") {
        QFileDialog dialog;
        dialog.selectFile(sCurPath);
        QString filePath = dialog.getSaveFileName(nullptr, tr("Save as"), sCurPath, sFilter);

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
            if (sCurPath == filePath) {
                onSaveFile();
            } else {
                QString sFilePath = FFH::getFilePath(filePath, nCurDocType);

                bool rl = DocummentProxy::instance()->saveas(sFilePath, true);
                if (rl) {
                    //insert a new bookmark record to bookmarktabel
                    dApp->m_pDBService->qSaveData(sFilePath, DB_BOOKMARK);
                    dApp->m_pDBService->qSaveData(sFilePath, DB_HISTROY);

                    notifyMsg(MSG_OPERATION_OPEN_FILE_OK, sFilePath);
                }
            }
        }
    }
}

//  跳转页面
void DocummentFileHelper::__PageJump(const int &pagenum)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        int nPageSize = _proxy->getPageSNum();      //  总页数
        if (pagenum < 0 || pagenum == nPageSize) {
            return;
        }

        int nCurPage = _proxy->currentPageNo();
        if (nCurPage != pagenum) {
            _proxy->pageJump(pagenum);
        }
    }
}

//  前一页\第一页\后一页\最后一页 操作
void DocummentFileHelper::__PageJumpByMsg(const int &iType)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        int iPage = -1;
        if (iType == MSG_OPERATION_FIRST_PAGE) {
            iPage = 0;
        } else if (iType == MSG_OPERATION_PREV_PAGE) {
            int nCurPage = _proxy->currentPageNo();
            iPage = nCurPage - 1;
        } else if (iType == MSG_OPERATION_NEXT_PAGE) {
            int nCurPage = _proxy->currentPageNo();
            iPage = nCurPage + 1;
        } else if (iType == MSG_OPERATION_END_PAGE) {
            int nCurPage = _proxy->getPageSNum();
            iPage = nCurPage - 1;
        }

        __PageJump(iPage);
    }
}

//  打开　文件路径
void DocummentFileHelper::onOpenFile(const QString &filePaths)
{
    DocummentProxy *proxy =  DocummentProxy::instance(dApp->m_pDataManager->qGetFileUuid(filePaths));
    if (!proxy) {
        return;
    }

    QStringList fileList = filePaths.split(Constant::sQStringSep,  QString::SkipEmptyParts);
    int nSize = fileList.size();
    if (nSize > 0) {
        QString sPath = fileList.at(0);

        QFileInfo info(sPath);

        QString sCompleteSuffix = info.completeSuffix();
        DocType_EM nCurDocType = FFH::setCurDocuType(sCompleteSuffix);

        bool rl = false;
        notifyMsg(MSG_DOC_OPEN_FILE_START, sPath);
        //从数据库中获取文件的字号信息
        dApp->m_pDBService->qSelectData(sPath, DB_HISTROY);
        FileDataModel fdm = dApp->m_pDBService->getHistroyData(sPath);

        dApp->m_pDataManager->qSetFileData(sPath, fdm);

        int curPage = fdm.qGetData(CurPage);
        if (curPage != -1) {
            int iscale = fdm.qGetData(Scale);          // 缩放
            int doubPage = fdm.qGetData(DoubleShow);     // 是否是双页
            int rotate = fdm.qGetData(Rotate);         // 文档旋转角度(0~360)

            iscale = (iscale > 500 ? 500 : iscale) <= 0 ? 100 : iscale;
            double scale = iscale / 100.0;
            RotateType_EM rotatetype = static_cast<RotateType_EM>((rotate / 90) + 1);
            ViewMode_EM viewmode = static_cast<ViewMode_EM>(doubPage);

            rl = proxy->openFile(nCurDocType, sPath, curPage, rotatetype, scale, viewmode);
        } else {
            rl = proxy->openFile(nCurDocType, sPath);
        }

        if (!rl) {
            notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("File not supported"));
        }
    }
}

//void DocummentFileHelper::onOpenFiles(const QString &filePaths)
//{
//    bool bisopen = false;
//    QStringList canOpenFileList = filePaths.split(Constant::sQStringSep, QString::SkipEmptyParts);
//    foreach (auto s, canOpenFileList) {
//        QString sOpenPath = DataManager::instance()->strOnlyFilePath();
//        if (s == sOpenPath) {
//            notifyMsg(CENTRAL_SHOW_TIP, tr("The file is already open"));
//        } else {
//            QString sRes = s + Constant::sQStringSep;
//            QString sOpenPath = DataManager::instance()->strOnlyFilePath(); //  打开文档为空
//            if (sOpenPath == "") {
//                if (!bisopen)
//                    notifyMsg(MSG_OPEN_FILE_PATH, sRes);
//                else {
//                    if (!Utils::runApp(s))
//                        qDebug() << __FUNCTION__ << "process start deepin-reader failed";
//                }

//                bisopen = true;
//            } else {
//                if (!Utils::runApp(s))
//                    qDebug() << __FUNCTION__ << "process start deepin-reader failed";
//            }
//        }
//    }
//}

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
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy) {
        return;
    }

    if (nFlag == 1) {
        bool bSlideModel = _proxy->showSlideModel();    //  开启幻灯片
        if (bSlideModel) {
            _proxy->setAutoPlaySlide(true);
            dApp->m_pAppInfo->qSetCurShowState(FILE_SLIDE);
        }
    } else {
        if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE) {
            bool rl = _proxy->exitSlideModel();
            if (rl) {
                dApp->m_pAppInfo->qSetCurShowState(FILE_NORMAL);
            }
        }
    }
}

void DocummentFileHelper::__FileCtrlCContent()
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy) {
        return;
    }

    QString sSelectText = "";
    if (_proxy->getSelectTextString(sSelectText)) { //  选择　当前选中下面是否有文字
        if (sSelectText != "") {
            slotCopySelectContent(sSelectText);
        }
    }
}

