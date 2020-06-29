/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#ifndef RenderThreadDJVU_H
#define RenderThreadDJVU_H

#include <QThread>
#include <QMutex>
#include <QStack>
#include <QImage>

#include "Global.h"

class SheetBrowserDJVUItem;
struct RenderTaskDJVU {
    SheetBrowserDJVUItem *item = nullptr;
    double scaleFactor = 1.0;
    Dr::Rotation rotation = Dr::RotateBy0;
};

class RenderThreadDJVU : public QThread
{
    Q_OBJECT
public:
    explicit RenderThreadDJVU(QObject *parent = nullptr);

    ~RenderThreadDJVU();

    static void appendTask(SheetBrowserDJVUItem *item, double scaleFactor, Dr::Rotation rotation);

    static void destroy();

    void run();

private slots:
    void onTaskFinished(SheetBrowserDJVUItem *item, QImage image, double scaleFactor, int rotation);

signals:
    void sigTaskFinished(SheetBrowserDJVUItem *item, QImage image, double scaleFactor, int rotation);

private:
    RenderTaskDJVU m_curTask;
    QStack<RenderTaskDJVU> m_tasks;
    QMutex m_mutex;
    bool m_quit = false;

    static RenderThreadDJVU *instance;
};

#endif // RenderThreadDJVU_H
