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
#ifndef NOTESITEMWIDGET_H
#define NOTESITEMWIDGET_H

#include "../CustomItemWidget.h"
#include "ModuleHeader.h"

/**
 * @brief The NotesItemWidget class
 * @brief   注释item
 */

class NotesItemWidget : public CustomItemWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NotesItemWidget)

public:
    explicit NotesItemWidget(DWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

public:
    void setTextEditText(const QString &);
    inline void setNoteUUid(const QString &uuid)
    {
        m_strUUid = uuid;
    }
    inline QString noteUUId() const
    {
        return m_strUUid;
    }
    inline QString note() const
    {
        return m_strNote;
    }

    inline void setNote(const QString &note)
    {
        m_strNote = note;
    }

    bool bSelect();
    void setBSelect(const bool &paint);

    int nNoteType() const;
    void setNNoteType(const int &nNoteType);

    QString strPage() const;
    void setStrPage(const QString &strPage);

    void CopyItemText();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void initWidget() override;
    void __InitConnections();

private slots:
    void slotUpdateTheme();

private:
    DLabel      *m_pTextLab = nullptr;
    QString     m_strUUid = "";     // 当前注释唯一标识
    QString     m_strNote = "";     // 注释内容
    QString     m_strPage = "";     // 注释页面
    int         m_nNoteType = NOTE_HIGHLIGHT;   // 注释类型, 0,高亮注释; 1,页面注释
    bool        m_bPaint = false;

    // CustomWidget interface
public:
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;
};

#endif // NOTESITEMWIDGET_H
