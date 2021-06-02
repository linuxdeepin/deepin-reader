/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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

DWIDGET_USE_NAMESPACE

class TransparentTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TransparentTextEdit(DWidget *parent = nullptr);

signals:
    void sigNeedShowTips(const QString &tips, int);

    /**
     * @brief sigCloseNoteWidget 关闭注释编辑框窗口
     * @param isEsc 是否Esc键触发注释编辑框窗关闭
     */
    void sigCloseNoteWidget(bool isEsc = false);

private:
    void init();

private slots:
    void slotTextEditMaxContantNum();

protected:
    /**
     * @brief keyPressEvent
     * 键盘按键事件
     * @param event
     */
    void keyPressEvent(QKeyEvent *event) override;

    void focusOutEvent(QFocusEvent *event) override;

private:
    int m_nMaxContantLen = 1500;  // 允许输入文本最大长度
};

#endif // TRANSPARENTTEXTEDIT_H
