// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SideBarImageViewModel.h"
#include "DocSheet.h"
#include "PageRenderThread.h"
#include "Application.h"
#include "ddlog.h"

#include <QTimer>
#include <QDebug>

ImagePageInfo_t::ImagePageInfo_t(int index): pageIndex(index)
{
    // qCDebug(appLog) << "ImagePageInfo_t created for page index:" << index;
}

bool ImagePageInfo_t::operator == (const ImagePageInfo_t &other) const
{
    // qCDebug(appLog) << "ImagePageInfo_t == other";
    return (this->pageIndex == other.pageIndex);
}

bool ImagePageInfo_t::operator < (const ImagePageInfo_t &other) const
{
    // qCDebug(appLog) << "ImagePageInfo_t < other";
    return (this->pageIndex < other.pageIndex);
}

bool ImagePageInfo_t::operator > (const ImagePageInfo_t &other) const
{
    // qCDebug(appLog) << "ImagePageInfo_t > other";
    return (this->pageIndex > other.pageIndex);
}

SideBarImageViewModel::SideBarImageViewModel(DocSheet *sheet, QObject *parent)
    : QAbstractListModel(parent)
    , m_parent(parent), m_sheet(sheet)
{
    qCDebug(appLog) << "SideBarImageViewModel created for document:" << (sheet ? sheet->filePath() : "null");
    connect(m_sheet, &DocSheet::sigPageModified, this, &SideBarImageViewModel::onUpdateImage);
}

void SideBarImageViewModel::resetData()
{
    qCDebug(appLog) << "Resetting model data";
    beginResetModel();
    m_pagelst.clear();
    endResetModel();
}

void SideBarImageViewModel::initModelLst(const QList<ImagePageInfo_t> &pagelst, bool sort)
{
    qCDebug(appLog) << "Initializing model with" << pagelst.size() << "items, sort:" << sort;
    beginResetModel();

    m_pagelst = pagelst;

    if (sort) {
        qCDebug(appLog) << "Sorting model data";
        std::sort(m_pagelst.begin(), m_pagelst.end());
    }

    endResetModel();
}

void SideBarImageViewModel::changeModelData(const QList<ImagePageInfo_t> &pagelst)
{
    qCDebug(appLog) << "Changing model data";
    m_pagelst = pagelst;
}

void SideBarImageViewModel::setBookMarkVisible(int index, bool visible, bool updateIndex)
{
    qCDebug(appLog) << "Setting book mark visible for index:" << index << "visible:" << visible << "updateIndex:" << updateIndex;
    m_cacheBookMarkMap.insert(index, visible);
    if (updateIndex) {
        qCDebug(appLog) << "Updating model data for index:" << index;
        const QList<QModelIndex> &modelIndexlst = getModelIndexForPageIndex(index);
        for (const QModelIndex &modelIndex : modelIndexlst)
            emit dataChanged(modelIndex, modelIndex);
    }
}

int SideBarImageViewModel::rowCount(const QModelIndex &) const
{
    // qCDebug(appLog) << "Getting row count:" << m_pagelst.size();
    return m_pagelst.size();
}

int SideBarImageViewModel::columnCount(const QModelIndex &) const
{
    // qCDebug(appLog) << "Getting column count";
    return 1;
}

