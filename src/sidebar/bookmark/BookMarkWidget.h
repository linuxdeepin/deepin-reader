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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include "widgets/CustomWidget.h"

#include <QPointer>

class DocSheet;
class ImageListView;

/**
 * @brief The BookMarkWidget class
 * 书签目录控件类
 */
class BookMarkWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BookMarkWidget)

public:
    explicit BookMarkWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~BookMarkWidget() override;

    /**
     * @brief prevPage
     * 上一页
     */
    void prevPage();

    /**
     * @brief pageUp
     * 上翻一页
     */
    void pageUp();

    /**
     * @brief nextPage
     * 下一页
     */
    void nextPage();

    /**
     * @brief pageDown
     * 下翻一页
     */
    void pageDown();

    /**
     * @brief deleteItemByKey
     * 删除当前节点
     */
    void deleteItemByKey();

    /**
     * @brief handlePage
     * 跳转指定页节点
     * @param index
     */
    void handlePage(int index);

    /**
     * @brief handleOpenSuccess
     * 数据初始化
     */
    void handleOpenSuccess();

    /**
     * @brief handleBookMark
     * 设置书签节点状态
     * @param index
     * @param state
     */
    void handleBookMark(int index, int state);

    /**
     * @brief addBtnCheckEnter
     * 触发添加书签操作
     * @return
     */
    bool addBtnCheckEnter();

    /**
     * @brief setTabOrderWidget
     * 设置TAB切换顺序控件
     * @param tabWidgetlst
     */
    void setTabOrderWidget(QList<QWidget *> &tabWidgetlst);

public:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget() Q_DECL_OVERRIDE;

    /**
     * @brief adaptWindowSize
     * 调整控件大小
     */
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;

    /**
     * @brief updateThumbnail
     * 刷新指定节点
     */
    void updateThumbnail(const int &) Q_DECL_OVERRIDE;

    /**
     * @brief showMenu
     * 显示菜单
     */
    void showMenu() Q_DECL_OVERRIDE;

private slots:
    /**
     * @brief onUpdateTheme
     * 响应主题变更刷新
     */
    void onUpdateTheme();

    /**
     * @brief onAddBookMarkClicked
     * 响应添加书签按钮
     */
    void onAddBookMarkClicked();

    /**
     * @brief onListMenuClick
     * 响应菜单点击
     * @param iType
     */
    void onListMenuClick(const int &iType);

private:
    /**
     * @brief deleteAllItem
     * 删除所有书签节点
     */
    void deleteAllItem();

private:
    QPointer<DocSheet> m_sheet;
    DPushButton       *m_pAddBookMarkBtn = nullptr;
    ImageListView     *m_pImageListView = nullptr;
};

#endif  // BOOKMARKFORM_H
