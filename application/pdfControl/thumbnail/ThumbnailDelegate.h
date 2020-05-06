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
#ifndef IMAGEVIEWDELEGATE_H
#define IMAGEVIEWDELEGATE_H

#include <DStyledItemDelegate>

DWIDGET_USE_NAMESPACE
class ThumbnailDelegate : public DStyledItemDelegate
{
public:
    ThumbnailDelegate(QAbstractItemView* parent = nullptr);

protected:
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    void drawBookMark(QPainter* painter, const QRect& rect, bool visible) const;

private:
    QAbstractItemView* m_parent;
};

#endif // IMAGEVIEWDELEGATE_H
