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
#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include "CustomControl/CustomWidget.h"
#include <QPointer>

class DocSheet;
class PagingWidget;
class ImageListView;
class DocummentProxy;
class ThumbnailWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ThumbnailWidget)

public:
    explicit ThumbnailWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~ThumbnailWidget() Q_DECL_OVERRIDE;

public:
    void handleOpenSuccess();
    void handlePage(int index);
    void handleRotate(int rotate);
    void setBookMarkState(const int &index, const int &type);

    void prevPage();
    void nextPage();

    void adaptWindowSize(const double &scale)  Q_DECL_OVERRIDE;
    void updateThumbnail(const int &index) Q_DECL_OVERRIDE;

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    void scrollToCurrentPage();

private:
    QPointer<DocSheet> m_sheet;
    PagingWidget *m_pPageWidget;
    ImageListView *m_pImageListView;
};

#endif  // THUMBNAILWIDGET_H
