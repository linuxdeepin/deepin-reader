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
#ifndef SEARCHRESWIDGET_H
#define SEARCHRESWIDGET_H

#include "widgets/CustomWidget.h"

namespace deepin_reader {
struct SearchResult;
}
class DocSheet;
class ImageListView;
class QStackedLayout;

/**
 * @brief The SearchResWidget class
 * 搜索目录控件
 */
class SearchResWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchResWidget)

public:
    explicit SearchResWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~SearchResWidget() override;

    /**
     * @brief clearFindResult
     * 清空搜索目录结果
     */
    void clearFindResult();

    /**
     * @brief handleFindFinished
     * 搜索完毕
     * @return 搜索结果个数
     */
    int  handleFindFinished();

    /**
     * @brief handFindContentComming
     *
     */
    void handFindContentComming(const deepin_reader::SearchResult &);

public:
    /**
     * @brief adaptWindowSize
     * 控件大小变化响应
     */
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;

    /**
     * @brief updateThumbnail
     * 刷新指定页数
     */
    void updateThumbnail(const int &) Q_DECL_OVERRIDE;

protected:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget() Q_DECL_OVERRIDE;

private:
    /**
     * @brief addSearchsItem
     * 添加搜索节点
     * @param pageIndex 页数
     * @param text 文本内容
     * @param resultNum 搜索个数
     */
    void addSearchsItem(const int &pageIndex, const QString &text, const int &resultNum);

private:
    DocSheet *m_sheet;
    QStackedLayout *m_stackLayout;
    ImageListView *m_pImageListView;
};

#endif  // NOTESFORM_H
