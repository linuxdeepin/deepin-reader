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
#ifndef PROXYMOUSEMOVE_H
#define PROXYMOUSEMOVE_H

#include <QObject>

#include <QMouseEvent>

#include "docview/pagebase.h"

class SheetBrowserPDFPrivate;

class ProxyMouseMove : public QObject
{
    Q_OBJECT
public:
    explicit ProxyMouseMove(QObject *parent = nullptr);

private:
    void mouseMoveEvent(QMouseEvent *event);

    void __ShowMagnifier(const QPoint &clickPoint);
    void __FilePageMove(const QPoint &endPos);
    void __MouseSelectText(const QPoint &clickPoint);
    void __OtherMouseMove(const QPoint &globalPos);
    void __ShowPageNoteWidget(const QPoint &docPos);
    void __ShowFileNoteWidget(const QPoint &docPos);

private:
    void mousePressEvent(QMouseEvent *event);
    void __AddIconAnnotation(const QPoint &globalPos);
    void __HandlClicked(const QPoint &globalPos);
    void __OtherMousePress(const QPoint &globalPos);
    void __ClickPageLink(Page::Link *pLink);

private:
    void mouseReleaseEvent(QMouseEvent *event);
    bool sameHighLight() const
    {
        return m_bSameHighLight;
    }
private:
    void notifyMsg(const int &, const QString &);

private:
    SheetBrowserPDFPrivate   *_fvwParent = nullptr;

    bool            m_bSelectOrMove = false;
    QPoint          m_pMoveStartPoint;
    QString m_strUUid{""};                                   //  鼠标左键点击高亮文字的UUid
    bool m_bSameHighLight{false};                            //  鼠标选择的一段文字是否是同一个高亮
    friend class SheetBrowserPDFPrivate;
};

#endif // PROXYMOUSEMOVE_H
