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
#include <QGestureEvent>

#include <math.h>

DWIDGET_USE_NAMESPACE

#define TAP_MOVE_DELAY 300

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

    bool event(QEvent *event) override;

    void mousePressEvent(QMouseEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *e) override;

    void mouseMoveEvent(QMouseEvent *e) override;

private:
    /**
     * @brief gestureEvent 手势事件
     * @param event
     * @return
     */
    bool gestureEvent(QGestureEvent *event);

    /**
     * @brief tapGestureTriggered 单击手势事件
     */
    void tapGestureTriggered(QTapGesture *);

    /**
     * @brief tapAndHoldGestureTriggered 单指长按事件
     */
    void tapAndHoldGestureTriggered(QTapAndHoldGesture *);

    /**
     * @brief slideGesture 单指滑动手势(通过原生触摸屏事件进行抽象模拟)
     * @param diff
     * add for single refers to the sliding
     */
    void slideGesture(qreal diff);

    /**
     * @brief onSelectionArea 滑动选中事件
     */
    void onSelectionArea();

private:
    //触摸屏手势动作
    enum GestureAction {
        GA_null, //无动作
        GA_tap, //点击
        GA_slide, //滑动
        GA_hold, //长按
    };

    int m_nMaxContantLen = 1500;  // 允许输入文本最大长度

    GestureAction m_gestureAction = GA_null; //手势动作 默认误动作

    qint64 m_tapBeginTime = 0; //开始点击的时间

    bool m_slideContinue {false}; //是否持续滑动

    //add for single refers to the sliding
//    FlashTween tween; //滑动惯性

//    qreal change = {0.0}; //滑动变化量
//    qreal duration = {0.0}; //滑动方向

    //鼠标事件的位置
    int m_start = 0; //开始时鼠标的位置
    int m_end = 0; //结束时鼠标的位置
    qreal m_stepSpeed = 0; //移动的速度

    int m_lastMousepos; //上次移动后鼠标的位置

    ulong m_lastMouseTime; //上次移动鼠标的时间

//    int m_nSelectEndLine; //< 选择结束时后鼠标所在行
    QPointF m_nSelectStart; //< 选择开始时的鼠标位置
//    QPointF m_nSelectEnd; //< 选择结束时的鼠标位置
};

#endif // TRANSPARENTTEXTEDIT_H
