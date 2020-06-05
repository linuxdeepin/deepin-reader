#ifndef JSON_H
#define JSON_H

#include <QJsonObject>
#include <QJsonDocument>

struct JsonPrivate;
class Json
{
public:
    explicit Json(const QString &jsonOrJsonFilePath = "{}", bool fromFile = false);
    ~Json();
    int         getInt(const QString &path, int def = 0, const QJsonObject &fromNode = QJsonObject()) const;
    bool        getBool(const QString &path, bool def = false, const QJsonObject &fromNode = QJsonObject()) const;
    double      getDouble(const QString &path, double def = 0.0, const QJsonObject &fromNode = QJsonObject()) const;
    QString     getString(const QString &path, const QString &def = QString(), const QJsonObject &fromNode = QJsonObject()) const;
    QStringList getStringList(const QString &path, const QJsonObject &fromNode = QJsonObject()) const;
    QJsonArray  getJsonArray(const QString &path, const QJsonObject &fromNode = QJsonObject()) const;
    QJsonValue  getJsonValue(const QString &path, const QJsonObject &fromNode = QJsonObject()) const;
    QJsonObject getJsonObject(const QString &path, const QJsonObject &fromNode = QJsonObject()) const;
    void set(const QString &path, const QJsonValue &value);
    void set(const QString &path, const QStringList &strings);
    void save(const QString &path, QJsonDocument::JsonFormat format = QJsonDocument::Indented);
    QString toString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
public:
    JsonPrivate *d;
};
#endif // JSON_H
