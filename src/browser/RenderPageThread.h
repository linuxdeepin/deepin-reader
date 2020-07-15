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
#ifndef RenderPageThread_H
#define RenderPageThread_H

#include <QThread>
#include <QMutex>
#include <QStack>
#include <QImage>
#include "Global.h"

class SheetBrowser;
class BrowserPage;
struct RenderPageTask {
    SheetBrowser *view = nullptr;
    BrowserPage *item = nullptr;
    double scaleFactor = 1.0;
    Dr::Rotation rotation = Dr::RotateBy0;
    QRect renderRect;
    bool preRender = false; //先加载小图
};

class RenderPageThread : public QThread
{
    Q_OBJECT
public:
    explicit RenderPageThread(QObject *parent = nullptr);

    ~RenderPageThread();

    static void clearTask(BrowserPage *item);

    static void clearTasks(SheetBrowser *view);

    static void appendTask(RenderPageTask task);

    static void appendTasks(QList<RenderPageTask> list);

    static void appendTask(BrowserPage *item, double scaleFactor, Dr::Rotation rotation, QRect renderRect);

    static void destroyForever();

    void run();

signals:
    void sigTaskFinished(BrowserPage *item, QImage image, double scaleFactor, int rotation, QRect rect, bool preRender);

private slots:
    void onTaskFinished(BrowserPage *item, QImage image, double scaleFactor, int rotation, QRect rect, bool preRender);

private:
    RenderPageTask m_curTask;
    QStack<RenderPageTask> m_tasks;
    QMutex m_mutex;
    bool m_quit = false;
    static bool quitForever;
    static RenderPageThread *instance;
};

#endif // RenderPageThread_H
