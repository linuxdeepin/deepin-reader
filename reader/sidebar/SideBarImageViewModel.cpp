/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "SideBarImageViewModel.h"
#include "DocSheet.h"
#include "threadmanager/ReaderImageThreadPoolManager.h"

ImagePageInfo_t::ImagePageInfo_t(int index): pageIndex(index)
{

}

bool ImagePageInfo_t::operator == (const ImagePageInfo_t &other) const
{
    return (this->pageIndex == other.pageIndex);
}

bool ImagePageInfo_t::operator < (const ImagePageInfo_t &other) const
{
    return (this->pageIndex < other.pageIndex);
}

bool ImagePageInfo_t::operator > (const ImagePageInfo_t &other) const
{
    return (this->pageIndex > other.pageIndex);
}

SideBarImageViewModel::SideBarImageViewModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_parent(parent)
{
    m_docSheet = nullptr;
}

void SideBarImageViewModel::resetData()
{
    beginResetModel();
    m_pagelst.clear();
    endResetModel();
}

void SideBarImageViewModel::initModelLst(const QList<ImagePageInfo_t> &pagelst, bool sort)
{
    beginResetModel();

    m_pagelst = pagelst;

    if (sort)
        std::sort(m_pagelst.begin(), m_pagelst.end());

    endResetModel();
}

void SideBarImageViewModel::changeModelData(const QList<ImagePageInfo_t> &pagelst)
{
    m_pagelst = pagelst;
}

void SideBarImageViewModel::setDocSheet(DocSheet *sheet)
{
    m_docSheet = sheet;
}

void SideBarImageViewModel::setBookMarkVisible(int index, bool visible, bool updateIndex)
{
    m_cacheBookMarkMap.insert(index, visible);
    if (updateIndex) {
        const QList<QModelIndex> &modelIndexlst = getModelIndexForPageIndex(index);
        for (const QModelIndex &modelIndex : modelIndexlst)
            emit dataChanged(modelIndex, modelIndex);
    }
}

int SideBarImageViewModel::rowCount(const QModelIndex &) const
{
    return m_pagelst.size();
}

int SideBarImageViewModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant SideBarImageViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int nRow = m_pagelst.at(index.row()).pageIndex;
    if (role == ImageinfoType_e::IMAGE_PIXMAP) {
        const QPixmap &image = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, nRow);
        if (image.isNull()) {
            QImage srcimg;
            if (m_docSheet->getImage(nRow, srcimg, 198, 198, true) && !srcimg.isNull())
                return QVariant::fromValue(srcimg);
            else
                onFetchImage(nRow);
        } else {
            return QVariant::fromValue(image);
        }
    } else if (role == ImageinfoType_e::IMAGE_BOOKMARK) {
        if (m_cacheBookMarkMap.contains(nRow)) {
            return QVariant::fromValue(m_cacheBookMarkMap.value(nRow));
        }
        return QVariant::fromValue(false);
    } else if (role == ImageinfoType_e::IMAGE_ROTATE) {
        return QVariant::fromValue(m_docSheet->operation().rotation * 90);
    } else if (role == ImageinfoType_e::IMAGE_INDEX_TEXT) {
        return QVariant::fromValue(tr("Page %1").arg(nRow + 1));
    } else if (role == ImageinfoType_e::IMAGE_CONTENT_TEXT) {
        return QVariant::fromValue(m_pagelst.at(index.row()).strcontents);
    } else if (role == ImageinfoType_e::IMAGE_SEARCH_COUNT) {
        return QVariant::fromValue(m_pagelst.at(index.row()).strSearchcount);
    } else if (role == Qt::AccessibleTextRole) {
        return index.row();
    } else if (role == ImageinfoType_e::IMAGE_PAGE_SIZE) {
        return QVariant::fromValue(m_docSheet->pageSizeByIndex(nRow));
    }
    return QVariant();
}

bool SideBarImageViewModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    if (!index.isValid())
        return false;
    return QAbstractListModel::setData(index, data, role);
}

QList<QModelIndex> SideBarImageViewModel::getModelIndexForPageIndex(int pageIndex)
{
    QList<QModelIndex> modelIndexlst;
    int pageSize = m_pagelst.size();
    for (int index = 0; index < pageSize; index++) {
        if (m_pagelst.at(index) == pageIndex)
            modelIndexlst << this->index(index);
    }
    return modelIndexlst;
}

