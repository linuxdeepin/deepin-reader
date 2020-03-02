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
#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <DHorizontalLine>
#include <DPushButton>
#include <QMap>
#include <QThread>
#include <QVBoxLayout>

#include "../CustomListWidget.h"
#include "CustomControl/CustomWidget.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签  列表数据
 */

class BookMarkWidget;

class LoadBookMarkThread : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(LoadBookMarkThread)

public:
    explicit LoadBookMarkThread(QObject *parent = nullptr);

signals:
    void sigLoadImage(const int &, const QImage &);

public:
    inline void setBookMark(BookMarkWidget *bookMarkW) { m_pBookMarkWidget = bookMarkW; }

    inline void setBookMarks(const int &count) { m_bookMarks = count; }

    void stopThreadRun();

protected:
    void run() override;

private:
    BookMarkWidget *m_pBookMarkWidget = nullptr;
    int m_bookMarks = 0;      // 书签总数
    int m_nStartIndex = 0;    // 加载图片起始位置
    int m_nEndIndex = 19;     // 加载图片结束位置
    bool m_isRunning = true;  // 运行状态
};

class BookMarkWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BookMarkWidget)

public:
    explicit BookMarkWidget(DWidget *parent = nullptr);
    ~BookMarkWidget() override;

    void prevPage();
    void nextPage();
    void DeleteItemByKey();

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
    // CustomWidget interface

public:
    int qDealWithData(const int &, const QString &) override;
    int qDealWithShortKey(const QString &) override;

    int getBookMarkPage(const int &index);

signals:
    void sigSetBookMarkState(const int &, const int &);

private slots:
    void slotAddBookMark();
    void slotAddBookMark(const QString &);
    void slotDeleteBookItem(const QString &);
    void slotLoadImage(const int &, const QImage &);
    void slotUpdateTheme();
    void slotRightSelectItem(const QString &);
    void slotSelectItemBackColor(QListWidgetItem *);

protected:
    void initWidget() override;

private:
    void OnOpenFileOk(const QString &);
    void slotDocFilePageChanged(const QString &);

    void initConnection();
    QListWidgetItem *addBookMarkItem(const int &);
    void deleteIndexPage(const int &pageIndex);
    void clearItemColor();

private:
    CustomListWidget    *m_pBookMarkListWidget = nullptr;
    DPushButton         *m_pAddBookMarkBtn = nullptr;
    LoadBookMarkThread  m_loadBookMarkThread;

};

#endif  // BOOKMARKFORM_H
