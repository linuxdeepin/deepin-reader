// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EyeProtectionManager.h"
#include "ddlog.h"

#include <QSettings>
#include <QDir>
#include <QStandardPaths>

// 各模式颜色映射表
const EyeProtectionManager::ModeColors EyeProtectionManager::s_modeColors[4] = {
    // Off:       页面背景       视口背景       前景色
    { QColor("#FFFFFF"), QColor("#E0E0E0"), QColor("#000000") },
    // Classic:
    { QColor("#F7F3E8"), QColor("#DEDAD0"), QColor("#333333") },
    // Green:
    { QColor("#C7EDCC"), QColor("#B3D5B7"), QColor("#333333") },
    // Night:
    { QColor("#1E1E1E"), QColor("#252525"), QColor("#E0E0E0") },
};

EyeProtectionManager *EyeProtectionManager::instance()
{
    static EyeProtectionManager s_instance;
    return &s_instance;
}

EyeProtectionManager::EyeProtectionManager(QObject *parent)
    : QObject(parent)
    , m_mode(Off)
{
    loadMode();
}

void EyeProtectionManager::setMode(Mode mode)
{
    if (m_mode == mode)
        return;

    qCInfo(appLog) << "Eye protection mode changed from" << m_mode << "to" << mode;
    m_mode = mode;
    saveMode();
    emit modeChanged(m_mode);
}

EyeProtectionManager::Mode EyeProtectionManager::mode() const
{
    return m_mode;
}

EyeProtectionManager::NightImagePolicy EyeProtectionManager::nightImagePolicy() const
{
    return m_nightImagePolicy;
}

qreal EyeProtectionManager::nightImageDimFactor() const
{
    return m_nightImageDimFactor;
}

QColor EyeProtectionManager::pageBackgroundColor() const
{
    return pageBackgroundColor(m_mode);
}

QColor EyeProtectionManager::viewportBackgroundColor() const
{
    return viewportBackgroundColor(m_mode);
}

QColor EyeProtectionManager::foregroundColor() const
{
    int idx = qBound(0, static_cast<int>(m_mode), 3);
    return s_modeColors[idx].foreground;
}

QColor EyeProtectionManager::pageBackgroundColor(Mode mode)
{
    int idx = qBound(0, static_cast<int>(mode), 3);
    return s_modeColors[idx].pageBg;
}

QColor EyeProtectionManager::viewportBackgroundColor(Mode mode)
{
    int idx = qBound(0, static_cast<int>(mode), 3);
    return s_modeColors[idx].viewportBg;
}

void EyeProtectionManager::loadMode()
{
    QSettings settings(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.conf"),
                       QSettings::IniFormat);
    int value = settings.value("EyeProtectionMode", 0).toInt();
    m_mode = (value >= Off && value <= Night) ? static_cast<Mode>(value) : Off;
    qCDebug(appLog) << "Loaded eye protection mode:" << m_mode;

    // 夜间图片策略(实验):默认调暗;环境变量可覆盖便于 A/B 实验
    int policy = settings.value("EyeProtection/NightImagePolicy", NightImageDim).toInt();
    m_nightImagePolicy = (policy >= NightImageDim && policy <= NightImageOriginal)
            ? static_cast<NightImagePolicy>(policy) : NightImageDim;

    const QByteArray envPolicy = qgetenv("DEEPIN_READER_NIGHT_IMAGE_POLICY");
    if (envPolicy == "invert")
        m_nightImagePolicy = NightImageInvert;
    else if (envPolicy == "original")
        m_nightImagePolicy = NightImageOriginal;
    else if (envPolicy == "dim")
        m_nightImagePolicy = NightImageDim;

    m_nightImageDimFactor = settings.value("EyeProtection/NightImageDimFactor", 1.0).toReal();
    bool dimOk = false;
    const QByteArray envDim = qgetenv("DEEPIN_READER_NIGHT_IMAGE_DIM");
    const qreal envDimValue = QString::fromLocal8Bit(envDim).toDouble(&dimOk);
    if (dimOk && envDimValue > 0.0 && envDimValue <= 1.0)
        m_nightImageDimFactor = envDimValue;
    m_nightImageDimFactor = qBound(0.1, m_nightImageDimFactor, 1.0);

    qCDebug(appLog) << "Night image policy:" << m_nightImagePolicy << "dim factor:" << m_nightImageDimFactor;
}

void EyeProtectionManager::saveMode()
{
    QSettings settings(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("config.conf"),
                       QSettings::IniFormat);
    settings.setValue("EyeProtectionMode", static_cast<int>(m_mode));
    qCDebug(appLog) << "Saved eye protection mode:" << m_mode;
}
