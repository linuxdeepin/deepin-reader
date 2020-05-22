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
#include "pdfControl/DocSheetPDF.h"

ProxyFileDataModel::ProxyFileDataModel(QObject *parent) : QObject(parent)
{
    _fvwParent = qobject_cast<SheetBrowserPDFPrivate *>(parent);
}

void ProxyFileDataModel::saveOper()
{
    QString sPath = _fvwParent->m_pProxyData->getPath();
    dApp->m_pDBService->setHistroyData(sPath, Scale, _fvwParent->m_sheet->operation().scaleFactor * 100.0);
    dApp->m_pDBService->setHistroyData(sPath, DoubleShow, _fvwParent->m_sheet->operation().layoutMode);
    dApp->m_pDBService->setHistroyData(sPath, Fit, _fvwParent->m_sheet->operation().scaleMode);
    dApp->m_pDBService->setHistroyData(sPath, Rotate, _fvwParent->m_sheet->operation().rotation);
    dApp->m_pDBService->setHistroyData(sPath, Thumbnail, _fvwParent->m_sheet->operation().sidebarVisible);
    dApp->m_pDBService->setHistroyData(sPath, LeftIndex, _fvwParent->m_sheet->operation().sidebarIndex);
    dApp->m_pDBService->setHistroyData(sPath, CurIndex, _fvwParent->m_sheet->operation().currentPage - 1);
    dApp->m_pDBService->setHistroyData(sPath, HandShape, _fvwParent->m_sheet->operation().mouseShape);
    dApp->m_pDBService->qSaveData(sPath, DB_HISTROY);
}

void ProxyFileDataModel::saveData()
{
    QString sPath = _fvwParent->m_pProxyData->getPath();
    dApp->m_pDBService->setBookMarkList(sPath, _fvwParent->m_sheet->getBookMarkList().values());
    dApp->m_pDBService->qSaveData(sPath, DB_BOOKMARK);
}

void ProxyFileDataModel::saveAsData(const QString &originPath, const QString &targetPath)
{
    dApp->m_pDBService->setBookMarkList(originPath, _fvwParent->m_sheet->getBookMarkList().values());
    dApp->m_pDBService->saveAsData(originPath, targetPath, DB_BOOKMARK);
}

void ProxyFileDataModel::setModel(FileDataModel model)
{
    DocOperation opera;
    opera.layoutMode = (Dr::LayoutMode)model.getOper(DoubleShow).toInt();
    opera.mouseShape = (Dr::MouseShape)model.getOper(HandShape).toInt();
    opera.scaleMode = (Dr::ScaleMode)model.getOper(Fit).toInt();
    opera.rotation = (Dr::Rotation)model.getOper(Rotate).toInt();
    opera.scaleFactor = model.getOper(Scale).toDouble() / 100.0;
    opera.sidebarVisible = model.getOper(Thumbnail).toBool();
    opera.sidebarIndex = model.getOper(LeftIndex).toInt();
    opera.currentPage = model.getOper(CurIndex).toInt() + 1;
    _fvwParent->m_sheet->initOperationData(opera);
}
