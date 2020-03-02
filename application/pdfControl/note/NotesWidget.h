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

#include "../CustomListWidget.h"
#include "CustomControl/CustomWidget.h"
#include "docview/commonstruct.h"

class ThreadLoadImageOfNote : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(ThreadLoadImageOfNote)

public:
    explicit ThreadLoadImageOfNote(QObject *parent = nullptr);
    ~ThreadLoadImageOfNote() override { stopThreadRun(); }

public:
    void stopThreadRun();

    inline void setIsLoaded(const bool &load) { m_isLoaded = load; }

    inline bool isLoaded() { return m_isLoaded; }

    inline void setListNoteSt(const QList<stHighlightContent> &list) { m_stListNote = list; }

signals:
    void sigLoadImage(const QImage &);

protected:
    void run() override;

private:
    bool m_isLoaded = false;                 // 是都加载完毕
    QList<stHighlightContent> m_stListNote;  // 新文件的注释列表
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
    explicit NotesWidget(DWidget *parent = nullptr);
    ~NotesWidget() override;

    void prevPage();
    void nextPage();
    void DeleteItemByKey();

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
    int qDealWithData(const int &, const QString &) override;

protected:
    void initWidget() override;

private slots:
    void slotDealWithData(const int &, const QString &);

    void slotOpenFileOk(QString strcontent);
    void slotLoadImage(const QImage &);
    void slotSelectItem(QListWidgetItem *);
    void SlotRightSelectItem(const QString &);
    void slotAddAnnotation();

private:
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

private:
    CustomListWidget *m_pNotesList = nullptr;
    QMap<QString, int> m_mapUuidAndPage;      //  uuid 和 页码 对应
    ThreadLoadImageOfNote m_ThreadLoadImage;  // 加载注释缩略图线程
    DPushButton *m_pAddAnnotationBtn = nullptr ;   // 添加注释

    int m_nIndex = -1;                        // 当前注释列表数

};

#endif  // NOTESFORM_H
