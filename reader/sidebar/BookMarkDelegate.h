// Copyright (C) 2019 ~ 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BOOKMARKDELEGATE_H
#define BOOKMARKDELEGATE_H

#include <DStyledItemDelegate>

#include <QCache>
#include <QPixmap>

DWIDGET_USE_NAMESPACE

/**
 * @brief The BookMarkDelegate class
 * 书签目录代理类
 */
class BookMarkDelegate : public DStyledItemDelegate
{
public:
    explicit BookMarkDelegate(QAbstractItemView *parent = nullptr);

protected:
    /**
     * @brief paint
     * 书签目录节点绘制
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QAbstractItemView *m_parent = nullptr;

    /**
     * @brief 深色主题反色结果缓存（键：源图 cacheKey()；paint() 为 const 故 mutable）
     */
    mutable QCache<qint64, QPixmap> m_darkPixmapCache;
};

#endif // BOOKMARKDELEGATE_H
