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
#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>

class DocSheet;
class Sheet;
class QDateTime;
class DocumentView;
class Database : public QObject
{
    Q_OBJECT

public:
    static Database *instance();

    ~Database();

    /**
     * @brief readOperation
     * 读取操作
     * @param sheet 哪个文档
     * @return
     */
    bool readOperation(DocSheet *sheet);

    /**
     * @brief saveOperation
     * 保存操作
     * @param sheet 哪个文档
     * @return
     */
    bool saveOperation(DocSheet *sheet);

    /**
     * @brief readBookmarks
     * 读取书签
     * @param filePath 文件名(唯一标识)
     * @param bookmarks 书签列表
     * @return
     */
    bool readBookmarks(const QString &filePath, QSet<int> &bookmarks);

    /**
     * @brief saveBookmarks
     * 保存书签
     * @param filePath 文件名(唯一标识)
     * @param bookmarks 书签列表
     * @return
     */
    bool saveBookmarks(const QString &filePath, const QSet<int> bookmarks);

private:
    Q_DISABLE_COPY(Database)

    static Database *s_instance;

    Database(QObject *parent = nullptr);

    /**
     * @brief prepareOperation
     *准备操作记录表(文档操作)
     * @return
     */
    bool prepareOperation();

    /**
     * @brief prepareBookmark
     * 准备书签表
     * @return
     */
    bool prepareBookmark();

    QSqlDatabase m_database;

};

#endif // DATABASE_H
