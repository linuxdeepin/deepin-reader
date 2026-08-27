// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EyeProtectionAction.h"
#include "EyeProtectionManager.h"
#include "RoundColorWidget.h"
#include "ddlog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCoreApplication>
#include <QPalette>
#include <DLabel>
#include <DPalette>

// 各模式名称（tooltip）
static const char *s_modeNames[] = {
    QT_TRANSLATE_NOOP("EyeProtection", "No eye protection"),
    QT_TRANSLATE_NOOP("EyeProtection", "Classic eye protection"),
    QT_TRANSLATE_NOOP("EyeProtection", "Green eye protection"),
    QT_TRANSLATE_NOOP("EyeProtection", "Night eye protection")
};

EyeProtectionAction::EyeProtectionAction(DWidget *parent)
    : QWidgetAction(parent)
{
    qCDebug(appLog) << "EyeProtectionAction created";
    initWidget(parent);
    setSeparator(true);

    // 监听模式变化，同步选中态
    connect(EyeProtectionManager::instance(), &EyeProtectionManager::modeChanged,
            this, &EyeProtectionAction::onModeChanged);
}

void EyeProtectionAction::initWidget(DWidget *parent)
{
    DWidget *pWidget = new DWidget(parent);
    setDefaultWidget(pWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(32, 6, 20, 6);
    mainLayout->setSpacing(4);

    // 第一行：标题
    DLabel *titleLabel = new DLabel(tr("Eye protection mode"), pWidget);
    titleLabel->setForegroundRole(DPalette::TextTitle);
    mainLayout->addWidget(titleLabel);

    // 第二行：4个圆形选择控件
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 2, 0, 2);
    buttonLayout->setSpacing(6);

    EyeProtectionManager::Mode curMode = EyeProtectionManager::instance()->mode();

    for (int i = 0; i < 4; ++i) {
        QColor color = EyeProtectionManager::pageBackgroundColor(
            static_cast<EyeProtectionManager::Mode>(i));
        auto btn = new RoundColorWidget(color, pWidget);
        btn->setAllClickNotify(true);
        btn->setObjectName(QString("eye_%1").arg(i));
        btn->setFixedSize(QSize(25, 25));
        btn->setToolTip(QCoreApplication::translate("EyeProtection", s_modeNames[i]));
        btn->setSelected(static_cast<int>(curMode) == i);
        m_buttons[i] = btn;

        // 连接点击信号
        connect(btn, &RoundColorWidget::clicked, this, [this, i]() {
            onBtnClicked(i);
        });

        buttonLayout->addWidget(btn);
    }

    buttonLayout->addStretch(1);

    mainLayout->addItem(buttonLayout);
    pWidget->setLayout(mainLayout);
}

void EyeProtectionAction::onModeChanged(int mode)
{
    qCDebug(appLog) << "EyeProtectionAction: updating selection to mode" << mode;
    for (int i = 0; i < 4; ++i) {
        if (m_buttons[i]) {
            m_buttons[i]->setSelected(i == mode);
        }
    }
}

void EyeProtectionAction::onBtnClicked(int index)
{
    qCDebug(appLog) << "Eye protection button clicked, mode:" << index;
    EyeProtectionManager::instance()->setMode(static_cast<EyeProtectionManager::Mode>(index));
}
