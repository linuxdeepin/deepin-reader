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
#include "ProxyFileDataModel.h"

#include "application.h"
#include "pdfControl/SheetBrowserPDFPrivate.h"
#include "MsgHeader.h"
#include "ProxyData.h"

ProxyFileDataModel::ProxyFileDataModel(QObject *parent) : QObject(parent)
{
    _fvwParent = qobject_cast<SheetBrowserPDFPrivate *>(parent);
}

void ProxyFileDataModel::setOper(const int &nType, const QVariant &sValue)
{
    m_fileDataModel.setOper(nType, sValue);
}

QVariant ProxyFileDataModel::getOper(int type)
{
    return m_fileDataModel.getOper(type);
}

void ProxyFileDataModel::saveOper()
{
    QString sPath = _fvwParent->m_pProxyData->getPath();
    for (int iLoop = Scale; iLoop < CurPage + 1; iLoop++) {
        dApp->m_pDBService->setHistroyData(sPath, iLoop, m_fileDataModel.getOper(iLoop));
    }

    dApp->m_pDBService->qSaveData(sPath, DB_HISTROY);
}

void ProxyFileDataModel::saveData()
{
    QString sPath = _fvwParent->m_pProxyData->getPath();
    dApp->m_pDBService->qSaveData(sPath, DB_BOOKMARK);
}

void ProxyFileDataModel::saveAsData(const QString &originPath, const QString &targetPath)
{
    dApp->m_pDBService->saveAsData(originPath, targetPath, DB_BOOKMARK);
}

void ProxyFileDataModel::setModel(FileDataModel model)
{
    m_fileDataModel = model;
}
