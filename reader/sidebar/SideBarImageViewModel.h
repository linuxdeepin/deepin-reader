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
#ifndef SIDEBARIMAGEVIEWMODEL_H
#define SIDEBARIMAGEVIEWMODEL_H

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
    IMAGE_PAGE_SIZE    = Qt::UserRole + 6,
} ImageinfoType_e;

typedef struct ImagePageInfo_t {
    int pageIndex;

    QString strcontents;
    QString strSearchcount;
    deepin_reader::Annotation *annotation = nullptr;

    ImagePageInfo_t(int index = -1): pageIndex(index)
    {

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
/**
 * @brief The SideBarImageViewModel class
 * ImageVIew Model
 */
class SideBarImageViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit  SideBarImageViewModel(QObject *parent = nullptr);

public:
    /**
     * @brief resetData
     * 清空数据
     */
    void resetData();

    /**
     * @brief initModelLst
     * 初始化MODEL数据
     * @param pagelst
     * @param sort
     */
    void initModelLst(const QList<ImagePageInfo_t> &pagelst, bool sort = false);

    /**
     * @brief setDocSheet
     * 设置文档操作对象
     * @param sheet
     */
    void setDocSheet(DocSheet *sheet);

    /**
     * @brief setBookMarkVisible
     * 设置书签数据缓存状态
     * @param pageIndex
     * @param visible
     * @param updateIndex
     */
    void setBookMarkVisible(int pageIndex, bool visible, bool updateIndex = true);

    /**
     * @brief 更新指定页数
     * @param pageIndex 页数
     * @param force 是否强制刷新
     */
    void updatePageIndex(int pageIndex, bool force = false, bool bSrc = false);

    /**
     * @brief insertPageIndex
     * 插入指定页码数据
     * @param pageIndex
     */
    void insertPageIndex(int pageIndex);

    /**
     * @brief insertPageIndex
     * 插入缩略图数据
     * @param tImagePageInfo
     */
    void insertPageIndex(const ImagePageInfo_t &tImagePageInfo);

    /**
     * @brief removePageIndex
     * 删除指定页码数据
     * @param pageIndex
     */
    void removePageIndex(int pageIndex);

    /**
     * @brief removeItemForAnno
     * 删除指定注释数据
     * @param annotation
     */
    void removeItemForAnno(deepin_reader::Annotation *annotation);

    /**
     * @brief changeModelData
     * 重置model数据
     * @param pagelst
     */
    void changeModelData(const QList<ImagePageInfo_t> &pagelst);

    /**
     * @brief getModelIndexForPageIndex
     * 根据页码获取对应MODEL INDEX
     * @param pageIndex
     * @return
     */
    QList<QModelIndex> getModelIndexForPageIndex(int pageIndex);

    /**
     * @brief getModelIndexImageInfo
     * 根据指定页码,获取ImagePageInfo_t数据
     * @param modelIndex
     * @param tImagePageInfo
     */
    void getModelIndexImageInfo(int modelIndex, ImagePageInfo_t &tImagePageInfo);

    /**
     * @brief getPageIndexForModelIndex
     * 根据节点索引值,获取对应页码
     * @param row
     * @return
     */
    int getPageIndexForModelIndex(int row);

    /**
     * @brief findItemForAnno
     * 通过注释对象获取对应节点索引值
     * @param annotation
     * @return
     */
    int findItemForAnno(deepin_reader::Annotation *annotation);

public slots:
    /**
     * @brief onUpdatePageImage
     * 通知指定页码数据变更
     * @param pageIndex
     */
    void onUpdatePageImage(int pageIndex);

    /**
     * @brief onFetchImage
     * 拉取指定页码数据
     * @param nRow
     * @param force
     */
    void onFetchImage(int nRow, bool force = false) const;

protected:
    /**
     * @brief columnCount
     * view列数
     * @return
     */
    int columnCount(const QModelIndex &) const override;

    /**
     * @brief rowCount
     * view行数
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief data
     * view显示数据
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief setData
     * 设置model数据
     * @param index
     * @param data
     * @param role
     * @return
     */
    bool setData(const QModelIndex &index, const QVariant &data, int role) override;

private:
    QObject *m_parent = nullptr;
    DocSheet *m_docSheet = nullptr;
    QList<ImagePageInfo_t> m_pagelst;
    QMap<int, bool> m_cacheBookMarkMap;
};

#endif // SIDEBARIMAGEVIEWMODEL_H
