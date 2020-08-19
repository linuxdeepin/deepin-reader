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

#include "widgets/CustomWidget.h"
#include <QPointer>

namespace deepin_reader {
class Annotation;
}
class DocSheet;
class ImageListView;
class DocummentProxy;
class NotesWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NotesWidget)

signals:
    void sigUpdateThumbnail(const int &page);

public:
    explicit NotesWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~NotesWidget() override;

    void prevPage();
    void nextPage();
    void DeleteItemByKey();
    void handleOpenSuccess();
    void handleAnntationMsg(const int &, deepin_reader::Annotation *anno);

public:
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;
    void updateThumbnail(const int &) Q_DECL_OVERRIDE;
    void showMenu() Q_DECL_OVERRIDE;

public slots:
    void changeResetModelData();

private:
    void initWidget() Q_DECL_OVERRIDE;

private slots:
    void onListMenuClick(const int &);
    void onListItemClicked(int row);
    void onAddAnnotation();

private:
    void copyNoteContent();
    void addNoteItem(deepin_reader::Annotation *anno);
    void deleteNoteItem(deepin_reader::Annotation *anno);
    void deleteAllItem();

private:
    QPointer<DocSheet> m_sheet;
    ImageListView     *m_pImageListView;
    DPushButton       *m_pAddAnnotationBtn;
};

#endif  // NOTESFORM_H
