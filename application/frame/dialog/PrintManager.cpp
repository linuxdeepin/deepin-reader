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

#include <QPrinter>
#include <QPainter>
#include <QPrintPreviewDialog>

#include <DDialog>

#include "frame/DocummentFileHelper.h"
#include "utils/PublicFunction.h"

PrintManager::PrintManager(QObject *parent)
    : QObject(parent)
{

}

void PrintManager::showPrintDialog(DWidget *widget)
{
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setColorMode(QPrinter::Color);

    // 创建打印对话框
    QString printerName = printer.printerName();

    if (printerName.size() == 0) {
        DDialog dlg("", tr("No printer found"));
        dlg.setIcon(QIcon(PF::getIconPath("exception-logo")));
        dlg.addButtons(QStringList() << tr("OK"));
        QMargins mar(0, 0, 0, 30);
        dlg.setContentLayoutContentsMargins(mar);
        dlg.exec();
        return;
    }

    QPrintPreviewDialog preview(&printer, widget);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(slotPrintPreview(QPrinter *)));
    preview.exec();
}

void PrintManager::slotPrintPreview(QPrinter *printer)
{
    //  文档实际大小
    stFileInfo fileInfo;
    int nPageSize = 0;
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        _proxy->docBasicInfo(fileInfo);

        nPageSize = _proxy->getPageSNum();  //  pdf 页数

        QPainter painter(printer);
        painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing);
        painter.begin(printer);

        QRect rect = painter.viewport();

        for (int iIndex = 0; iIndex < nPageSize; iIndex++) {
            QImage image;

            bool rl = _proxy->getImage(iIndex, image, rect.width(), rect.height());
            if (rl) {
                painter.drawPixmap(0, 0, QPixmap::fromImage(image));
                if (iIndex < nPageSize - 1)
                    printer->newPage();
            }
        }
        painter.end();
    }
}
