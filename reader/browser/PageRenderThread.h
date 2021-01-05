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
#ifndef PAGERENDERTHREAD_H
#define PAGERENDERTHREAD_H

#include "Global.h"
#include "Model.h"

#include <QThread>
#include <QMutex>
#include <QStack>
#include <QImage>

class SheetBrowser;
class BrowserPage;
struct RenderPageTask {
    enum RenderPageTaskType {
        Image = 1,
        Word = 2,
        BigImage = 3,
        ImageSlice = 4
    };

    int type = RenderPageTaskType::Image;
    BrowserPage *page = nullptr;
    double scaleFactor = 1.0;   //暂时不用
    int pixmapId = 0;           //任务艾迪
    QRect whole = QRect();      //整个大小
    QRect slice = QRect();      //切片大小
};

/**
 * @brief The PageRenderThread class
 * 执行加载图片和文字等耗时操作的线程
 */
class PageRenderThread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief clearTask
     * 清除该项和该类型的任务
     * @param item 项指针
     * @param pixmapId 删除不同的pixmapId,-1为删除所有
     * @return 是否成功
     */
    static bool clearImageTask(BrowserPage *item, int pixmapId = -1);

    /**
     * @brief 添加任务
     * @param task 任务
     * @param beforeFirst 插入到最开始
     */
    static void appendTask(RenderPageTask task);

    /**
     * @brief 添加延时任务
     * @param task 任务
     */
    static void appendDelayTask(RenderPageTask task);

    /**
     * @brief appendTasks
     * 根据任务参数添加任务
     * @param item 项指针
     * @param scaleFactor 缩放因子
     * @param rotation 旋转
     * @param renderRect 所占区域
     */
    static void appendTask(BrowserPage *page, int pixmapId, QRect whole, QRect slice);

    /**
     * @brief destroyForever
     * 销毁线程且不会再被创建
     */
    static void destroyForever();

private:
    explicit PageRenderThread(QObject *parent = nullptr);

    ~PageRenderThread();

    void run();

    bool execNextImageTask();

    bool execNextImageSliceTask();

    bool execNextWordTask();

    bool getNextTask(RenderPageTask::RenderPageTaskType type, RenderPageTask &task);

signals:
    void sigImageTaskFinished(BrowserPage *item, QPixmap pixmap, int pixmapId, QRect slice);

    void sigWordTaskFinished(BrowserPage *item, QList<deepin_reader::Word> words);

private slots:
    void onImageTaskFinished(BrowserPage *item, QPixmap pixmap, int pixmapId, QRect slice);

    void onWordTaskFinished(BrowserPage *item, QList<deepin_reader::Word> words);

    void onDelayTaskTimeout();

private:
    QList<RenderPageTask> m_tasks;

    QMutex m_mutex;

    bool m_quit = false;

    RenderPageTask m_delayTask;

    QTimer *m_delayTimer = nullptr;

    static bool quitForever;

    static PageRenderThread *s_instance;

    static PageRenderThread *instance();
};

#endif // PAGERENDERTHREAD_H
