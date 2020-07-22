#include "AppInfo.h"

#include "utils/utils.h"
#include "ModuleHeader.h"
#include <QDir>

AppInfo::AppInfo(QObject *parent)
    : QObject(parent)
{
    m_pSettings = new QSettings(QDir(Utils::getConfigPath()).filePath(ConstantMsg::g_cfg_name), QSettings::IniFormat, parent);
    InitColor();
    InitKeyList();
}

void AppInfo::InitColor()
{
    m_listColor.append(QColor("#FFA503"));
    m_listColor.append(QColor("#FF1C49"));
    m_listColor.append(QColor("#9023FC"));
    m_listColor.append(QColor("#3468FF"));
    m_listColor.append(QColor("#00C7E1"));
    m_listColor.append(QColor("#05EA6B"));
    m_listColor.append(QColor("#FEF144"));
    m_listColor.append(QColor("#D5D5D1"));
}

void AppInfo::InitKeyList()
{
    m_pKeyList.append(QKeySequence::Find);
    m_pKeyList.append(QKeySequence::Open);
    m_pKeyList.append(QKeySequence::Print);
    m_pKeyList.append(QKeySequence::Save);
    m_pKeyList.append(QKeySequence::Copy);
    m_pKeyList.append(QKeySequence(Qt::Key_F5));
    m_pKeyList.append(QKeySequence(Qt::Key_Left));
    m_pKeyList.append(QKeySequence(Qt::Key_Right));
    m_pKeyList.append(QKeySequence(Qt::Key_Space));
    m_pKeyList.append(QKeySequence(Qt::Key_Escape));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_1));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_2));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_A));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_H));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_Z));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_1));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_2));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_3));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_D));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_M));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_R));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Equal));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
}

QList<QColor> AppInfo::getLightColorList()
{
    return m_listColor;
}

QList<QKeySequence> AppInfo::getKeyList() const
{
    return m_pKeyList;
}

QRect AppInfo::screenRect() const
{
    return  m_screenRect;
}

void AppInfo::setScreenRect(const QRect &rect)
{
    m_screenRect = rect;
}

void AppInfo::setAppKeyValue(const int &iKey, const QString &sValue)
{
    // initialize the configuration file.
    QString sKey = QString("%1").arg(iKey);

    QString ssValue = QString("%1").arg(sValue);

    if (sValue == "") {
        m_pSettings->remove(sKey);
    } else {
        m_pSettings->setValue(sKey, ssValue);
    }
}

QString AppInfo::getAppKeyValue(const int &iKey) const
{
    // initialize the configuration file.
    QString sKey = QString("%1").arg(iKey);

    return m_pSettings->value(sKey).toString();
}
