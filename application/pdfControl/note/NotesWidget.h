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
#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DIconButton>
#include <DPushButton>

#include <QThread>
#include <QPointer>

#include "../CustomListWidget.h"
#include "CustomControl/CustomWidget.h"
#include "docview/commonstruct.h"
#include "NotesItemWidget.h"

class NotesWidget;
class DocummentProxy;
class ThreadLoadImageOfNote : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(ThreadLoadImageOfNote)

public:
    explicit ThreadLoadImageOfNote(QObject *parent = nullptr);

    ~ThreadLoadImageOfNote() override
    {
        stopThreadRun();
    }

public:
    void stopThreadRun();

    inline void setIsLoaded(const bool &load)
    {
        m_isLoaded = load;
    }

    inline bool isLoaded()
    {
        return m_isLoaded;
    }

    inline void setListNoteSt(const QList<stHighlightContent> &list)
    {
        m_stListNote = list;
    }
    inline void setParentWidget(NotesWidget *thumbnail)
    {
        m_pNoteWidget = thumbnail;
    }
    void setProxy(DocummentProxy *proxy )
    {
        m_proxy = proxy;
    }

signals:
    void sigLoadImage(const QImage &);

protected:
    void run() override;

private:
    bool m_isLoaded = false;                 // 是都加载完毕
    QList<stHighlightContent> m_stListNote;  // 新文件的注释列表
    NotesWidget *m_pNoteWidget = nullptr;
    DocummentProxy *m_proxy = nullptr;
};

/**
 * @brief The ThumbnailItemWidget class
 * @brief   注释窗体
 */

class NotesWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NotesWidget)

public:
    explicit NotesWidget(DocSheet *sheet, DWidget *parent = nullptr);

    ~NotesWidget() override;

    void prevPage();

    void nextPage();

    void DeleteItemByKey();

    void handleOpenSuccess();

signals:
    void sigDeleteContent(const int &, const QString &);

    void sigUpdateThumbnail(const int &page);

public:
    QString getBindPath() const;

protected:
    void initWidget() override;

private slots:
    void slotLoadImage(const QImage &);

    void slotListMenuClick(const int &);

    void slotSelectItem(QListWidgetItem *);

    void SlotRightDeleteItem();

    void slotAddAnnotation();

    void SlotAnntationMsg(const int &, const QString &);

private:
    void CopyNoteContent();
    void __AddNoteItem(const QString &, const int &iType = NOTE_HIGHLIGHT);
    void __DeleteNoteItem(const QString &);
    void __UpdateNoteItem(const QString &);

    void __JumpToPrevItem();
    void __JumpToNextItem();

    void addNotesItem(const QString &text, const int &iType);
    void initConnection();
    void setSelectItemBackColor(QListWidgetItem *);
    void clearItemColor();

    void fillContantToList();
    QListWidgetItem *addNewItem(const QImage &image, const int &page, const QString &uuid, const QString &text,
                                const bool &bNew = false, const int &iType = NOTE_HIGHLIGHT);
    NotesItemWidget *getItemWidget(QListWidgetItem *);
private:
    CustomListWidget *m_pNotesList = nullptr;
    ThreadLoadImageOfNote m_ThreadLoadImage;  // 加载注释缩略图线程
    DPushButton *m_pAddAnnotationBtn = nullptr ;   // 添加注释

    int m_nIndex = -1;                        // 当前注释列表数
    QString m_strBindPath = "";
    bool m_bOpenFileOk = false;               //是否刚打开文件

    QPointer<DocSheet> m_sheet;
    // CustomWidget interface
public:
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;
    void updateThumbnail(const int &) Q_DECL_OVERRIDE;
};

#endif  // NOTESFORM_H
