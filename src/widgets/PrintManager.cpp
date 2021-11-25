/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#include "PrintManager.h"
#include "DocSheet.h"

#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QDebug>

PrintManager::PrintManager(DocSheet *sheet, QObject *parent)
    : QObject(parent), m_sheet(sheet)
{
    if (nullptr != m_sheet)
        setPrintPath(m_sheet->filePath());
}

void PrintManager::showPrintDialog(DWidget *widget)
{
    DPrintPreviewDialog preview(widget);

    connect(&preview, SIGNAL(paintRequested(DPrinter *)), SLOT(slotPrintPreview(DPrinter *)));

    preview.exec();
}

void PrintManager::slotPrintPreview(DPrinter *printer)
{
    if (nullptr == m_sheet)
        return;

    printer->setDocName(m_strPrintName);

    qreal left = 0;
    qreal top = 0;
    qreal right = 0;
    qreal bottom = 0;
    printer->getPageMargins(&left, &top, &right, &bottom, QPrinter::DevicePixel);

    QPainter painter(printer);

    QRect rect = painter.viewport();

    QRect paintRect = QRect(static_cast<int>(left), static_cast<int>(top),
                            static_cast<int>(rect.width() - left - right), static_cast<int>(rect.height() - bottom - top));
    QRect ImageRect = QRect(0, 0, static_cast<int>(rect.width() - left - right), static_cast<int>(rect.height() - bottom - top));

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

    int nPageSize = m_sheet->pagesNumber();

    printer->margins();

    if (printer->fromPage() == 0 && printer->toPage() == 0) {
        for (int iIndex = 0; iIndex < nPageSize; iIndex++) {
            QImage image;

            if (nPageSize > 100 && ImageRect.width() > 800) {
                //当页数过多时，处理一下
                if (m_sheet->getImage(iIndex, image, 200, 200.0 * static_cast<double>(ImageRect.height()) / static_cast<double>(ImageRect.width()),
                                      Qt::KeepAspectRatio)) { // 保持图片的原始横竖比

                    int tmpWidth = paintRect.width();
                    int tmpHeight = image.height() * tmpWidth / image.width();
                    if (tmpHeight > paintRect.height()) {
                        tmpHeight = paintRect.height();
                        tmpWidth = image.width() * tmpHeight / image.height();
                    }

                    painter.drawImage(QRect((paintRect.width() - tmpWidth + static_cast<int>(left + right)) / 2,
                                            (paintRect.height() - tmpHeight + static_cast<int>(top + bottom)) / 2,
                                            tmpWidth,
                                            tmpHeight),
                                      image); // 居中显示
                }
            } else {
                if (m_sheet->getImage(iIndex, image, ImageRect.width(), ImageRect.height(), Qt::KeepAspectRatio)) { // 保持图片的原始横竖比
                    painter.drawImage(QPoint((paintRect.width() - image.width() + static_cast<int>(left + right)) / 2,
                                             (paintRect.height() - image.height() + static_cast<int>(top + bottom)) / 2),
                                      image); // 居中显示
                }
            }

            if (iIndex < nPageSize - 1)
                printer->newPage();
        }
    } else {
        for (int iIndex = printer->fromPage() - 1; iIndex < printer->toPage(); iIndex++) {
            if (iIndex >= nPageSize)
                break;

            QImage image;
            if (m_sheet->getImage(iIndex, image, ImageRect.width(), ImageRect.height(), Qt::KeepAspectRatio)) { // 保持图片的原始横竖比
                painter.drawImage(QPoint((paintRect.width() - image.width() + static_cast<int>(left + right)) / 2,
                                         (paintRect.height() - image.height() + static_cast<int>(top + bottom)) / 2),
                                  image); // 居中显示
            }

            if (iIndex < printer->toPage() - 1)
                printer->newPage();
        }
    }
}

void PrintManager::setPrintPath(const QString &strPrintPath)
{
    m_strPrintPath = strPrintPath;
    QString sPath = strPrintPath;
    int nLastPos = sPath.lastIndexOf('/');
    nLastPos++;
    m_strPrintName = sPath.mid(nLastPos);
}
