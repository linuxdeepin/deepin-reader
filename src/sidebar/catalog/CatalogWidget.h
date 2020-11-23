/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#ifndef PDFCONTROL_CATALOGWIDGET_H
#define PDFCONTROL_CATALOGWIDGET_H

#include "BaseWidget.h"

#include <DLabel>

class CatalogTreeView;
/**
 * @brief The CatalogWidget class
 * @brief       目录 界面
 */

class DocSheet;
class CatalogWidget : public BaseWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CatalogWidget)

public:
    explicit CatalogWidget(DocSheet *sheet, DWidget *parent = nullptr);

    ~CatalogWidget() override;

    /**
     * @brief handleOpenSuccess
     * 文档打开成功,初始化数据
     */
    void handleOpenSuccess();

    /**
     * @brief handlePage
     * 选中指定页
     * @param page
     */
    void handlePage(int page);

    /**
     * @brief nextPage
     * 下一页
     */
    void nextPage();

    /**
     * @brief pageDown
     * 上一翻页
     */
    void pageDown();

    /**
     * @brief prevPage
     * 上一页
     */
    void prevPage();

    /**
     * @brief pageUp
     * 下一翻页
     */
    void pageUp();

private:
    /**
     * @brief setTitleTheme
     * 设置标题名称
     */
    void setTitleTheme();

protected:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget();

protected:
    /**
     * @brief resizeEvent
     * resize事件
     * @param event
     */
    void resizeEvent(QResizeEvent *event) override;

private:
    QString             m_strTheme = "";
    DLabel              *titleLabel = nullptr;
    CatalogTreeView     *m_pTree = nullptr;
    DocSheet *m_sheet;
};


#endif // CATALOGWIDGET_H
