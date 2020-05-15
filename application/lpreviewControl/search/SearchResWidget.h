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
#ifndef SEARCHRESWIDGET_H
#define SEARCHRESWIDGET_H

#include "CustomControl/CustomWidget.h"
#include "pdfControl/docview/commonstruct.h"

class DocSheet;
class ImageListView;
class QStackedLayout;
class SearchResWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchResWidget)

public:
    explicit SearchResWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~SearchResWidget() override;

    void clearFindResult();
    int  handleFindFinished();
    void handFindContentComming(const stSearchRes &);

public:
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;
    void updateThumbnail(const int &) Q_DECL_OVERRIDE;

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    void addSearchsItem(const int &pageIndex, const QString &text, const int &resultNum);

private:
    DocSheet *m_sheet;
    QStackedLayout *m_stackLayout;
    ImageListView *m_pImageListView;
};

#endif  // NOTESFORM_H
