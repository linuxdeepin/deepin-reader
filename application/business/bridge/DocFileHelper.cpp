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

#include "DocFileHelper.h"

#include <QClipboard>
#include <DFileDialog>
#include <DDialog>

#include <QJsonObject>
#include <QJsonDocument>

#include "../FileFormatHelper.h"

#include "business/FileDataManager.h"
#include "docview/docummentproxy.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"

DocFileHelper::DocFileHelper(QObject *parent)
    : HelperImpl(parent)
{

}
//  通知消息, 不需要撤回
void DocFileHelper::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

void DocFileHelper::RemoveTabFile(const int &iType, const QString &sPath)
{
    QString sUuid = dApp->m_pDataManager->qGetFileUuid(sPath);
    if (sUuid != "") {
        DocummentProxy *_proxy = DocummentProxy::instance(sUuid);
        if (_proxy) {
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
}

void DocFileHelper::AppExitFile(const int &iType, const QString &sPaths)
{
    //  应用退出
    QStringList pathList = sPaths.split(Constant::sQStringSep, QString::SkipEmptyParts);
    foreach (const QString &s, pathList) {
        QString sUuid = dApp->m_pDataManager->qGetFileUuid(s);
        if (sUuid != "") {
            DocummentProxy *_proxy = DocummentProxy::instance(sUuid);
            if (_proxy) {
                if (APP_EXIT_SAVE_FILE == iType || APP_EXIT_NOT_SAVE_FILE == iType) {
                    bool bSave = iType == APP_EXIT_SAVE_FILE ? true : false;
                    bool rl = _proxy->save(s, bSave);
                    if (rl) {
                        if (bSave) {
                            dApp->m_pDBService->qSaveData(s, DB_BOOKMARK);
                        }
                    } else {
                        qDebug() << __FUNCTION__ << "            ";
                    }
                }

                dApp->m_pDataManager->qSaveData(s);

                _proxy->closeFile();
            }
        }
    }
}

//  保存 数据
void DocFileHelper::onSaveFile()
{
    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    FileState fs = dApp->m_pDataManager->qGetFileChange(sCurPath);
    if (fs.isChange) {  //  改变了
        DocummentProxy *_proxy = dApp->m_pDataManager->qGetCurrentProxy();
        if (_proxy) {
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
        }
    } else {
        notifyMsg(CENTRAL_SHOW_TIP, tr("No changes"));
    }
    //insert msg to FileFontTable
    dApp->m_pDataManager->qSaveData(sCurPath);
}

//  另存为
void DocFileHelper::onSaveAsFile()
{
    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    if (sCurPath != "") {
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

                        dApp->m_pDataManager->qSaveData(sFilePath);

                        notifyMsg(MSG_OPERATION_OPEN_FILE_OK, sFilePath);
                    }
                }
            }
        }
    }
}

//  跳转页面


int DocFileHelper::qDealWithData(const int &msgType, const QString &msgContent)
{
    int nRes = MSG_NO_OK;
    if (msgType == MSG_OPEN_FILE_PATH) {            //  打开单个文件
        onOpenFile(msgContent);
        nRes = MSG_OK;
    } else if (msgType == MSG_OPERATION_TEXT_COPY) {    //  复制
        OnCopySelectContent(msgContent);
        nRes = MSG_OK;
    } else if (msgType == MSG_DOC_JUMP_PAGE) {              //  请求跳转页面
        __PageJump(msgContent.toInt());
        nRes = MSG_OK;
    } else if (msgType == MSG_OPERATION_FIRST_PAGE || msgType == MSG_OPERATION_PREV_PAGE ||
               msgType == MSG_OPERATION_NEXT_PAGE || msgType == MSG_OPERATION_END_PAGE) {
        __PageJumpByMsg(msgType);
        nRes = MSG_OK;
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_ctrl_s) {
            onSaveFile();
            nRes = MSG_OK;
        } else if (msgContent == KeyStr::g_ctrl_shift_s) {
            onSaveAsFile();
            nRes = MSG_OK;
        } else if (msgContent == KeyStr::g_ctrl_c) {
            __FileCtrlCContent();
            nRes = MSG_OK;
        }
    } else if (msgType == MSG_OPERATION_SLIDE) {
        SetFileSlider(msgContent.toInt());
        nRes = MSG_OK;
    } else if (MSG_TAB_SAVE_FILE == msgType || MSG_TAB_NOT_SAVE_FILE == msgType || MSG_TAB_NOT_CHANGE_SAVE_FILE == msgType)  {
        RemoveTabFile(msgType, msgContent);
        nRes = MSG_OK;
    } else if (APP_EXIT_SAVE_FILE == msgType || APP_EXIT_NOT_SAVE_FILE == msgType || APP_EXIT_NOT_CHANGE_FILE == msgType) {
        AppExitFile(msgType, msgContent);
        nRes = MSG_OK;
    }
    return nRes;
}

void DocFileHelper::__PageJump(const int &pagenum)
{
    DocummentProxy *_proxy =  dApp->m_pDataManager->qGetCurrentProxy();
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
void DocFileHelper::__PageJumpByMsg(const int &iType)
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
void DocFileHelper::onOpenFile(const QString &filePaths)
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

        //从数据库中获取文件的字号信息
        dApp->m_pDBService->qSelectData(sPath, DB_HISTROY);
        FileDataModel fdm = dApp->m_pDBService->getHistroyData(sPath);

        dApp->m_pDataManager->qSetFileData(sPath, fdm);

        int curPage = fdm.qGetData(CurPage);
        int iscale = fdm.qGetData(Scale);          // 缩放
        int doubPage = fdm.qGetData(DoubleShow);     // 是否是双页
        int rotate = fdm.qGetData(Rotate);         // 文档旋转角度(0,1,2,3,4)

        iscale = (iscale > 500 ? 500 : iscale) <= 0 ? 100 : iscale;
        double scale = iscale / 100.0;
        RotateType_EM rotatetype = static_cast<RotateType_EM>(rotate);
        ViewMode_EM viewmode = static_cast<ViewMode_EM>(doubPage);

        bool rl = proxy->openFile(nCurDocType, sPath, curPage, rotatetype, scale, viewmode);
        if (!rl) {
            notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("File not supported"));
        } else {
            dApp->m_pDataManager->qInsertOpenFilePath(sPath);

            notifyMsg(MSG_DOC_OPEN_FILE_START, sPath);
        }
    }
}

//  复制
void DocFileHelper::OnCopySelectContent(const QString &sCopy)
{
    QClipboard *clipboard = DApplication::clipboard();   //获取系统剪贴板指针
    clipboard->setText(sCopy);
}

void DocFileHelper::__FileCtrlCContent()
{
    DocummentProxy *_proxy = dApp->m_pDataManager->qGetCurrentProxy();
    if (!_proxy) {
        return;
    }

    QString sSelectText = "";
    if (_proxy->getSelectTextString(sSelectText)) { //  选择　当前选中下面是否有文字
        if (sSelectText != "") {
            OnCopySelectContent(sSelectText);
        }
    }
}


void DocFileHelper::SetFileSlider(const int &nFlag)
{
    DocummentProxy *_proxy = dApp->m_pDataManager->qGetCurrentProxy();
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
