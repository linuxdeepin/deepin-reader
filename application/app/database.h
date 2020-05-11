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

private:
    Q_DISABLE_COPY(Database)

    static Database *s_instance;

    Database(QObject *parent = 0);

    bool prepareOperation();

    bool prepareBookmarks();

    QSqlDatabase m_database;

};

#endif // DATABASE_H
