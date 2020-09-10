/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     duanxiaohui<duanxiaohui@uniontech.com>
*
* Maintainer: duanxiaohui<duanxiaohui@uniontech.com>
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
#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include "Application.h"

#include <DWidget>
#include <DFloatingWidget>
#include <DSearchEdit>

DWIDGET_USE_NAMESPACE

class DocSheet;
/**
 * @brief The FindWidget class
 * 搜索窗口
 */
class FindWidget : public DFloatingWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FindWidget)

public:
    explicit FindWidget(DWidget *parent = nullptr);

    ~FindWidget() override;

public:
    /**
     * @brief setDocSheet
     * 设置文档对象
     * @param sheet
     */
    void setDocSheet(DocSheet *sheet);

    /**
     * @brief showPosition
     * 设置显示位置
     */
    void showPosition(const int &);

    /**
     * @brief setSearchEditFocus
     * 设置搜索编辑框焦点
     */
    void setSearchEditFocus();

    /**
     * @brief stopSearch
     * 停止搜索
     */
    void stopSearch();

    /**
     * @brief setEditAlert
     * 搜索编辑框提示
     * @param iFlag
     */
    void setEditAlert(const int &iFlag);

private slots:
    /**
     * @brief findCancel
     * 取消搜索
     */
    void findCancel();

    /**
     * @brief handleContentChanged
     * 开始搜索
     */
    void handleContentChanged();

    /**
     * @brief slotFindNextBtnClicked
     * 查找下一个
     */
    void slotFindNextBtnClicked();

    /**
     * @brief slotFindPrevBtnClicked
     * 查找上一个
     */
    void slotFindPrevBtnClicked();

    /**
     * @brief slotEditAborted
     * 终止搜索
     */
    void slotEditAborted();

    /**
     * @brief slotClearContent
     * 清空搜索内容
     */
    void slotClearContent();

private:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget();

protected:
    /**
     * @brief keyPressEvent
     * 键盘按键事件
     * @param event
     */
    void keyPressEvent(QKeyEvent *event) override;

private:
    DocSheet *m_docSheet = nullptr;
    DSearchEdit *m_pSearchEdit = nullptr;
    QString m_strLastFindText = ""; //上一次查找内容
};

#endif
