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
#ifndef FVMMOUSEEVENT_H
#define FVMMOUSEEVENT_H

#include <QObject>
#include <DWidget>

#include "docview/pagebase.h"
#include "subjectObserver/Singleton.h"

class FileViewWidget;
class DocummentProxy;

DWIDGET_USE_NAMESPACE

/**
 * @brief The MouseEventController class
 *  鼠标事件处理
 */

class FVMMouseEvent : public QObject
{
    Q_OBJECT
public:
    void mouseMoveEvent(QMouseEvent *event, DWidget *);
    void mousePressEvent(QMouseEvent *event, DWidget *);
    void mouseReleaseEvent(QMouseEvent *event, DWidget *);

private:
    void __FilePageMove(FileViewWidget *, const QPoint &);
    void __ShowMagnifier(const QPoint &);
    void __MouseSelectText(FileViewWidget *fvw, const QPoint &);
    void __ShowPageNoteWidget(FileViewWidget *fvw, const QPoint &);
    void __ShowFileNoteWidget(FileViewWidget *fvw, const QPoint &);
    void __CloseFileNoteWidget(FileViewWidget *fvw);
    void __OtherMouseMove(FileViewWidget *fvw, const QPoint &);

private:
    void __AddIconAnnotation(FileViewWidget *fvw, const QPoint &);
    void __HandlClicked(const QPoint &);
    void __ClickPageLink(Page::Link *pLink, FileViewWidget *);
    void __OtherMousePress(FileViewWidget *fvw, const QPoint &);

    void __ShowNoteTipWidget(FileViewWidget *fvw, const QString &sText);

private:
    bool        m_bSelectOrMove = false;                //  是否可以选中文字、移动
    bool        m_bIsHandleSelect = false;              //  是否可以选中
    QPoint      m_pMoveStartPoint;

};

typedef CSingleton<FVMMouseEvent> g_FVMMouseEvent;

#endif // FILEVIEWMOUSEEVENT_H
