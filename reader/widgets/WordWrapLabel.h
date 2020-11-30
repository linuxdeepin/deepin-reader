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
#ifndef WORDWRAPLABEL_H
#define WORDWRAPLABEL_H

#include <DLabel>

DWIDGET_USE_NAMESPACE
/**
 * @brief The WordWrapLabel class
 * 自动换行Label
 */
class WordWrapLabel : public DLabel
{
public:
    explicit WordWrapLabel(QWidget *parent = nullptr);

    /**
     * @brief setText
     * 设置内容
     * @param text
     */
    void setText(const QString &text);

    /**
     * @brief setMargin
     * 设置间距
     * @param margin
     */
    void setMargin(int margin);

private:
    /**
     * @brief adjustContent
     * 自适应内容
     */
    void adjustContent();

protected:
    /**
     * @brief paintEvent
     * 绘制事件
     * @param event
     */
    void paintEvent(QPaintEvent *event);

private:
    QString m_text;
    int m_margin;
};

#endif // WORDWRAPLABEL_H
