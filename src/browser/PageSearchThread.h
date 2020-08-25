/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
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
#ifndef PAGESEARCHTHREAD_H
#define PAGESEARCHTHREAD_H

#include "Model.h"

#include <QThread>

class BrowserPage;
class PageSearchThread : public QThread
{
    Q_OBJECT
signals:
    void sigSearchReady(const deepin_reader::SearchResult &res);

public:
    explicit PageSearchThread(QObject *parent = nullptr);

    virtual ~PageSearchThread();

public:
    void startSearch(const QList<BrowserPage *> pagelst, const QString &searchText, int startPage);

    void stopSearch();

protected:
    void run() override;

private:
    bool m_quit = false;
    int m_startIndex = 0;
    QString m_searchText;
    QList<BrowserPage *> m_pagelst;
};

#endif // PAGESEARCHTHREAD_H
