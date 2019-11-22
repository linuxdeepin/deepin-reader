#ifndef APPSETTING_H
#define APPSETTING_H

#include <QSettings>

/**
 * @brief The AppSetting class
 * 配置项数据
 */


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
    void setKeyValue(const QString &, const QString &);
    QString getKeyValue(const QString &) const;

private:
    QSettings   *m_pSettings = nullptr;
};

#endif // APPSETTING_H