QVariant SideBarImageViewModel::data(const QModelIndex &index, int role) const
{
    // qCDebug(appLog) << "Getting data for index:" << index.row() << "role:" << role;
    if (!index.isValid())
        return QVariant();

    int nRow = m_pagelst.at(index.row()).pageIndex;

    if (-1 == nRow)
        return QVariant();

    if (role == ImageinfoType_e::IMAGE_PIXMAP) {
        QPixmap pixmap = m_sheet->thumbnail(nRow);

        if (pixmap.isNull()) {
            // qCDebug(appLog) << "Pixmap is null, filling with white";
            //先填充空白
            QPixmap emptyPixmap(174, 174);
            emptyPixmap.fill(Qt::white);
            m_sheet->setThumbnail(nRow, emptyPixmap);

            //使用线程
            DocPageThumbnailTask task;
            task.sheet = m_sheet;
            task.index = nRow;
            task.model = const_cast<SideBarImageViewModel *>(this);
            PageRenderThread::appendTask(task);
        }

        return QVariant::fromValue(pixmap);
    } else if (role == ImageinfoType_e::IMAGE_BOOKMARK) {
        // qCDebug(appLog) << "Getting book mark for index:" << nRow;
        if (m_cacheBookMarkMap.contains(nRow)) {
            return QVariant::fromValue(m_cacheBookMarkMap.value(nRow));
        }
        return QVariant::fromValue(false);
    } else if (role == ImageinfoType_e::IMAGE_ROTATE) {
        // qCDebug(appLog) << "Getting rotate for index:" << nRow;
        return QVariant::fromValue(m_sheet->operation().rotation * 90);
    } else if (role == ImageinfoType_e::IMAGE_INDEX_TEXT) {
        // qCDebug(appLog) << "Getting index text for index:" << nRow;
        return QVariant::fromValue(tr("Page %1").arg(nRow + 1));
    } else if (role == ImageinfoType_e::IMAGE_CONTENT_TEXT) {
        // qCDebug(appLog) << "Getting content text for index:" << nRow;
        return QVariant::fromValue(m_pagelst.at(index.row()).strcontents);
    } else if (role == ImageinfoType_e::IMAGE_SEARCH_COUNT) {
        // qCDebug(appLog) << "Getting search count for index:" << nRow;
        return QVariant::fromValue(m_pagelst.at(index.row()).strSearchcount);
    } else if (role == Qt::AccessibleTextRole) {
        // qCDebug(appLog) << "Getting accessible text for index:" << nRow;
        return index.row();
    } else if (role == ImageinfoType_e::IMAGE_PAGE_SIZE) {
        // qCDebug(appLog) << "Getting page size for index:" << nRow;
        return QVariant::fromValue(m_sheet->pageSizeByIndex(nRow));
    }
    return QVariant();
}

bool SideBarImageViewModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    // qCDebug(appLog) << "Setting data for index:" << index.row() << "role:" << role;
    if (!index.isValid())
        return false;
    return QAbstractListModel::setData(index, data, role);
}

QList<QModelIndex> SideBarImageViewModel::getModelIndexForPageIndex(int pageIndex)
{
    qCDebug(appLog) << "Getting model index for page index:" << pageIndex;
    QList<QModelIndex> modelIndexlst;

    int pageSize = m_pagelst.size();

    for (int index = 0; index < pageSize; index++) {
        if (m_pagelst.at(index) == ImagePageInfo_t(pageIndex))
            modelIndexlst << this->index(index);
    }
    return modelIndexlst;
}

int SideBarImageViewModel::getPageIndexForModelIndex(int row)
{
    qCDebug(appLog) << "Getting page index for model index:" << row;
    if (row >= 0 && row < m_pagelst.size())
        return m_pagelst.at(row).pageIndex;
    return -1;
}

void SideBarImageViewModel::onUpdateImage(int index)
{
    qCDebug(appLog) << "Updating image for index:" << index;
    DocPageThumbnailTask task;
    task.sheet = m_sheet;
    task.index = index;
    task.model = const_cast<SideBarImageViewModel *>(this);
    PageRenderThread::appendTask(task);
    qCDebug(appLog) << "Updating image for index:" << index << "end";
}

void SideBarImageViewModel::insertPageIndex(int pageIndex)
{
    qCDebug(appLog) << "Inserting page index:" << pageIndex;
    if (!m_pagelst.contains(ImagePageInfo_t(pageIndex))) {
        qCDebug(appLog) << "Inserting page index:" << pageIndex;
        int iterIndex = 0;
        int rowCount = m_pagelst.size();
        for (iterIndex = 0; iterIndex < rowCount; iterIndex++) {
            if (pageIndex < m_pagelst.at(iterIndex).pageIndex)
                break;
        }
        ImagePageInfo_t tImageinfo(pageIndex);
        m_pagelst.insert(iterIndex, tImageinfo);
        beginInsertRows(this->index(iterIndex).parent(), iterIndex, iterIndex);
        endInsertRows();
    } else {
        qCWarning(appLog) << "Page index already exists:" << pageIndex;
    }
    qCDebug(appLog) << "Inserting page index:" << pageIndex << "end";
}

