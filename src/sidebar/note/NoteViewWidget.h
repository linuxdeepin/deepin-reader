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

#include "widgets/CustomWidget.h"

class TransparentTextEdit;
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

    void sigNeedShowTips(const QString &tips, int index);

public:
    void setEditText(const QString &note);

    void setPointAndPage(const QString &);

    void setNoteUuid(const QString &pNoteUuid);

    void setNotePage(const QString &pNotePage);

protected:
    void initWidget() override;
    void hideEvent(QHideEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void fileNoteHideEvent();
    void pageNoteHideEvent();

private:
    QString m_pNoteUuid = "";
    QString m_pNotePage = "";

    DWidget *widget;
    TransparentTextEdit *m_pTextEdit = nullptr;    // 注释
    QString m_strNote = "";                   // 注释内容
};

class NoteShadowViewWidget : public DWidget
{
public:
    NoteShadowViewWidget(QWidget *parent);
    NoteViewWidget *getNoteViewWidget();

    void showWidget(const QPoint &);

private:
    void initWidget();

private:
    NoteViewWidget *m_noteViewWidget;
};

#endif  // NOTEVIEWWIDGET_H
