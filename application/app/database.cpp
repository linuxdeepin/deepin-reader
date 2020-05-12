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
               ",openThumbnail INTEGER"
               ",leftIndex INTEGER"
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
        if (query.exec() && query.next()) {
            sheet->operation().layoutMode = (Dr::LayoutMode)query.value("layoutMode").toInt();
            sheet->operation().mouseShape = (Dr::MouseShape)query.value("mouseShape").toInt();
            sheet->operation().scaleMode = (Dr::ScaleMode)query.value("scaleMode").toInt();
            sheet->operation().rotation = (Dr::Rotation)query.value("rotation").toInt();
            sheet->operation().scaleFactor = query.value("scaleFactor").toInt();
            sheet->operation().openThumbnail = query.value("openThumbnail").toInt();
            sheet->operation().leftIndex = query.value("leftIndex").toInt();
            sheet->operation().currentPage = query.value("currentPage").toInt();
            return true;
        }
    }
    return false;
}

bool Database::saveOperation(DocSheet *sheet)
{
    if (nullptr != sheet && m_database.isOpen()) {
        QSqlQuery query(m_database);

        query.prepare(" replace into "
                      " operation(filePath,layoutMode,mouseShape,scaleMode,rotation,scaleFactor,openThumbnail,leftIndex,currentPage)"
                      " VALUES(:filePath,:layoutMode,:mouseShape,:scaleMode,:rotation,:scaleFactor,:openThumbnail,:leftIndex,currentPage)");

        query.bindValue(":filePath", sheet->filePath());
        query.bindValue(":layoutMode", sheet->operation().layoutMode);
        query.bindValue(":mouseShape", sheet->operation().mouseShape);
        query.bindValue(":scaleMode", sheet->operation().scaleMode);
        query.bindValue(":rotation", sheet->operation().rotation);
        query.bindValue(":scaleFactor", sheet->operation().scaleFactor);
        query.bindValue(":openThumbnail", sheet->operation().openThumbnail);
        query.bindValue(":leftIndex", sheet->operation().leftIndex);
        query.bindValue(":currentPage", sheet->operation().currentPage);
        return query.exec();
    }

    return false;
}

Database::Database(QObject *parent) : QObject(parent)
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    QDir().mkpath(path);

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(QDir(path).filePath("database"));
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

        if (!tables.contains("bookmarks")) {
            prepareBookmarks();
        }
    } else {
        qDebug() << m_database.lastError();
    }
}

bool Database::prepareBookmarks()
{
    Transaction transaction(m_database);

    QSqlQuery query(m_database);
    query.exec("CREATE TABLE bookmarks_v3 "
               "(filePath TEXT"
               ",page INTEGER"
               ",label TEXT"
               ",comment TEXT"
               ",modified DATETIME)");

    if (!query.isActive()) {
        qDebug() << query.lastError();
        return false;
    }

    transaction.commit();
    return true;
}
