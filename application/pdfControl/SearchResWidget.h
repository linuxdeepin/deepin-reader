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
#ifndef SEARCHRESWIDGET_H
#define SEARCHRESWIDGET_H

#include <QThread>
#include <QVBoxLayout>

#include "CustomControl/CustomWidget.h"
#include "CustomListWidget.h"
#include "NotesItemWidget.h"
#include "docview/commonstruct.h"

/**
 * @brief The LoadSearchResThread class
 * @brief   加载搜索结果缩略图线程
 */

class SearchResWidget;

class LoadSearchResThread : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(LoadSearchResThread)

public:
    explicit LoadSearchResThread(QObject *parent = nullptr);

signals:
    void sigLoadImage(const int &, const QImage &);
    void sigStopFind();

public:
    inline void setSearchResW(SearchResWidget *searchResW) { m_pSearchResWidget = searchResW; }

    inline void setRunning(const bool &state) { m_isRunning = state; }

    inline void setPages(const int &pages) { m_pages = pages; }

    inline QList<stSearchRes> searchList() const { return m_searchContantList; }

    inline void pushSearch(const stSearchRes &search) { m_searchContantList.append(search); }

    void stopThread();

protected:
    void run() Q_DECL_OVERRIDE;

private:
    SearchResWidget *m_pSearchResWidget = nullptr;  // 左侧搜索结果缩略图页面
    bool m_isRunning = false;                       // 线程是否在运行
    //    int m_nStartIndex                   = 0;       //　每次加载开始页
    //    int m_nEndIndex                     = 19;      //　每次加载结束页
    int m_pages = -1;                        //  搜索内容总页数
    QList<stSearchRes> m_searchContantList;  // 搜索全部内容
};

/**
 * @brief The SearchResWidget class
 * 搜索结果界面
 */
class SearchResWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchResWidget)

public:
    explicit SearchResWidget(DWidget *parent = nullptr);
    ~SearchResWidget() Q_DECL_OVERRIDE;

signals:
//    void sigDealWithData(const int &, const QString &);
//    void sigClearWidget();
    void sigCloseFile();
    void sigFlushSearchWidget(const QString &);
//    void sigFindPrev();
//    void sigFindNext();

private slots:
    void SlotDealWithData(const int &, const QString &);

    void slotCloseFile();
    void slotFlushSearchWidget(const QString &);
    void slotGetSearchContant(stSearchRes);
    void slotSearchOver();
    void slotLoadImage(const int &, const QImage &);
//    void slotFindPrev();
//    void slotFindNext();
    void slotSelectItem(QListWidgetItem *);
    void slotStopFind();

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    void __ClearSearchContent();
    void initConnections();
    void initSearchList(const QList<stSearchRes> &);
    void addSearchsItem(const int &page, const QString &text, const int &resultNum);
    void showTips();
    void setSelectItemBackColor(QListWidgetItem *);
    void clearItemColor();

private:
    CustomListWidget *m_pSearchList = nullptr;       // 搜索结果列表
    NotesItemWidget *m_pSearchItemWidget = nullptr;  // 缩略图子窗体
    LoadSearchResThread m_loadSearchResThread;       // 加载搜索缩略图线程
    QListWidgetItem *m_pSearchItem = nullptr;        // 当前鼠标左键点击的item
    bool m_bShowList = false;                        // 是否显示搜索列表
    bool    m_isSearch = false;                     //  是否开启了搜索 , 标志位

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    int getSearchPage(const int &);
};

#endif  // NOTESFORM_H
