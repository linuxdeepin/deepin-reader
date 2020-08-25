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
#ifndef PAGEVIEWPORTTHREAD_H
#define PAGEVIEWPORTTHREAD_H

#include "BrowserPage.h"
#include "Global.h"

#include <QThread>
#include <QMutex>
#include <QStack>
#include <QImage>
#include <QPointer>

class SheetBrowser;
class BrowserPage;
struct RenderViewportTask {
    BrowserPage *page = nullptr;
    double scaleFactor = 1.0;
    Dr::Rotation rotation = Dr::RotateBy0;
    QRect renderRect;
};

struct RenderViewportTaskPiece {
    RenderViewportTask task;
    int pieceIndex = 0;
};

class PageViewportThread : public QThread
{
    Q_OBJECT
public:
    static void destroyForever();

    static void appendTask(RenderViewportTask task);

signals:
    void sigTaskFinished(BrowserPage *item, QImage image, double scaleFactor,  QRect rect);

private:
    explicit PageViewportThread(QObject *parent = nullptr);

    virtual ~PageViewportThread();

    void run();

    void appendTaskPiece(RenderViewportTaskPiece task);

private slots:
    void onTaskFinished(BrowserPage *item, QImage image, double scaleFactor,  QRect rect);

private:
    RenderViewportTaskPiece m_curTaskPiece;
    QStack<RenderViewportTaskPiece> m_taskPieces;
    QMutex m_mutex;
    bool m_quit = false;
    static bool quitForever;
    static QList<PageViewportThread *> instances;
    static PageViewportThread *instance(int threadIndex);
};

#endif // PAGEVIEWPORTTHREAD_H
