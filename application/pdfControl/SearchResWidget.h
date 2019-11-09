#ifndef SEARCHRESWIDGET_H
#define SEARCHRESWIDGET_H

#include <QVBoxLayout>
#include <QThread>

#include "NotesItemWidget.h"
#include "CustomControl/CustomWidget.h"
#include "docview/docummentproxy.h"
#include "CustomListWidget.h"

/**
 * @brief The LoadSearchResThread class
 * @brief   加载搜索结果缩略图线程
 */

class SearchResWidget;

class LoadSearchResThread : public QThread
{
    Q_OBJECT
public:
    LoadSearchResThread(QObject *parent = nullptr);

signals:
    void sigLoadImage(const int &, const QImage &);

public:
    inline void setSearchResW(SearchResWidget *searchResW)
    {
        m_pSearchResWidget = searchResW;
    }

    inline void setRunning(const bool &state)
    {
        m_isRunning = state;
    }

    inline void setPages(const int &pages)
    {
        m_pages = pages;
    }

    inline QList<stSearchRes> searchList() const
    {
        return m_searchContantList;
    }

    inline void pushSearch(const stSearchRes &search)
    {
        m_searchContantList.append(search);
    }

    void stopThread();

protected:
    void run() Q_DECL_OVERRIDE;

private:
    SearchResWidget *m_pSearchResWidget = nullptr; // 左侧搜索结果缩略图页面
    bool m_isRunning                    = false;   // 线程是否在运行
//    int m_nStartIndex                   = 0;       //　每次加载开始页
//    int m_nEndIndex                     = 19;      //　每次加载结束页
    int m_pages                         = -1;      //  搜索内容总页数
    QList<stSearchRes> m_searchContantList;        // 搜索全部内容
};

/**
 * @brief The SearchResWidget class
 * 搜索结果界面
 */
class SearchResWidget : public CustomWidget
{
    Q_OBJECT

public:
    SearchResWidget(CustomWidget *parent = nullptr);
    ~SearchResWidget() Q_DECL_OVERRIDE;

signals:
    void sigClearWidget();
    void sigCloseFile();
    void sigFlushSearchWidget(const QString &);
    void sigFindPrev();
    void sigFindNext();

private slots:
    void slotClearWidget();
    void slotCloseFile();
    void slotFlushSearchWidget(const QString &);
    void slotGetSearchContant(stSearchRes);
    void slotSearchOver();
    void slotLoadImage(const int &, const QImage &);
    void slotFindPrev();
    void slotFindNext();

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    void initConnections();
    void initSearchList(const QList<stSearchRes> &);
    void addSearchsItem(const int &page, const QString &text, const int &resultNum);

private:
    CustomListWidget *m_pNotesList          = nullptr; // 搜索结果列表
    NotesItemWidget *m_pSearchItemWidget    = nullptr;      // 缩略图子窗体
    LoadSearchResThread m_loadSearchResThread;           // 加载搜索缩略图线程

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    int getSearchPage(const int &);
};

#endif // NOTESFORM_H
