/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     leiyu <leiyu@live.com>
* Maintainer: leiyu <leiyu@deepin.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef IMAGELISTVIEW_H
#define IMAGELISTVIEW_H

#include <DListView>

DWIDGET_USE_NAMESPACE
class DocSheet;
class ImageViewModel;
class ImageViewDelegate;
class ImageListView : public DListView
{
    Q_OBJECT
public:
    ImageListView(DocSheet *sheet, QWidget* parent = nullptr);

public:
    void handleOpenSuccess();
    void setBoolMarkVisible(int pageIndex, bool visible);
    void updatePageIndex(int index);
    void scrollToIndex(int pageIndex);

private:
    void initControl();

private slots:
    void onUpdatePageImage(int pageIndex);
    void onItemClicked(const QModelIndex &index);

private:
    DocSheet *m_docSheet;
    ImageViewModel *m_imageModel;
    ImageViewDelegate *m_imageDelegate;
};

#endif // IMAGELISTVIEW_H
