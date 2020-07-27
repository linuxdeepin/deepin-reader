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
#ifndef RenderViewportThread_H
#define RenderViewportThread_H

#include <QThread>
#include <QMutex>
#include <QStack>
#include <QImage>
#include <QPointer>
#include "BrowserPage.h"

#include "Global.h"

class SheetBrowser;
class BrowserPage;
struct RenderViewportTask {
    BrowserPage *page = nullptr;
    double scaleFactor = 1.0;
    Dr::Rotation rotation = Dr::RotateBy0;
    QRect renderRect;
    bool cover = false;
};

class RenderViewportThread : public QThread
{
    Q_OBJECT
public:
    explicit RenderViewportThread(QObject *parent = nullptr);

    ~RenderViewportThread();

    static void destroyForever();

    static void appendTask(RenderViewportTask task);

    static int  count(BrowserPage *page);        //当前任务数量

    void run();

signals:
    void sigTaskFinished(BrowserPage *item, QImage image, double scaleFactor, int rotation, QRect rect);

private slots:
    void onTaskFinished(BrowserPage *item, QImage image, double scaleFactor, int rotation, QRect rect);

private:
    RenderViewportTask m_curTask;
    QStack<RenderViewportTask> m_tasks;
    QMutex m_mutex;
    bool m_quit = false;
    static bool quitForever;
    static RenderViewportThread *m_instance;
};

#endif // RenderViewportThread_H
