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
#ifndef IMAGELISTVIEW_H
#define IMAGELISTVIEW_H

#include <DListView>

DWIDGET_USE_NAMESPACE
struct ImagePageInfo_t;

class DocSheet;
class ImageViewModel;
class ImageViewDelegate;
class BookMarkMenu;
class NoteMenu;

/**
 * @brief The ImageListView class
 * 左侧栏缩略图目录VIEW
 */
class QMouseEvent;
class ImageListView : public DListView
{
    Q_OBJECT
public:
    ImageListView(DocSheet *sheet, QWidget *parent = nullptr);

signals:
    /**
     * @brief sigListMenuClick
     * 节点右键菜单点击
     */
    void sigListMenuClick(const int &);

    /**
     * @brief sigListItemClicked
     * 节点左键点击
     * @param row
     */
    void sigListItemClicked(int row);

public:
    /**
     * @brief handleOpenSuccess
     * 文档打开成功,初始化相关数据
     */
    void handleOpenSuccess();

    /**
     * @brief setListType
     * 设置ListView类型
     * @param type 缩略图,目录,书签,注释,搜索
     */
    void setListType(int type);

    /**
     * @brief scrollToIndex
     * 滚动到指定页数
     * @param pageIndex
     * @param scrollTo
     * @return
     */
    bool scrollToIndex(int pageIndex, bool scrollTo = true);

    /**
     * @brief scrollToModelInexPage
     * 滚动到指定节点
     * @param index
     * @param scrollto
     */
    void scrollToModelInexPage(const QModelIndex &index, bool scrollto = true);

    /**
     * @brief getModelIndexForPageIndex
     * 根据页数获取节点索引
     * @param pageIndex
     * @return
     */
    int  getModelIndexForPageIndex(int pageIndex);

    /**
     * @brief getPageIndexForModelIndex
     * 根据节点索引获取文档页码
     * @param row
     * @return
     */
    int  getPageIndexForModelIndex(int row);

    /**
     * @brief getImageModel
     * 获取model
     * @return
     */
    ImageViewModel *getImageModel();

    /**
     * @brief showMenu
     * 显示菜单
     */
    void showMenu();

    /**
     * @brief pageUpIndex
     * 上翻页
     * @return
     */
    QModelIndex pageUpIndex();

    /**
     * @brief pageDownIndex
     * 下翻页
     * @return
     */
    QModelIndex pageDownIndex();

private:
    /**
     * @brief initControl
     * 初始化控件
     */
    void initControl();

    /**
     * @brief showNoteMenu
     * 显示注释菜单
     */
    void showNoteMenu();

    /**
     * @brief showBookMarkMenu
     * 显示书签菜单
     */
    void showBookMarkMenu();

private slots:
    /**
     * @brief onUpdatePageImage
     * 更新指定页数缩略图
     * @param pageIndex
     */
    void onUpdatePageImage(int pageIndex);

    /**
     * @brief onItemClicked
     * 节点点击
     * @param index
     */
    void onItemClicked(const QModelIndex &index);

    /**
     * @brief onSetDocSlideGesture
     * 设置文档触摸屏滑动事件
     */
    void onSetThumbnailListSlideGesture();

    /**
     * @brief onRemoveDocSlideGesture
     * 移除文档触摸屏滑动事件
     */
    void onRemoveThumbnailListSlideGesture();

protected:
    /**
     * @brief mousePressEvent
     * 鼠标点击事件
     * @param event
     */
    void mousePressEvent(QMouseEvent *event);

private:
    int m_listType;
    DocSheet *m_docSheet;
    ImageViewModel *m_imageModel;

    BookMarkMenu *m_pBookMarkMenu;
    NoteMenu *m_pNoteMenu;
};

#endif // IMAGELISTVIEW_H
