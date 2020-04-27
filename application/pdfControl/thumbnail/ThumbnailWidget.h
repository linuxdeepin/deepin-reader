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
#include <QPointer>

#include "CustomControl/CustomWidget.h"
#include "../CustomListWidget.h"
#include "PagingWidget.h"
#include "ThumbnailItemWidget.h"

/**
 * @brief The ThreadLoadImage class
 * @brief   加载缩略图线程
 */
class DocummentProxy;
class ThumbnailWidget;

class ThreadLoadImage : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(ThreadLoadImage)

public:
    explicit ThreadLoadImage(QObject *parent = nullptr);
    ~ThreadLoadImage() override
    {
        stopThreadRun();
    }

public:
    void stopThreadRun();

    inline void setPages(const int pages)
    {
        m_pages = pages;
    }

    inline int endPage()
    {
        return m_nEndPage;
    }

    inline void setIsLoaded(const bool &load)
    {
        m_isLoaded = load;
    }

    inline bool isLoaded()
    {
        return m_isLoaded;
    }

    inline void setThumbnail(ThumbnailWidget *thumbnail)
    {
        m_pThumbnailWidget = thumbnail;
    }

    bool inLoading(int);

    void setStartAndEndIndex(int startIndex, int endIndex);

    inline void clearList()
    {
        m_listLoad.clear();
    }

    void setFilePath(const QString &strfilepath)
    {
        m_filepath = strfilepath;
    }

    void setProxy(DocummentProxy *proxy)
    {
        m_proxy = proxy;
    }

signals:
    void sigLoadImage(const int &, const QImage &);

    void sigRotateImage(const int &index);

    void sigLoadingImageOk();

protected:
    void run() override;

private:
    int m_pages = 0;          // 文件总页数
    bool m_isLoaded = false;  // 是都加载完毕
    ThumbnailWidget *m_pThumbnailWidget = nullptr;
    int m_nStartPage = 0;  // 加载图片起始页码
    int m_nEndPage = 19;   // 加载图片结束页码
    QList<int> m_listLoad; // 加载过来的缩略图
    DocummentProxy *m_proxy = nullptr;
    QString m_filepath;
};

/*
 *缩略图列表页面
 */
class DocSheet;
class ThumbnailWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ThumbnailWidget)

public:
    explicit ThumbnailWidget(DocSheet *sheet, DWidget *parent = nullptr);

    ~ThumbnailWidget() override;

public:
    void handleOpenSuccess();

    void handleRotate(int rotate);

    void handlePage(int page);

    bool isLoading();

    void prevPage();

    void nextPage();

    void adaptWindowSize(const double &scale) Q_DECL_OVERRIDE; //缩略图列表自适应窗体大小  add by duanxiaohui 2020-3-19

    void updateThumbnail(const int &) Q_DECL_OVERRIDE;

protected:
    void initWidget() override;

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void fillContantToList();

    void showItemBookMark();

    void setSelectItemBackColor(QListWidgetItem *);

    void addThumbnailItem(const int &);

    void initConnection();

    void SetSelectItemColor(QListWidgetItem *item, const bool &);

    ThumbnailItemWidget *getItemWidget(QListWidgetItem *);

    void reCalcItemSize();

private slots:
    void slotUpdateTheme();

    void slotRotateThumbnail(const int &);

    void slotLoadThumbnail(const int &);

    void slotLoadImage(const int &row, const QImage &image);

    void SlotSetBookMarkState(const int &, const int &);

private:
    CustomListWidget *m_pThumbnailListWidget = nullptr;  // list widget item子界面
    PagingWidget *m_pPageWidget = nullptr;               // 跳页界面
    int m_totalPages = -1;                               // 总页码数
    ThreadLoadImage m_ThreadLoadImage;                   // 加载缩略图线程
    bool m_isLoading = true;                            // 缩略图list是否初始化完毕
    int m_nRotate = 0;                                   // 旋转度数
    int m_nValuePreIndex = 0;                            // 每一个item所占scrollbar的大小

    QString     m_strBindPath = "";
    QPointer<DocSheet>   m_sheet;
    int m_page;
    double m_scale = 1.0;
    bool m_firstLoadingImage = false;                    // 缩略图list是否初始化完毕
};

#endif  // THUMBNAILWIDGET_H
