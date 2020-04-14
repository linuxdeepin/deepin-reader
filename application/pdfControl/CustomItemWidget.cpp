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

CustomItemWidget::CustomItemWidget(DWidget *parent)
    : CustomWidget(parent)
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

void CustomItemWidget::imageAdaptView(const int &, const int &)
{
    return;
}

void CustomItemWidget::resizeEvent(QResizeEvent *event)
{
    CustomWidget::resizeEvent(event);

    auto parentWidget = qobject_cast<QWidget *>(this->parent());
    if (parentWidget) {
        resize(parentWidget->width(), this->height());
    }
}

QString CustomItemWidget::calcText(const QFont &font, const QString &note, const QSize &size)
{
    QString text;
    QString tempText;
    int totalHeight = size.height();
    int lineWidth = size.width() - 12;

    QFontMetrics fm(font);

    int calcHeight = 0;
    int lineHeight = fm.height();
    int lineSpace = 0;
    int lineCount = (totalHeight - lineSpace) / lineHeight;
    int prevLineCharIndex = 0;
    for (int charIndex = 0; charIndex < note.size() && lineCount >= 0; ++charIndex) {
        int fmWidth = fm.horizontalAdvance(tempText);
        if (fmWidth > lineWidth) {
            calcHeight += lineHeight /*+3*/;
            if (calcHeight + lineHeight > totalHeight) {
                QString endString = note.mid(prevLineCharIndex);
                const QString &endText = fm.elidedText(endString, Qt::ElideRight, size.width());
                text += endText;

                lineCount = 0;
                break;
            }

            QChar currChar = tempText.at(tempText.length() - 1);
            QChar nextChar = note.at(note.indexOf(tempText) + tempText.length());
            if (currChar.isLetter() && nextChar.isLetter()) {
                //                tempText += '-';
            }
            fmWidth = fm.horizontalAdvance(tempText);
            if (fmWidth > size.width()) {
                --charIndex;
                tempText = tempText.remove(tempText.length() - 2, 1);
            }
            text += tempText;

            --lineCount;
            if (lineCount > 0) {
                text += "\n";
            }
            tempText = note.at(charIndex);

            prevLineCharIndex = charIndex;
        } else {
            tempText += note.at(charIndex);
        }
    }

    if (lineCount > 0) {
        text += tempText;
    }

    return text;
}



