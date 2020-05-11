#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include "global.h"

class QDateTime;
class DocumentView;

class Database : public QObject
{
    Q_OBJECT

public:
    static Database *instance();

    ~Database();

//    QStringList loadInstanceNames();

//    void restoreTabs();
//    void saveTabs(const QList< DocumentView * > &tabs);
//    void clearTabs();

//    void restoreBookmarks();
//    void saveBookmarks();
//    void clearBookmarks();

//    void restorePerFileSettings(DocumentView *tab);
//    void savePerFileSettings(const DocumentView *tab);

//signals:
//    void tabRestored(const QString &absoluteFilePath, bool continuousMode, LayoutMode layoutMode, bool rightToLeftMode, ScaleMode scaleMode, qreal scaleFactor, Rotation rotation, int currentPage);

private:
    Q_DISABLE_COPY(Database)
    static Database *s_instance;
    Database(QObject *parent = 0);

//    static QString instanceName();

    bool prepareTables();

//    bool prepareTabs_v3();
//    bool prepareBookmarks_v3();
//    bool preparePerFileSettings_v3();

//    void migrateTabs_v2_v3();
//    void migrateTabs_v1_v3();
//    void migrateBookmarks_v2_v3();
//    void migrateBookmarks_v1_v3();
//    void migratePerFileSettings_v2_v3();
//    void migratePerFileSettings_v1_v3();

//    void limitPerFileSettings();

    QSqlDatabase m_database;

};

#endif // DATABASE_H
