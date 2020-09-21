/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
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
#include "DPdfController.h"

#include <QPluginLoader>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

Q_GLOBAL_STATIC(DPdfController, theInstance)
DPdfController::DPdfController()
{
    qDebug() << "load pdfplugin result = " << loadDPdfPlugin();
}

DPdfiumControlInterface *DPdfController::getInstance()
{
    return theInstance()->m_dPdfPlugin;
}

bool DPdfController::loadDPdfPlugin()
{
    QDir pluginsDir("/home/leiyu/workspace/Work/Read_20200723_sp3/3rdparty/lib");
    QPluginLoader pluginLoader(pluginsDir.absoluteFilePath("libdpdf"));
    QObject *plugin = pluginLoader.instance();
    if (plugin) {
        m_dPdfPlugin = qobject_cast<DPdfiumControlInterface *>(plugin);
        if (m_dPdfPlugin)
            return true;
    }
    qDebug() << "load pdf plugin error = " << pluginLoader.errorString();
    return false;
}
