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
#include "DocSheet.h"

#include <QPrinter>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QPrintDialog>

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

    QPainter painter(printer);

    QRect rect = painter.viewport();

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

    int nPageSize = m_sheet->pagesNumber();

    printer->margins();

    if (printer->fromPage() == 0 && printer->toPage() == 0) {
        for (int iIndex = 0; iIndex < nPageSize; iIndex++) {
            QImage image;

            if (nPageSize > 100 && rect.width() > 800) {
                //当页数过多时，处理一下
                if (m_sheet->getImage(iIndex, image, 200, 200.0 * static_cast<double>(rect.height()) / static_cast<double>(rect.width()))) {
                    painter.drawImage(rect, image);
                }
            } else {
                if (m_sheet->getImage(iIndex, image, rect.width(), rect.height())) {
                    painter.drawImage(rect, image);
                }
            }

            if (iIndex < nPageSize - 1)
                printer->newPage();
        }
    } else {
        printer->setPageMargins(5, 5, 5, 5, QPrinter::Inch);
        for (int iIndex = printer->fromPage() - 1; iIndex < printer->toPage(); iIndex++) {
            if (iIndex >= nPageSize)
                break;

            QImage image;
            if (m_sheet->getImage(iIndex, image, rect.width(), rect.height()))      //公司只有一台打印机，会发生向右偏移
                painter.drawImage(rect, image);

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
