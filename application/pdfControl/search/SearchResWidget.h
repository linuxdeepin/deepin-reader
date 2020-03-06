/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef SEARCHRESWIDGET_H
#define SEARCHRESWIDGET_H

#include "CustomControl/CustomWidget.h"
#include "../CustomListWidget.h"
#include "docview/commonstruct.h"

/**
 * @brief The LoadSearchResThread class
 * @brief   加载搜索结果缩略图线程
 */


/**
 * @brief The SearchResWidget class
 * 搜索结果界面
 */
class SearchResWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchResWidget)

public:
    explicit SearchResWidget(DWidget *parent = nullptr);
    ~SearchResWidget() override;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
    void OnExitSearch();

protected:
    void initWidget() override;

private slots:
//    void slotFlushSearchWidget(const QString &);
    void slotGetSearchContant(const stSearchRes &);
    void slotSearchOver();
    void slotSelectItem(QListWidgetItem *);

private:
    void OnOpenFileOk(const QString &);
    void initConnections();
    void addSearchsItem(const int &page, const QString &text, const int &resultNum);
    void showTips();
    void setSelectItemBackColor(QListWidgetItem *);
    void clearItemColor();

private:
    CustomListWidget *m_pSearchList = nullptr;       // 搜索结果列表
};

#endif  // NOTESFORM_H
