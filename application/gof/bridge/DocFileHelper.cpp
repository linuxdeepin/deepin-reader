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

#include <DFileDialog>
#include <DDialog>

#include <QJsonObject>
#include <QJsonDocument>

#include "business/FileFormatHelper.h"

#include "docview/docummentproxy.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"

#include "widgets/main/MainTabWidgetEx.h"

DocFileHelper::DocFileHelper(QObject *parent)
    : HelperImpl(parent)
{
    m_pMsgList = { MSG_DOC_JUMP_PAGE, MSG_OPERATION_FIRST_PAGE, MSG_OPERATION_END_PAGE, MSG_OPERATION_PREV_PAGE, MSG_OPERATION_NEXT_PAGE,
                   MSG_SAVE_FILE, MSG_NOT_SAVE_FILE, MSG_NOT_CHANGE_SAVE_FILE
                 };
}
//  通知消息, 不需要撤回
void DocFileHelper::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

void DocFileHelper::CloseFile(const int &iType, const QString &sPath)
{
    DocummentProxy *_proxy = MainTabWidgetEx::Instance()->getCurFileAndProxy(sPath);
    if (_proxy) {
        if (MSG_SAVE_FILE == iType || MSG_NOT_SAVE_FILE == iType) {
            bool bSave = iType == MSG_SAVE_FILE ? true : false;
            _proxy->save(sPath, bSave);

            if (bSave) {
                dApp->m_pDBService->qSaveData(sPath, DB_BOOKMARK);
            }
        }
        _proxy->closeFile();
    }
}

//  保存 数据
void DocFileHelper::onSaveFile()
{
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    if (pMtwe) {
        QString sCurPath = pMtwe->qGetCurPath();
        if (sCurPath != "") {
            int fs = pMtwe->GetFileChange(sCurPath);
            if (fs == 1) {  //  改变了
                DocummentProxy *_proxy = pMtwe->getCurFileAndProxy(sCurPath);
                if (_proxy) {
                    bool rl = _proxy->save(sCurPath, true);
                    qDebug() << "slotSaveFile" << rl;
                    if (rl) {
                        //  保存需要保存 数据库记录
                        dApp->m_pDBService->qSaveData(sCurPath, DB_BOOKMARK);

                        notifyMsg(CENTRAL_SHOW_TIP, tr("Saved successfully"));
                    } else {
                        notifyMsg(CENTRAL_SHOW_TIP, tr("Saved failed"));
                    }
                }
            } else {
                notifyMsg(CENTRAL_SHOW_TIP, tr("No changes"));
            }
        }
    }
}

//  另存为
void DocFileHelper::onSaveAsFile()
{
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    if (pMtwe) {
        QString sCurPath = pMtwe->qGetCurPath();
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

                        bool rl = pMtwe->getCurFileAndProxy(sCurPath)->saveas(sFilePath, true);
                        if (rl) {
                            //insert a new bookmark record to bookmarktabel
                            dApp->m_pDBService->qSaveData(sFilePath, DB_BOOKMARK);

                            notifyMsg(MSG_OPERATION_OPEN_FILE_OK, sFilePath);
                        }
                    }
                }
            }
        }
    }
}


//  跳转页面


QString DocFileHelper::qDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_DOC_JUMP_PAGE) {              //  请求跳转页面
        __PageJump(msgContent.toInt());
    } else if (msgType == MSG_OPERATION_FIRST_PAGE || msgType == MSG_OPERATION_PREV_PAGE ||
               msgType == MSG_OPERATION_NEXT_PAGE || msgType == MSG_OPERATION_END_PAGE) {
        __PageJumpByMsg(msgType);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_ctrl_shift_s) {
            onSaveAsFile();
        }
    } else if (MSG_SAVE_FILE == msgType || MSG_NOT_SAVE_FILE == msgType || MSG_NOT_CHANGE_SAVE_FILE == msgType)  {
        CloseFile(msgType, msgContent);
    } else if (msgType == MSG_SAVE_AS_FILE_PATH) {
        onSaveAsFile();
    }

    int nRes = MSG_NO_OK;
    if (m_pMsgList.contains(msgType)) {
        nRes = MSG_OK;
    }

    QJsonObject obj;
    obj.insert("return", nRes);

    QJsonDocument doc(obj);

    return doc.toJson(QJsonDocument::Compact);
}

void DocFileHelper::__PageJump(const int &pagenum)
{
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    if (pMtwe) {
        QString sCurPath = pMtwe->qGetCurPath();
        if (sCurPath != "") {
            DocummentProxy *_proxy =  pMtwe->getCurFileAndProxy(sCurPath);
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
    }
}

//  前一页\第一页\后一页\最后一页 操作
void DocFileHelper::__PageJumpByMsg(const int &iType)
{
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    if (pMtwe) {
        QString sCurPath = pMtwe->qGetCurPath();
        if (sCurPath != "") {
            DocummentProxy *_proxy =  pMtwe->getCurFileAndProxy(sCurPath);
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
    }
}
