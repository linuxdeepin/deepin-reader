// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef IMAGEVIEWDELEGATE_H
#define IMAGEVIEWDELEGATE_H

#include <DStyledItemDelegate>
#include <QPixmap>

DWIDGET_USE_NAMESPACE
/**
 * @brief The ThumbnailDelegate class
 * 缩略图代理
 */
class ThumbnailDelegate : public DStyledItemDelegate
{
public:
    explicit ThumbnailDelegate(QAbstractItemView *parent = nullptr);

protected:
    /**
     * @brief paint
     * 绘制事件
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
    /**
     * @brief drawBookMark
     * 绘制书签图标
     * @param painter
     * @param rect
     * @param visible
     */
    void drawBookMark(QPainter *painter, const QRect &rect, bool visible) const;

    /**
     * @brief nightPixmap
     * 夜间智能滤镜/深色主题下缩略图的反色结果（带缓存）：
     * 走主干 NightFilter 管线（CIELAB L* 反转 + 图片对象区域跳过 + 扫描页整页反色特判），
     * 与主视图观感一致
     * @param src 原始缩略图
     * @param imageRects 图片对象 bbox（与 src 像素对齐，这些区域不反色避免照片负片）
     * @return 反色后的缩略图
     */
    QPixmap nightPixmap(const QPixmap &src, const QVector<QRectF> &imageRects = QVector<QRectF>()) const;

private:
    QAbstractItemView *m_parent = nullptr;

    mutable QPixmap m_nightSourceCache;    // 反色缓存的源缩略图
    mutable QPixmap m_nightPixmapCache;    // 反色后的缩略图
};

#endif // IMAGEVIEWDELEGATE_H
