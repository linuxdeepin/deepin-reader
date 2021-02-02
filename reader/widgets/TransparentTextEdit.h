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
#ifndef TRANSPARENTTEXTEDIT_H
#define TRANSPARENTTEXTEDIT_H

#include <QTextEdit>

#include <DWidget>

#include <QTimer>
#include <QContextMenuEvent>

DWIDGET_USE_NAMESPACE

/**
 * @brief The TransparentTextEdit class
 * 文本编辑控件
 */
class TransparentTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TransparentTextEdit(DWidget *parent = nullptr);

signals:
    /**
     * @brief sigNeedShowTips
     * 提示信号
     * @param tips
     */
    void sigNeedShowTips(const QString &tips, int);

private slots:
    /**
     * @brief slotTextEditMaxContantNum
     * 检测编辑框是否达到字数上限
     */
    void slotTextEditMaxContantNum();

protected:
    /**
     * @brief paintEvent
     * 绘制事件
     * @param event
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief insertFromMimeData
     * 重载剪贴板插入函数
     * @param source
     */
    void insertFromMimeData(const QMimeData *source) override;

    /**
     * @brief keyPressEvent
     * 键盘按键事件
     * @param event
     */
    void keyPressEvent(QKeyEvent *event) override;

private:
    int m_nMaxContantLen = 1500;  // 允许输入文本最大长度
};

#endif // TRANSPARENTTEXTEDIT_H
