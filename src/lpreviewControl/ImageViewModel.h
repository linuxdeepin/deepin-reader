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
#ifndef IMAGEVIEWMODEL_H
#define IMAGEVIEWMODEL_H

#include <QAbstractListModel>
#include <QMap>

#include "pdfControl/docview/CommonStruct.h"

typedef enum ImageinfoType_e {
    IMAGE_PIXMAP       = Qt::UserRole,
    IMAGE_BOOKMARK     = Qt::UserRole + 1,
    IMAGE_ROTATE       = Qt::UserRole + 2,
    IMAGE_INDEX_TEXT   = Qt::UserRole + 3,
    IMAGE_CONTENT_TEXT = Qt::UserRole + 4,
    IMAGE_SEARCH_COUNT = Qt::UserRole + 5,
} ImageinfoType_e;

typedef struct ImagePageInfo_t {
    int pageIndex;
    int iType;

    QString struuid;
    QString strcontents;
    QString strSearchcount;

    ImagePageInfo_t()
    {
        pageIndex = -1;
        iType = -1;
        struuid = "";
        strcontents = "";
        strSearchcount = "";
    }

    ImagePageInfo_t(int index)
    {
        this->pageIndex = index;
        this->iType = -1;
    }

    bool operator == (const ImagePageInfo_t &other) const
    {
        return (this->pageIndex == other.pageIndex);
    }

    bool operator < (const ImagePageInfo_t &other) const
    {
        return (this->pageIndex < other.pageIndex);
    }

    bool operator > (const ImagePageInfo_t &other) const
    {
        return (this->pageIndex > other.pageIndex);
    }
} ImagePageInfo_t;
Q_DECLARE_METATYPE(ImagePageInfo_t);

class DocSheet;
class ImageViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ImageViewModel(QObject *parent = nullptr);

public:
    void resetData();
    void initModelLst(const QList<ImagePageInfo_t> &pagelst, bool sort = false);
    void setDocSheet(DocSheet *sheet);
    void setBookMarkVisible(int pageIndex, bool visible, bool updateIndex = true);
    void updatePageIndex(int pageIndex);
    void insertPageIndex(int pageIndex);
    void insertPageIndex(const ImagePageInfo_t &tImagePageInfo);
    void removePageIndex(int pageIndex);
    void removeItemForuuid(const QString &uuid);

    QList<QModelIndex> getModelIndexForPageIndex(int pageIndex);
    void getModelIndexImageInfo(int modelIndex, ImagePageInfo_t &tImagePageInfo);
    int getPageIndexForModelIndex(int row);
    int findItemForuuid(const QString &uuid);

public slots:
    void onUpdatePageImage(int pageIndex);
    void onFetchImage(int nRow) const;

protected:
    int columnCount(const QModelIndex &) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &data, int role) override;

private:
    QObject *m_parent;
    DocSheet *m_docSheet;
    QList<ImagePageInfo_t> m_pagelst;
    QMap<int, bool> m_cacheBookMarkMap;
};

#endif // IMAGEVIEWMODEL_H
