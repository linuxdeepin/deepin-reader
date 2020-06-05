/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
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
#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>

class DocSheet;
class QDateTime;
class DocumentView;
class Database : public QObject
{
    Q_OBJECT

public:
    static Database *instance();

    ~Database();

    bool readOperation(DocSheet *sheet);

    bool saveOperation(DocSheet *sheet);

    bool readBookmarks(const QString &filePath, QSet<int> &bookmarks);

    bool saveBookmarks(const QString &filePath, const QSet<int> bookmarks);

private:
    Q_DISABLE_COPY(Database)

    static Database *s_instance;

    Database(QObject *parent = 0);

    bool prepareOperation();

    bool prepareBookmark();

    QSqlDatabase m_database;

};

#endif // DATABASE_H
