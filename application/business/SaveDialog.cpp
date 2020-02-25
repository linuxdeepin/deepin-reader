/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
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
#include "SaveDialog.h"

#include <DDialog>

DWIDGET_USE_NAMESPACE

#include "MsgHeader.h"

#include "application.h"
#include "FileData.h"
#include "controller/DataManager.h"

SaveDialog::SaveDialog(QObject *parent)
    : QObject(parent)
{

}

void SaveDialog::showSaveDialog(const int &msgType, const QString &sPath)
{
    if (E_APP_MSG == msgType) {
        AppExit();
    } else {
        TabRemove(sPath);
    }
}

void SaveDialog::AppExit()
{
    QString saveFileList = "";
    QString noChangeFileList = "";

    QMap<QString, FileData> sFileStateMap = dApp->m_pDataManager->qGetFileStateMap();
    QMapIterator<QString, FileData> iter(sFileStateMap);
    while (iter.hasNext()) {
        iter.next();
        if (iter.value().bIsChane) {
            saveFileList.append(iter.key() + Constant::sQStringSep);
        } else {
            noChangeFileList.append(iter.key() + Constant::sQStringSep);
        }
    }

    if (saveFileList != "") {
        int nRes = showDialog();
        if (nRes <= 0) {
            return;
        }

        if (nRes == 2) {     //  保存
            dApp->m_pModelService->notifyMsg(MSG_EXIT_SAVE_FILE, saveFileList);
        } else {    //  不保存
            dApp->m_pModelService->notifyMsg(MSG_EXIT_NOT_SAVE_FILE, saveFileList);
        }
    }

    if (noChangeFileList != "") {
        dApp->m_pModelService->notifyMsg(MSG_EXIT_NOT_CHANGE_FILE, noChangeFileList);
    }
}

void SaveDialog::TabRemove(const QString &sPath)
{
    FileData fd = dApp->m_pDataManager->qGetFileData(sPath);
    if (fd.bIsChane) {
        int nRes = showDialog();
        if (nRes <= 0) {
            return;
        }

        if (nRes == 2) {
            dApp->m_pModelService->notifyMsg(MSG_TAB_SAVE_FILE, sPath);
        } else {    //  不保存
            dApp->m_pModelService->notifyMsg(MSG_TAB_NOT_SAVE_FILE, sPath);
        }
    } else {
        dApp->m_pModelService->notifyMsg(MSG_TAB_NOT_CHANGE_SAVE_FILE, sPath);
    }
}

int SaveDialog::showDialog()
{
    DDialog dlg("", tr("Do you want to save the changes?"));
    dlg.setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    dlg.addButtons(QStringList() <<  tr("Cancel") << tr("Discard"));
    dlg.addButton(tr("Save"), true, DDialog::ButtonRecommend);
    QMargins mar(0, 0, 0, 30);
    dlg.setContentLayoutContentsMargins(mar);

    int nRes = dlg.exec();
    return nRes;
}
