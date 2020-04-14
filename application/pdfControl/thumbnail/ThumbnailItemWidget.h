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
#ifndef THUMBNAILITEMWIDGET_H
#define THUMBNAILITEMWIDGET_H

#include "../CustomItemWidget.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   缩略图中的item
 */
class RotateImageLabel;
class ThumbnailItemWidget : public CustomItemWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ThumbnailItemWidget)

public:
    explicit ThumbnailItemWidget(DWidget *parent = nullptr);

    void setLabelImage(const QImage &);

public:
    void setBSelect(const bool &paint);

    void rotateThumbnail(int);

    void qSetBookMarkShowStatus(const bool &bshow);

protected:
    void initWidget() override;

public:
    void adaptWindowSize(const double &scale) Q_DECL_OVERRIDE;

private:
    RotateImageLabel  *m_label = nullptr;          // 承载缩略图的label
};

#endif  // THUMBNAILITEMWIDGET_H
