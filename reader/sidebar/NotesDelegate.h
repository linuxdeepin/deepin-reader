// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NOTESDELEGATE_H
#define NOTESDELEGATE_H

#include <DStyledItemDelegate>

#include <QCache>
#include <QPixmap>

DWIDGET_USE_NAMESPACE
/**
 * @brief The NotesDelegate class
 * 注释代理
 */
class NotesDelegate : public DStyledItemDelegate
{
public:
    explicit NotesDelegate(QAbstractItemView *parent = nullptr);

protected:
    /**
     * @brief paint
     * 绘制
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /**
     * @brief sizeHint
     * 节点大小
     * @param option
     * @param index
     * @return
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QAbstractItemView *m_parent = nullptr;

    /**
     * @brief 深色主题反色结果缓存（键：源图 cacheKey()；paint() 为 const 故 mutable）
     */
    mutable QCache<qint64, QPixmap> m_darkPixmapCache;
};

#endif // NOTESDELEGATE_H
