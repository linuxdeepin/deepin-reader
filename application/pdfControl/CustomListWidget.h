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
#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <DListWidget>
#include "menu/BookMarkMenu.h"
#include "menu/NoteMenu.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The CustomListWidget class
 * 自定义缩略图ListWidget
 */
class DocSheet;
class CustomListWidget : public DListWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomListWidget)

public:
    explicit CustomListWidget(DocSheet *sheet, DWidget *parent = nullptr);

signals:
    void sigSelectItem(QListWidgetItem *);
    void sigValueChanged(const int &);

    void sigListMenuClick(const int &);

public:
    QListWidgetItem *insertWidgetItem(const int &);

    void setListType(const int &nListType);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void slotShowSelectItem(QListWidgetItem *);

private:
    void showNoteMenu();
    void showBookMarkMenu();

private:
    int     m_nListType = -1;
    BookMarkMenu *m_pBookMarkMenu{nullptr};
    NoteMenu *pNoteMenu{nullptr};
    DocSheet *m_sheet = nullptr;
};

#endif // CUSTOMLISTWIDGET_H
