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
#ifndef SCALEWIDGET_H
#define SCALEWIDGET_H

#include "DocSheet.h"
#include "ScaleMenu.h"

#include <DWidget>
#include <DIconButton>
#include <DLineEdit>

#include <QPointer>

class DocSheet;
class ScaleMenu;
/**
 * @brief The ScaleWidget class
 * 缩放控件
 */
class ScaleWidget : public Dtk::Widget::DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ScaleWidget)

public:
    explicit ScaleWidget(Dtk::Widget::DWidget *parent = nullptr);

    ~ScaleWidget();

    /**
     * @brief setSheet
     * 设置文档对象
     * @param sheet
     */
    void setSheet(DocSheet *sheet);

    /**
     * @brief clear
     * 清空缩放比缩放框
     */
    void clear();

private:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget();

public slots:
    /**
     * @brief onPrevScale
     * 缩减25%缩放比
     */
    void onPrevScale();

    /**
     * @brief onNextScale
     * 缩增25%缩放比
     */
    void onNextScale();

private slots:
    /**
     * @brief onReturnPressed
     * 缩放比输入框确认响应
     */
    void onReturnPressed();

    /**
     * @brief onEditFinished
     * 输入框编辑完毕
     */
    void onEditFinished();

    /**
     * @brief onArrowBtnlicked
     * 缩放下拉菜单按钮点击
     */
    void onArrowBtnlicked();

private:
    QPointer<DocSheet> m_sheet;
    Dtk::Widget::DLineEdit *m_lineEdit = nullptr;
    ScaleMenu *m_ScaleMenu = nullptr;
};

#endif // SCALEWIDGET_H
