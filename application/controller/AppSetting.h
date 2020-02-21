#ifndef APPSETTING_H
#define APPSETTING_H

#include <QSettings>
#include "application.h"

/**
 * @brief The AppSetting class
 * 配置项数据
 */

enum KeyType {
    KEY_DIR,
//    KEY_PAGENUM,
//    KEY_M,
//    KEY_WIDGET,
//    KEY_PERCENTAGE,
//    KEY_DOUBPAGE,
//    KEY_ADAPTAT,
//    KEY_ROTATE,
    KEY_APP_WIDTH,
    KEY_APP_HEIGHT
};

class AppSetting : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AppSetting)

private:
    explicit AppSetting(QObject *parent = nullptr);

public:
    static AppSetting *instance()
    {
        static AppSetting app;
        return &app;
    }

public:
    void setFileKeyValue(const QString &);
//    QString getFileKeyValue() const;

//    void setKeyValue(const int &, const QString &);
    QString getKeyValue(const int &) const;

    void setAppKeyValue(const int &, const QString &);
    QString getAppKeyValue(const int &) const;

private:
    QSettings *m_pSettings = nullptr;
};

#endif  // APPSETTING_H
