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
#include <DIconButton>

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
     * @brief setEditAlert
     * 搜索编辑框提示
     * @param iFlag
     */
    void setEditAlert(const int &iFlag);

private slots:
    /**
     * @brief onCloseBtnClicked
     * 取消搜索并关闭
     */
    void onCloseBtnClicked();

    /**
     * @brief onSearchStop
     * 取消搜索
     */
    void onSearchStop();

    /**
     * @brief onSearchStart
     * 开始搜索
     */
    void onSearchStart();

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
     * @brief onTextChanged
     * 文本内容变化，如果空的时候需要设置提示
     */
    void onTextChanged();

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
    QString m_lastSearchText; //上一次查找内容
    DIconButton *m_findPrevButton = nullptr;
    DIconButton *m_findNextButton = nullptr;
};

#endif