void SideBarImageViewModel::insertPageIndex(const ImagePageInfo_t &tImagePageInfo)
{
    int index = -1;
    qCDebug(appLog) << "Inserting page index";
    if (tImagePageInfo.annotation == nullptr) {
        qCDebug(appLog) << "Inserting page index without annotation";
        index = m_pagelst.indexOf(tImagePageInfo);
    } else {
        qCDebug(appLog) << "Inserting page index with annotation";
        index = findItemForAnno(tImagePageInfo.annotation);
    }

    if (index == -1) {
        qCDebug(appLog) << "Inserting page index not found";
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
        qCDebug(appLog) << "Inserting page index found";
        m_pagelst[index].strcontents = tImagePageInfo.strcontents;
        emit dataChanged(this->index(index), this->index(index));
    }
    qCDebug(appLog) << "Inserting page index:" << tImagePageInfo.pageIndex << "end";
}

void SideBarImageViewModel::removePageIndex(int pageIndex)
{
    qCDebug(appLog) << "Removing page index:" << pageIndex;
    if (m_pagelst.contains(ImagePageInfo_t(pageIndex))) {
        qCDebug(appLog) << "Removing page index:" << pageIndex;
        beginResetModel();
        m_pagelst.removeAll(ImagePageInfo_t(pageIndex));
        endResetModel();
    } else {
        qCWarning(appLog) << "Page index not found:" << pageIndex;
    }
    qCDebug(appLog) << "Removing page index:" << pageIndex << "end";
}

void SideBarImageViewModel::removeItemForAnno(deepin_reader::Annotation *annotation)
{
    qCDebug(appLog) << "Removing item for annotation:" << annotation;
    int index = findItemForAnno(annotation);
    if (index >= 0) {
        qCDebug(appLog) << "Removing item for annotation:" << annotation << "found";
        beginResetModel();
        m_pagelst.removeAt(index);
        endResetModel();
    }
    qCDebug(appLog) << "Removing item for annotation:" << annotation << "end";
}

void SideBarImageViewModel::getModelIndexImageInfo(int modelIndex, ImagePageInfo_t &tImagePageInfo)
{
    qCDebug(appLog) << "Getting model index image info for model index:" << modelIndex;
    if (modelIndex >= 0 && modelIndex < m_pagelst.size()) {
        tImagePageInfo = m_pagelst.at(modelIndex);
    }
    qCDebug(appLog) << "Getting model index image info for model index:" << modelIndex << "end";
}

int SideBarImageViewModel::findItemForAnno(deepin_reader::Annotation *annotation)
{
    qCDebug(appLog) << "Finding item for annotation:" << annotation;
    int count = m_pagelst.size();
    for (int index = 0; index < count; index++) {
        if (annotation == m_pagelst.at(index).annotation) {
            return index;
        }
    }
    qCDebug(appLog) << "Finding item for annotation:" << annotation << "end";
    return -1;
}

void SideBarImageViewModel::handleRenderThumbnail(int index, QPixmap pixmap)
{
    qCDebug(appLog) << "Handling thumbnail render for page:" << index << "size:" << pixmap.size();
    pixmap.setDevicePixelRatio(dApp->devicePixelRatio());
    m_sheet->setThumbnail(index, pixmap);

    const QList<QModelIndex> &modelIndexlst = getModelIndexForPageIndex(index);
    qCDebug(appLog) << "Notifying" << modelIndexlst.size() << "views of data change";
    for (const QModelIndex &modelIndex : modelIndexlst)
        emit dataChanged(modelIndex, modelIndex);
}
