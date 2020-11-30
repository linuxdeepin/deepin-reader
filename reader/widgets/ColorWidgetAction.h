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
#ifndef COLORWIDGETACTION_H
#define COLORWIDGETACTION_H

#include <DWidget>

#include <QWidgetAction>

DWIDGET_USE_NAMESPACE

/**
 * @brief The ColorWidgetAction class
 * 颜色选项框Aciton
 */
class ColorWidgetAction : public QWidgetAction
{
    Q_OBJECT
    Q_DISABLE_COPY(ColorWidgetAction)

public:
    explicit ColorWidgetAction(DWidget *pParent = nullptr);

signals:
    /**
     * @brief sigBtnGroupClicked
     * 颜色按钮点击信号
     */
    void sigBtnGroupClicked();

private slots:
    /**
     * @brief slotBtnClicked
     * 响应颜色按钮点击
     */
    void slotBtnClicked(int);

private:
    /**
     * @brief initWidget
     * 控件初始化
     * @param pParent
     */
    void initWidget(DWidget *pParent);
};

#endif  // COLORWIDGETACTION_H