int SideBarImageViewModel::getPageIndexForModelIndex(int row)
{
    if (row >= 0 && row < m_pagelst.size())
        return m_pagelst.at(row).pageIndex;
    return -1;
}

void SideBarImageViewModel::onUpdatePageImage(int pageIndex)
{
    const QList<QModelIndex> &modelIndexlst = getModelIndexForPageIndex(pageIndex);
    for (const QModelIndex &modelIndex : modelIndexlst)
        emit dataChanged(modelIndex, modelIndex);
}

void SideBarImageViewModel::onFetchImage(int index, bool force) const
{
    ReaderImageParam_t tParam;
    tParam.maxPixel = 198;
    tParam.bForceUpdate = force;
    tParam.pageIndex = index;
    tParam.sheet = m_docSheet;
    tParam.receiver = m_parent;
    tParam.slotFun = "onUpdatePageImage";
    ReaderImageThreadPoolManager::getInstance()->addgetDocImageTask(tParam);
}

void SideBarImageViewModel::updatePageIndex(int index, bool force, bool bSrc)
{
    if (bSrc) {
        QImage img;
        if (m_docSheet->getImage(index, img, 198, 198, bSrc) && !img.isNull()) {
            ReaderImageThreadPoolManager::getInstance()->setImageForDocSheet(m_docSheet, index, QPixmap::fromImage(img));
            onUpdatePageImage(index);
        } else {
            onFetchImage(index, force);
        }
    } else {
        onFetchImage(index, force);
    }
}

void SideBarImageViewModel::insertPageIndex(int pageIndex)
{
    if (!m_pagelst.contains(pageIndex)) {
        int iterIndex = 0;
        int rowCount = m_pagelst.size();
        for (iterIndex = 0; iterIndex < rowCount; iterIndex++) {
            if (pageIndex < m_pagelst.at(iterIndex).pageIndex)
                break;
        }
        ImagePageInfo_t tImageinfo = pageIndex;
        m_pagelst.insert(iterIndex, tImageinfo);
        beginInsertRows(this->index(iterIndex).parent(), iterIndex, iterIndex);
        endInsertRows();
    }
}

void SideBarImageViewModel::insertPageIndex(const ImagePageInfo_t &tImagePageInfo)
{
    int index = -1;
    if (tImagePageInfo.annotation == nullptr) {
        index = m_pagelst.indexOf(tImagePageInfo);
    } else {
        index = findItemForAnno(tImagePageInfo.annotation);
    }

    if (index == -1) {
        int iterIndex = 0;
        int rowCount = m_pagelst.size();
        for (iterIndex = 0; iterIndex < rowCount; iterIndex++) {
            if (tImagePageInfo.pageIndex < m_pagelst.at(iterIndex).pageIndex)
                break;
        }
        m_pagelst.insert(iterIndex, tImagePageInfo);
        beginInsertRows(this->index(iterIndex).parent(), iterIndex, iterIndex);
        endInsertRows();
    } else {
        m_pagelst[index].strcontents = tImagePageInfo.strcontents;
        emit dataChanged(this->index(index), this->index(index));
    }
}

void SideBarImageViewModel::removePageIndex(int pageIndex)
{
    if (m_pagelst.contains(pageIndex)) {
        beginResetModel();
        m_pagelst.removeAll(pageIndex);
        endResetModel();
    }
}

void SideBarImageViewModel::removeItemForAnno(deepin_reader::Annotation *annotation)
{
    int index = findItemForAnno(annotation);
    if (index >= 0) {
        beginResetModel();
        m_pagelst.removeAt(index);
        endResetModel();
    }
}

void SideBarImageViewModel::getModelIndexImageInfo(int modelIndex, ImagePageInfo_t &tImagePageInfo)
{
    if (modelIndex >= 0 && modelIndex < m_pagelst.size()) {
        tImagePageInfo = m_pagelst.at(modelIndex);
    }
}

int SideBarImageViewModel::findItemForAnno(deepin_reader::Annotation *annotation)
{
    int count = m_pagelst.size();
    for (int index = 0; index < count; index++) {
        if (annotation == m_pagelst.at(index).annotation) {
            return index;
        }
    }
    return -1;
}
