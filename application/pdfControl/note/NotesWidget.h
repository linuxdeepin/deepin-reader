/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     leiyu
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

#include "CustomControl/CustomWidget.h"
#include <QPointer>

class DocSheet;
class ImageListView;
class DocummentProxy;
class NotesWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NotesWidget)

signals:
    void sigDeleteContent(const int &, const QString &);
    void sigUpdateThumbnail(const int &page);

public:
    explicit NotesWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~NotesWidget() override;

    void prevPage();
    void nextPage();
    void DeleteItemByKey();
    void handleOpenSuccess();
    void handleRotate(int rotate);
    void handleAnntationMsg(const int &, const QString &);

public:
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;
    void updateThumbnail(const int &) Q_DECL_OVERRIDE;

private:
    void initWidget() Q_DECL_OVERRIDE;

private slots:
    void onListMenuClick(const int &);
    void onListItemClicked(int row);
    void onAddAnnotation();

private:
    void copyNoteContent();
    void addNoteItem(const QString &, const int &iType = NOTE_HIGHLIGHT);
    void deleteNoteItem(const QString &);
    void updateNoteItem(const QString &);
    void scrollToModelInexPage(const QModelIndex &index);

private:
    QPointer<DocSheet> m_sheet;
    ImageListView     *m_pImageListView;
    DPushButton       *m_pAddAnnotationBtn;
};

#endif  // NOTESFORM_H
