#include "database.h"
#include "DocSheet.h"
#include "global.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSqlError>
#include <QSqlQuery>

class Transaction
{
public:
    Transaction(QSqlDatabase &database) :  m_committed(false), m_database(database)
    {
        m_database.transaction();
    }

    ~Transaction()
    {
        if (!m_committed) {
            m_database.rollback();
        }
    }

    void commit()
    {
        m_committed = m_database.commit();
    }

private:
    bool m_committed;
    QSqlDatabase &m_database;

};

Database *Database::s_instance = nullptr;

Database *Database::instance()
{
    if (s_instance == nullptr) {
        s_instance = new Database(qApp);
    }

    return s_instance;
}

Database::~Database()
{
    s_instance = nullptr;
}

bool Database::prepareOperation()
{
    Transaction transaction(m_database);

    QSqlQuery query(m_database);
    query.exec("CREATE TABLE operation "
               "(filePath TEXT primary key"
               ",layoutMode INTEGER"
               ",mouseShape INTEGER"
               ",scaleMode INTEGER"
               ",rotation INTEGER"
               ",scaleFactor REAL"
               ",sidebarVisible INTEGER"
               ",sidebarIndex INTEGER"
               ",currentPage INTEGER)");

    if (!query.isActive()) {
        qDebug() << query.lastError();
        return false;
    }

    transaction.commit();
    return true;
}

bool Database::readOperation(DocSheet *sheet)
{
    if (nullptr != sheet && m_database.isOpen()) {
        QSqlQuery query(m_database);

        query.prepare(" select * from operation where filePath = :filePath");
        query.bindValue(":filePath", sheet->filePath());
        if (!query.exec()) {
            qDebug() << query.lastError().text();
            return false;
        }

        if (query.next()) {
            sheet->m_operation.layoutMode = (Dr::LayoutMode)query.value("layoutMode").toInt();
            sheet->m_operation.mouseShape = (Dr::MouseShape)query.value("mouseShape").toInt();
            sheet->m_operation.scaleMode = (Dr::ScaleMode)query.value("scaleMode").toInt();
            sheet->m_operation.rotation = (Dr::Rotation)query.value("rotation").toInt();
            sheet->m_operation.scaleFactor = query.value("scaleFactor").toDouble();
            sheet->m_operation.sidebarVisible = query.value("sidebarVisible").toInt();
            sheet->m_operation.sidebarIndex = query.value("sidebarIndex").toInt();
            sheet->m_operation.currentPage = query.value("currentPage").toInt();
        }
    }

    return true;
}

bool Database::saveOperation(DocSheet *sheet)
{
    if (nullptr != sheet && m_database.isOpen()) {
        QSqlQuery query(m_database);

        query.prepare(" replace into "
                      " operation(filePath,layoutMode,mouseShape,scaleMode,rotation,scaleFactor,sidebarVisible,sidebarIndex,currentPage)"
                      " VALUES(:filePath,:layoutMode,:mouseShape,:scaleMode,:rotation,:scaleFactor,:sidebarVisible,:sidebarIndex,:currentPage)");

        query.bindValue(":filePath", sheet->filePath());
        query.bindValue(":layoutMode", sheet->m_operation.layoutMode);
        query.bindValue(":mouseShape", sheet->m_operation.mouseShape);
        query.bindValue(":scaleMode", sheet->m_operation.scaleMode);
        query.bindValue(":rotation", sheet->m_operation.rotation);
        query.bindValue(":scaleFactor", sheet->m_operation.scaleFactor);
        query.bindValue(":sidebarVisible", sheet->m_operation.sidebarVisible);
        query.bindValue(":sidebarIndex", sheet->m_operation.sidebarIndex);
        query.bindValue(":currentPage", sheet->m_operation.currentPage);

        if (!query.exec())
            qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::prepareBookmark()
{
    Transaction transaction(m_database);

    QSqlQuery query(m_database);
    query.exec("CREATE TABLE bookmark(filePath TEXT,bookmarkIndex INTEGER)");

    if (!query.isActive()) {
        qDebug() << query.lastError();
        return false;
    }

    transaction.commit();
    return true;
}

bool Database::readBookmarks(const QString &filePath, QSet<int> &bookmarks)
{
    if (m_database.isOpen()) {
        QSqlQuery query(m_database);

        query.prepare(" select * from bookmark where filePath = :filePath");
        query.bindValue(":filePath", filePath);

        if (!query.exec()) {
            qDebug() << query.lastError().text();
            return false;
        }

        while (query.next()) {
            bookmarks.insert(query.value("bookmarkIndex").toInt());
        }
    }

    return true;
}

bool Database::saveBookmarks(const QString &filePath, const QSet<int> bookmarks)
{
    if (m_database.isOpen()) {
        QSqlQuery query(m_database);

        m_database.transaction();

        query.prepare("delete from bookmark where filePath = :filePath");
        query.bindValue(":filePath", filePath);
        if (!query.exec()) {
            qDebug() << query.lastError().text();
            m_database.rollback();
            return false;
        }

        foreach (int index, bookmarks) {
            query.prepare(" insert into "
                          " bookmark(filePath,bookmarkIndex)"
                          " VALUES(:filePath,:bookmarkIndex)");

            query.bindValue(":filePath", filePath);
            query.bindValue(":bookmarkIndex", index);

            if (!query.exec()) {
                qDebug() << query.lastError().text();
                m_database.rollback();
                return false;
            }
        }
    }

    m_database.commit();
    return true;
}

Database::Database(QObject *parent) : QObject(parent)
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir().mkpath(path);

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(QDir(path).filePath("user.db"));
    qDebug() << QDir(path);
    m_database.open();

    if (m_database.isOpen()) {
        {
            QSqlQuery query(m_database);
            query.exec("PRAGMA synchronous = OFF");
            query.exec("PRAGMA journal_mode = MEMORY");
        }

        const QStringList tables = m_database.tables();

        if (!tables.contains("operation")) {
            prepareOperation();
        }

        if (!tables.contains("bookmark")) {
            prepareBookmark();
        }
    } else {
        qDebug() << m_database.lastError();
    }
}
