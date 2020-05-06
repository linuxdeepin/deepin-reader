/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     leiyu <leiyu@live.com>
* Maintainer: leiyu <leiyu@deepin.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "imageviewmodel.h"
#include "DocSheet.h"
#include "docview/docummentproxy.h"
#include "threadmanager/readerimagethreadpoolmanager.h"

ImageViewModel::ImageViewModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parent(parent)
{
    m_docSheet = nullptr;
}

void ImageViewModel::initModelLst(const QList<int>& pagelst, bool sort)
{
    beginResetModel();
    m_pagelst = pagelst;
    if (sort) qSort(m_pagelst.begin(), m_pagelst.end());
    endResetModel();
}

void ImageViewModel::setDocSheet(DocSheet *sheet)
{
    m_docSheet = sheet;
}

void ImageViewModel::setBookMarkVisible(int pageIndex, bool visible, bool updateIndex)
{
    m_cacheBookMarkMap.insert(pageIndex, visible);
    if(updateIndex){
        const QModelIndex &modelIndex = getModelIndexForPageIndex(pageIndex);
        emit dataChanged(modelIndex, modelIndex);
    }
}

void ImageViewModel::updatePageIndex(int pageIndex)
{
    onFetchImage(pageIndex);
}

int ImageViewModel::rowCount(const QModelIndex &) const
{
    return m_pagelst.size();
}

int ImageViewModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant ImageViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int nRow = m_pagelst.at(index.row());
    if(role == ImageinfoType_e::IMAGE_PIXMAP){
        const QPixmap &image =ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet->getDocProxy(), nRow);
        if(image.isNull())
            onFetchImage(nRow);
        else{
            return QVariant::fromValue(image);
        }
    }
    else if(role == ImageinfoType_e::IMAGE_BOOKMARK){
        if(m_cacheBookMarkMap.contains(nRow)){
            return QVariant::fromValue(m_cacheBookMarkMap.value(nRow));
        }
        return QVariant::fromValue(false);
    }
    else if(role == ImageinfoType_e::IMAGE_ROTATE){
        return QVariant::fromValue(m_docSheet->getOper(Rotate).toInt() * 90);
    }
    else if(role == ImageinfoType_e::IMAGE_INDEX_TEXT){
        return QVariant::fromValue(tr("Page %1").arg(nRow + 1));
    }
    return QVariant();
}

bool ImageViewModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    if (!index.isValid())
        return false;
    return QAbstractListModel::setData(index, data, role);
}

QModelIndex ImageViewModel::getModelIndexForPageIndex(int pageIndex)
{
    int index = m_pagelst.indexOf(pageIndex);
    if(index >= 0)
        return this->index(index);
    return QModelIndex();
}

int ImageViewModel::getPageIndexForModelIndex(int row)
{
    if(row >= 0 && row < m_pagelst.size())
        return m_pagelst.at(row);
    return -1;
}

void ImageViewModel::onUpdatePageImage(int pageIndex)
{
    const QModelIndex &modelIndex = getModelIndexForPageIndex(pageIndex);
    emit dataChanged(modelIndex, modelIndex);
}

void ImageViewModel::onFetchImage(int nRow) const
{
    DocummentProxy* docProxy = m_docSheet->getDocProxy();
    if(docProxy){
        ReaderImageParam_t tParam;
        tParam.pageNum = nRow;
        tParam.docProxy = docProxy;
        tParam.receiver = m_parent;
        tParam.slotFun = "onUpdatePageImage";
        ReaderImageThreadPoolManager::getInstance()->addgetDocImageTask(tParam);
    }
}

void ImageViewModel::insertPageIndex(int pageIndex)
{
    if(!m_pagelst.contains(pageIndex)){
        beginResetModel();
        m_pagelst << pageIndex;
        qSort(m_pagelst.begin(), m_pagelst.end());
        endResetModel();
    }
}

void ImageViewModel::removePageIndex(int pageIndex)
{
    if(m_pagelst.contains(pageIndex)){
        beginResetModel();
        m_pagelst.removeAll(pageIndex);
        endResetModel();
    }
}
