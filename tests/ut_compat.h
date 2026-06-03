// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UT_COMPAT_H
#define UT_COMPAT_H

#include <QtGlobal>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QHoverEvent>

//
// Qt5 → Qt6 事件构造函数兼容层
//
// Qt6 中 QMouseEvent/QWheelEvent/QHoverEvent 的构造函数签名发生了变化。
// 本头文件提供统一的辅助函数，使测试代码在 Qt5 和 Qt6 下都能编译。
//

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

// Qt6: 需要 globalPos 参数
inline QMouseEvent* createMouseEvent(QEvent::Type type,
                                      const QPointF &localPos,
                                      Qt::MouseButton button,
                                      Qt::MouseButtons buttons,
                                      Qt::KeyboardModifiers modifiers)
{
    return new QMouseEvent(type, localPos, localPos, button, buttons, modifiers);
}

// Qt6: 需要 globalPos + pixelDelta + angleDelta
inline QWheelEvent* createWheelEvent(const QPointF &pos,
                                      int delta,
                                      Qt::MouseButtons buttons,
                                      Qt::KeyboardModifiers modifiers)
{
    return new QWheelEvent(pos, pos, QPoint(), QPoint(0, delta), buttons, modifiers,
                           Qt::NoScrollPhase, false);
}

// Qt6: 需要 scenePos + globalPos
inline QHoverEvent* createHoverEvent(QEvent::Type type,
                                      const QPointF &pos,
                                      const QPointF &oldPos)
{
    return new QHoverEvent(type, pos, pos, oldPos);
}

#else

// Qt5: 直接使用原始构造函数
inline QMouseEvent* createMouseEvent(QEvent::Type type,
                                      const QPointF &localPos,
                                      Qt::MouseButton button,
                                      Qt::MouseButtons buttons,
                                      Qt::KeyboardModifiers modifiers)
{
    return new QMouseEvent(type, localPos, button, buttons, modifiers);
}

inline QWheelEvent* createWheelEvent(const QPointF &pos,
                                      int delta,
                                      Qt::MouseButtons buttons,
                                      Qt::KeyboardModifiers modifiers)
{
    return new QWheelEvent(pos, delta, buttons, modifiers);
}

inline QHoverEvent* createHoverEvent(QEvent::Type type,
                                      const QPointF &pos,
                                      const QPointF &oldPos)
{
    return new QHoverEvent(type, pos, oldPos);
}

#endif // QT_VERSION_CHECK(6, 0, 0)

#endif // UT_COMPAT_H
