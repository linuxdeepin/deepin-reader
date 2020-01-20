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
#include "CustomItemWidget.h"

CustomItemWidget::CustomItemWidget(const QString &name, QWidget *parent)
    : CustomWidget(name, parent)
{
    setFocusPolicy(Qt::NoFocus);
}

/**
 * @brief CustomItemWidget::setLabelImage
 * 给新label填充缩略图
 * @param image
 */
void CustomItemWidget::setLabelImage(const QImage &image)
{
    if (m_pPicture != nullptr) {
        QPixmap pixmap = QPixmap::fromImage(image);
        m_pPicture->setBackgroundPix(pixmap);
    }
}

/**
 * @brief CustomItemWidget::setLabelPage
 * 设置页码标签内容
 * @param value
 * @param nShowPage
 */
void CustomItemWidget::setLabelPage(const int &value, const int &nShowPage)
{
    m_nPageIndex = value;

    if (m_pPageNumber) {
        int nnPage = value + 1;
        if (nShowPage == 1) {
            QString sPageText = tr("Page %1").arg(nnPage);
            m_pPageNumber->setText(sPageText);
        } else {
            m_pPageNumber->setText(QString("%1").arg(nnPage));
        }
    }
}

/**
 * @brief CustomItemWidget::nPageIndex
 * 获取当前注释缩略图index
 * @return
 */
int CustomItemWidget::nPageIndex() const
{
    return m_nPageIndex;
}

void CustomItemWidget::resizeEvent(QResizeEvent *event)
{
    CustomWidget::resizeEvent(event);

    auto parentWidget = reinterpret_cast<QWidget *>(this->parent());
    if (parentWidget) {
        resize(parentWidget->width(), this->height());
    }
}
