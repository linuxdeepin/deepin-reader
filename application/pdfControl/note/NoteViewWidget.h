/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef NOTEVIEWWIDGET_H
#define NOTEVIEWWIDGET_H

#include "CustomControl/CustomWidget.h"
#include "MsgHeader.h"
#include "ModuleHeader.h"

class CustomClickLabel;
class TransparentTextEdit;

/**
 *@brief The NoteViewWidget class
 *@brief 添加注释子界面
 */
class NoteViewWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NoteViewWidget)

public:
    explicit NoteViewWidget(DWidget *parent = nullptr);
    ~NoteViewWidget() override;

signals:
    void sigNeedAddHighLightAnnotation(const QString &msgContent);

    void sigNoteViewMsg(const int &, const QString &);

    void sigNeedShowTips(const QString &tips, bool index);

public:

public:
    void showWidget(const QPoint &);

    void setEditText(const QString &note);

    void setPointAndPage(const QString &);

    void setNoteUuid(const QString &pNoteUuid);

    void setNotePage(const QString &pNotePage);

    void setWidgetType(const int &nWidgetType);

protected:
    void hideEvent(QHideEvent *event) override;

    void initWidget() override;

private:
    void initConnections();

    void __FileNoteHideEvent();

    void __PageNoteHideEvent();

private slots:
    void slotUpdateTheme();

private:
    QString m_pNoteUuid = "";
    QString m_pNotePage = "";

    TransparentTextEdit *m_pTextEdit = nullptr;    // 注释
    CustomClickLabel    *m_pCloseLab = nullptr;  // 关闭
    QString m_strNote = "";                   // 注释内容
    int     m_nWidgetType;         //  高亮注释\页面注释
};

#endif  // NOTEVIEWWIDGET_H
