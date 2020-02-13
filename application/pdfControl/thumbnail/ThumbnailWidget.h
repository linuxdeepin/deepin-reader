/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <DPushButton>

#include <QPalette>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

#include "CustomControl/CustomWidget.h"
#include "../CustomListWidget.h"
#include "PagingWidget.h"
#include "ThumbnailItemWidget.h"

/**
 * @brief The ThreadLoadImage class
 * @brief   加载缩略图线程
 */

class ThumbnailWidget;

class ThreadLoadImage : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(ThreadLoadImage)

public:
    explicit ThreadLoadImage(QObject *parent = nullptr);
    ~ThreadLoadImage() Q_DECL_OVERRIDE { stopThreadRun(); }

public:
    void stopThreadRun();

    inline void setPages(const int pages) { m_pages = pages; }

    inline int endPage() { return m_nEndPage; }

    inline void setIsLoaded(const bool &load) { m_isLoaded = load; }

    inline bool isLoaded() { return m_isLoaded; }

    inline void setThumbnail(ThumbnailWidget *thumbnail) { m_pThumbnailWidget = thumbnail; }

    bool inLoading(int);

    void setStartAndEndIndex(int startIndex, int endIndex);

    inline void clearList() {m_listLoad.clear();}

signals:
    void sigLoadImage(const int &, const QImage &);
    void sigRotateImage(const int &index);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    int m_pages = 0;          // 文件总页数
    bool m_isLoaded = false;  // 是都加载完毕
    ThumbnailWidget *m_pThumbnailWidget = nullptr;
    int m_nStartPage = 0;  // 加载图片起始页码
    int m_nEndPage = 19;   // 加载图片结束页码
    QList<int> m_listLoad; // 加载过来的缩略图
};

/*
 *缩略图列表页面
 */
class ThumbnailWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ThumbnailWidget)

public:
    explicit ThumbnailWidget(DWidget *parent = nullptr);
    ~ThumbnailWidget() Q_DECL_OVERRIDE;

signals:
    void sigFilePageChanged(const QString &);
    void sigOpenFileOk();
    void sigCloseFile();
    void sigSetRotate(int);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void fillContantToList();

    inline bool isLoading() { return m_isLoading; }
    void showItemBookMark(int ipage = -1);

    void prevPage();
    void nextPage();

//    void forScreenPageing(bool);

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    void setSelectItemBackColor(QListWidgetItem *);
    void addThumbnailItem(const int &);
    void initConnection();

private slots:
    void slotOpenFileOk();
    void slotDocFilePageChanged(const QString &);
    void slotCloseFile();
    void slotUpdateTheme();

    void slotSetRotate(int);
    void slotRotateThumbnail(int);
    void slotLoadThumbnail(int);
    void slotLoadImage(const int &row, const QImage &image);

private:
    CustomListWidget *m_pThumbnailListWidget = nullptr;  // list widget item子界面
    PagingWidget *m_pPageWidget = nullptr;               // 跳页界面
    int m_totalPages = -1;                               // 总页码数
    ThreadLoadImage m_ThreadLoadImage;                   // 加载缩略图线程
    bool m_isLoading = false;                            // 缩略图list是否初始化完毕
//    ThreadRotateImage m_threadRotateImage;               // 旋转缩略图
    int m_nRotate = 0;                                   // 旋转度数
    int m_nValuePreIndex = 0;                            // 每一个item所占scrollbar的大小
};

#endif  // THUMBNAILWIDGET_H
