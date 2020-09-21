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
#ifndef DPDFIUMCONTROLINTERFACE_H
#define DPDFIUMCONTROLINTERFACE_H

#include <QObject>

class DPdfium;
class DPdfiumControlInterface
{
public:
    virtual ~DPdfiumControlInterface() = 0;

    virtual DPdfium *loadFile(QString filename, QString password = QString()) = 0;
};
Q_DECLARE_INTERFACE(DPdfiumControlInterface, "org.deepin-project.DPdfiumControlInterface")

#endif // DPDFIUMCONTROLINTERFACE_H
