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
#ifndef IMAGEVIEWMODEL_H
#define IMAGEVIEWMODEL_H

#include <QAbstractListModel>
#include <QMap>

namespace deepin_reader {
class Annotation;
}

typedef enum E_SideBar {
    SIDE_THUMBNIL = 0,
    SIDE_BOOKMARK,
    SIDE_CATALOG,
    SIDE_NOTE,
    SIDE_SEARCH
} E_SideBar;

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

    QString strcontents;
    QString strSearchcount;

    deepin_reader::Annotation *annotation;

    ImagePageInfo_t()
    {
        pageIndex = -1;
        strcontents = "";
        strSearchcount = "";
        annotation = nullptr;
    }

    ImagePageInfo_t(int index)
    {
        this->pageIndex = index;
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
    void updatePageIndex(int pageIndex, bool force = false);
    void insertPageIndex(int pageIndex);
    void insertPageIndex(const ImagePageInfo_t &tImagePageInfo);
    void removePageIndex(int pageIndex);
    void removeItemForAnno(deepin_reader::Annotation *annotation);
    void changeModelData(const QList<ImagePageInfo_t> &pagelst);

    QList<QModelIndex> getModelIndexForPageIndex(int pageIndex);
    void getModelIndexImageInfo(int modelIndex, ImagePageInfo_t &tImagePageInfo);
    int getPageIndexForModelIndex(int row);
    int findItemForAnno(deepin_reader::Annotation *annotation);

public slots:
    void onUpdatePageImage(int pageIndex);
    void onFetchImage(int nRow, bool force = false) const;

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
