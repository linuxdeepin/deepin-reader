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
#ifndef CUSTOMCONTROL_ROUNDCOLORWIDGET_H
#define CUSTOMCONTROL_ROUNDCOLORWIDGET_H

#include <DWidget>

DWIDGET_USE_NAMESPACE

/**
 * @brief The RoundColorWidget class
 * 颜色选项框控件
 */
class RoundColorWidget : public DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(RoundColorWidget)

public:
    explicit RoundColorWidget(const QColor &color, QWidget *parent = nullptr);

    /**
     * @brief setSelected
     * 设置是否选中
     * @param selected
     */
    void setSelected(bool selected);

    /**
     * @brief setAllClickNotify
     * 点击是否发送点击信号
     * @param bnotify
     */
    inline void setAllClickNotify(bool bnotify) {m_allnotify = bnotify;}

Q_SIGNALS:
    /**
     * @brief clicked
     * 点击信号
     */
    void clicked();

protected:
    /**
     * @brief mousePressEvent
     * 鼠标点击事件
     * @param event
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief paintEvent
     * 绘制事件
     * @param event
     */
    void paintEvent(QPaintEvent *event);

private:
    bool m_isSelected = false;
    QColor m_color;
    bool m_allnotify = false;
};

#endif // CUSTOMCONTROL_ROUNDCOLORWIDGET_H
