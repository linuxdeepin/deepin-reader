/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#include "pdfControl/docview/docummentproxy.h"
#include "DocSheet.h"

#include <QPrinter>
#include <QPainter>
#include <QPrintPreviewDialog>
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
    QPrinter printer;

    QPrintPreviewDialog preview(&printer, widget);

    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(slotPrintPreview(QPrinter *)));

    preview.exec();
}

void PrintManager::slotPrintPreview(QPrinter *printer)
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

    QRect paintRect = QRect(left, top, rect.width() - left - right, rect.height() - bottom - top);
    QRect ImageRect = QRect(0, 0, rect.width() - left - right, rect.height() - bottom - top);

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

    int nPageSize = m_sheet->pagesNumber();

    if (printer->fromPage() == 0 && printer->toPage() == 0) {
        for (int iIndex = 0; iIndex < nPageSize; iIndex++) {
            QImage image;

            if (nPageSize > 100 && ImageRect.width() > 800) {
                //当页数过多时，处理一下
                if (m_sheet->getImage(iIndex, image, 200, 200.0 * static_cast<double>(ImageRect.height()) / static_cast<double>(ImageRect.width()))) {
                    painter.drawImage(paintRect, image);
                }
            } else {
                if (m_sheet->getImage(iIndex, image, ImageRect.width(), ImageRect.height())) {
                    painter.drawImage(paintRect, image);
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
            if (m_sheet->getImage(iIndex, image, ImageRect.width(), ImageRect.height()))      //公司只有一台打印机，会发生向右偏移
                painter.drawImage(paintRect, image);

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
