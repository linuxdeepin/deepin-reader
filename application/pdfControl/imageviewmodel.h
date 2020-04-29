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

typedef enum ImageinfoType_e{
    IMAGE_PIXMAP    = Qt::UserRole,
    IMAGE_BOOKMARK  = Qt::UserRole + 1,
    IMAGE_ROTATE    = Qt::UserRole + 2,
}ImageinfoType_e;

class DocSheet;
class ImageViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ImageViewModel(QObject *parent = nullptr);

public:
    void setPageCount(int pagecount);
    void setDocSheet(DocSheet *sheet);
    void setBookMarkVisible(int pageIndex, bool visible, bool updateIndex = true);
    void updatePageIndex(int pageIndex);

public slots:
    void onUpdatePageImage(int pageIndex);
    void onFetchImage(int nRow) const;

protected:
    int columnCount(const QModelIndex &) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &data, int role) override;

private:
    int m_pageCount;
    QObject *m_parent;
    DocSheet *m_docSheet;
    QMap<int, bool> m_cacheBookMarkMap;
};

#endif // IMAGEVIEWMODEL_H
