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
#include "ImageViewModel.h"
#include "DocSheet.h"
#include "pdfControl/docview/docummentproxy.h"
#include "threadmanager/readerimagethreadpoolmanager.h"

ImageViewModel::ImageViewModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parent(parent)
{
    m_docSheet = nullptr;
}

void ImageViewModel::resetData()
{
    beginResetModel();
    m_pagelst.clear();
    endResetModel();
}

void ImageViewModel::initModelLst(const QList<ImagePageInfo_t> &pagelst, bool sort)
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
    if (updateIndex) {
        const QList<QModelIndex> &modelIndexlst = getModelIndexForPageIndex(pageIndex);
        for (const QModelIndex &modelIndex : modelIndexlst)
            emit dataChanged(modelIndex, modelIndex);
    }
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
    int nRow = m_pagelst.at(index.row()).iPage;
    if (role == ImageinfoType_e::IMAGE_PIXMAP) {
        const QPixmap &image = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet->getDocProxy(), nRow);
        if (image.isNull())
            onFetchImage(nRow);
        else {
            return QVariant::fromValue(image);
        }
    } else if (role == ImageinfoType_e::IMAGE_BOOKMARK) {
        if (m_cacheBookMarkMap.contains(nRow)) {
            return QVariant::fromValue(m_cacheBookMarkMap.value(nRow));
        }
        return QVariant::fromValue(false);
    } else if (role == ImageinfoType_e::IMAGE_ROTATE) {
        return QVariant::fromValue(m_docSheet->getOper(Rotate).toInt() * 90);
    } else if (role == ImageinfoType_e::IMAGE_INDEX_TEXT) {
        return QVariant::fromValue(tr("Page %1").arg(nRow + 1));
    } else if (role == ImageinfoType_e::IMAGE_CONTENT_TEXT) {
        return QVariant::fromValue(m_pagelst.at(index.row()).strcontents);
    } else if (role == ImageinfoType_e::IMAGE_SEARCH_COUNT) {
        return QVariant::fromValue(m_pagelst.at(index.row()).strSearchcount);
    }
    return QVariant();
}

bool ImageViewModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    if (!index.isValid())
        return false;
    return QAbstractListModel::setData(index, data, role);
}

QList<QModelIndex> ImageViewModel::getModelIndexForPageIndex(int pageIndex)
{
    QList<QModelIndex> modelIndexlst;
    int pageSize = m_pagelst.size();
    for (int index = 0; index < pageSize; index++) {
        if (m_pagelst.at(index) == pageIndex)
            modelIndexlst << this->index(index);
    }
    return modelIndexlst;
}

int ImageViewModel::getPageIndexForModelIndex(int row)
{
    if (row >= 0 && row < m_pagelst.size())
        return m_pagelst.at(row).iPage;
    return -1;
}

void ImageViewModel::onUpdatePageImage(int pageIndex)
{
    const QList<QModelIndex> &modelIndexlst = getModelIndexForPageIndex(pageIndex);
    for (const QModelIndex &modelIndex : modelIndexlst)
        emit dataChanged(modelIndex, modelIndex);
}

void ImageViewModel::onFetchImage(int nRow) const
{
    DocummentProxy *docProxy = m_docSheet->getDocProxy();
    if (docProxy) {
        ReaderImageParam_t tParam;
        tParam.pageNum = nRow;
        tParam.docProxy = docProxy;
        tParam.receiver = m_parent;
        tParam.slotFun = "onUpdatePageImage";
        ReaderImageThreadPoolManager::getInstance()->addgetDocImageTask(tParam);
    }
}

void ImageViewModel::updatePageIndex(int pageIndex)
{
    onFetchImage(pageIndex);
}

void ImageViewModel::insertPageIndex(int pageIndex)
{
    if (!m_pagelst.contains(pageIndex)) {
        beginResetModel();
        m_pagelst << pageIndex;
        qSort(m_pagelst.begin(), m_pagelst.end());
        endResetModel();
    }
}

void ImageViewModel::insertPageIndex(const ImagePageInfo_t &tImagePageInfo)
{
    int index = findItemForuuid(tImagePageInfo.struuid);
    if (index == -1) {
        beginResetModel();
        m_pagelst << tImagePageInfo;
        qSort(m_pagelst.begin(), m_pagelst.end());
        endResetModel();
    } else {
        m_pagelst[index].strcontents = tImagePageInfo.strcontents;
        emit dataChanged(this->index(index), this->index(index));
    }
}

void ImageViewModel::removePageIndex(int pageIndex)
{
    if (m_pagelst.contains(pageIndex)) {
        beginResetModel();
        m_pagelst.removeAll(pageIndex);
        endResetModel();
    }
}

void ImageViewModel::removeModelIndex(int modelIndex)
{
    if (modelIndex >= 0 && modelIndex < m_pagelst.size()) {
        m_pagelst.removeAt(modelIndex);
    }
}

void ImageViewModel::removeItemForuuid(const QString &uuid)
{
    int index = findItemForuuid(uuid);
    if (index >= 0) {
        beginResetModel();
        m_pagelst.removeAt(index);
        endResetModel();
    }
}

void ImageViewModel::getModelIndexImageInfo(int modelIndex, ImagePageInfo_t &tImagePageInfo)
{
    if (modelIndex >= 0 && modelIndex < m_pagelst.size()) {
        tImagePageInfo = m_pagelst.at(modelIndex);
    }
}

int ImageViewModel::findItemForuuid(const QString &uuid)
{
    int count = m_pagelst.size();
    for (int index = 0; index < count; index++) {
        if (uuid == m_pagelst.at(index).struuid) {
            return index;
        }
    }
    return -1;
}
