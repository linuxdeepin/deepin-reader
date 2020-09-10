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
#ifndef TIPSWIDGET_H
#define TIPSWIDGET_H

#include <DWidget>
#include <QTimer>

DWIDGET_USE_NAMESPACE
/**
 * @brief The TipsWidget class
 * 悬浮提示框控件
 */
class TipsWidget : public DWidget
{
    Q_OBJECT
public:
    explicit TipsWidget(QWidget *parent = nullptr);

    /**
     * @brief setText
     * 设置提示框文本
     * @param text
     */
    void setText(const QString &text);

    /**
     * @brief setAlignment
     * 设置文本对齐方式
     */
    void setAlignment(Qt::Alignment);

    /**
     * @brief setLeftRightMargin
     * 设置左右边距
     * @param margin
     */
    void setLeftRightMargin(int margin);

    /**
     * @brief setTopBottomMargin
     * 设置上下边距
     * @param margin
     */
    void setTopBottomMargin(int margin);

    /**
     * @brief setMaxLineCount
     * 设置文本最大行数
     * @param maxLinecnt
     */
    void setMaxLineCount(int maxLinecnt);

    /**
     * @brief setAutoChecked
     * 设置是否自动检测关闭悬浮提示框
     * @param autoChecked
     */
    void setAutoChecked(bool autoChecked);

private:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget();

    /**
     * @brief adjustContent
     * 自适应文本内容
     * @param text
     */
    void adjustContent(const QString &text);

protected:
    /**
     * @brief paintEvent
     * 绘制事件
     * @param event
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief showEvent
     * 显示事件
     * @param event
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief hideEvent
     * 隐藏事件
     * @param event
     */
    void hideEvent(QHideEvent *event) override;

private slots:
    /**
     * @brief onUpdateTheme
     * 主题更新
     */
    void onUpdateTheme();

    /**
     * @brief onTimeOut
     * 自动检测超时
     */
    void onTimeOut();

private:
    QString m_text;
    int m_lrMargin;
    int m_tbMargin;
    int m_maxLineCount;
    Qt::Alignment m_alignment;

    QTimer m_timer;
    QWidget *m_parent;
    bool m_autoChecked = false;
};

#endif // TIPSWIDGET_H
