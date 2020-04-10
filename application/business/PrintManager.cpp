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
#include "docview/docummentproxy.h"
#include "DocSheet.h"

#include <QPrinter>
#include <QPainter>
#include <QPrintPreviewDialog>

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

    DocummentProxy *_proxy =  m_sheet->getDocProxy();
    if (_proxy) {
        //  文档实际大小
        stFileInfo fileInfo;
        _proxy->docBasicInfo(fileInfo);

        int nPageSize = _proxy->getPageSNum();

        printer->setDocName(m_strPrintName);

        QPainter painter(printer);
        painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

        for (int iIndex = 0; iIndex < nPageSize; iIndex++) {
            QImage image;
            qreal deviceratio = qApp->devicePixelRatio() * 2.0;
            bool rl = _proxy->getImage(iIndex, image, /*rect.width()*/fileInfo.iWidth * deviceratio, /*rect.height()*/fileInfo.iHeight * deviceratio);
            if (rl) {
                QPixmap printpixmap = QPixmap::fromImage(image);
                printpixmap.setDevicePixelRatio(deviceratio);
                painter.drawPixmap(0, 0, printpixmap);
                if (iIndex < nPageSize - 1)
                    printer->newPage();
            }
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
