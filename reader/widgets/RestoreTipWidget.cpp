// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RestoreTipWidget.h"
#include "Application.h"
#include "ddlog.h"

#include <DGuiApplicationHelper>
#include <DFontSizeManager>
#include <DApplication>

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QFontMetricsF>
#include <QResizeEvent>

DGUI_USE_NAMESPACE

RestoreTipWidget::RestoreTipWidget(QWidget *parent)
    : DWidget(parent)
{
    qCDebug(appLog) << "RestoreTipWidget created";
    initUI();

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
            this, &RestoreTipWidget::onUpdateTheme);
    onUpdateTheme();
}

void RestoreTipWidget::initUI()
{
    m_text = tr("Restored to last reading position");

    DFontSizeManager::instance()->bind(this, DFontSizeManager::T8);

    connect(dApp, &DApplication::fontChanged, this, &RestoreTipWidget::onFontChanged);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(m_lrMargin, m_tbMargin, m_lrMargin, m_tbMargin);
    layout->setSpacing(m_spacing);

    m_alertIcon = DDciIcon(QStringLiteral(":/icons/deepin/builtin/icons/dr_alert_16px.dci"));
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(16, 16);
    layout->addWidget(m_iconLabel);

    QLabel *textLabel = new QLabel(m_text, this);
    textLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    layout->addWidget(textLabel);

    layout->addStretch(1);

    m_jumpBtn = new DPushButton(tr("Jump to first page"), this);
    m_jumpBtn->setFixedHeight(28);
    m_jumpBtn->setFocusPolicy(Qt::NoFocus);
    DFontSizeManager::instance()->bind(m_jumpBtn, DFontSizeManager::T8);
    connect(m_jumpBtn, &DPushButton::clicked, this, [this]() {
        qCDebug(appLog) << "Restore tip: jump to first page clicked";
        hide();
        emit sigJumpToFirstPage();
    });
    layout->addWidget(m_jumpBtn);

    DDciIcon closeIcon(QStringLiteral(":/icons/deepin/builtin/icons/dr_close_16px.dci"));
    m_closeBtn = new DIconButton(closeIcon, this);
    m_closeBtn->setFixedSize(16, 16);
    m_closeBtn->setIconSize(QSize(16, 16));
    m_closeBtn->setFlat(true);
    m_closeBtn->setFocusPolicy(Qt::NoFocus);
    m_closeBtn->setToolTip(tr("Close"));
    connect(m_closeBtn, &DIconButton::clicked, this, [this]() {
        qCDebug(appLog) << "Restore tip: close clicked";
        hide();
    });
    layout->addWidget(m_closeBtn);
    setLayout(layout);
    adjustSize();
    hide();
}

void RestoreTipWidget::adjustSize()
{
    QFontMetricsF fm(this->font());
    qreal textHeight = fm.height();
    int h = static_cast<int>(qMax(textHeight + 2 * m_tbMargin, static_cast<qreal>(28 + 2 * m_tbMargin)));
    setFixedHeight(h);
    setMinimumWidth(336);
}

void RestoreTipWidget::showTip()
{
    qCDebug(appLog) << "Showing restore tip";
    adjustSize();
    reposition();
    show();
    raise();
}

void RestoreTipWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_backgroundColor);
    painter.drawRoundedRect(rect(), 12, 12);

    QColor borderColor(Qt::black);
    borderColor.setAlphaF(0.1);
    QPen borderPen(borderColor, 1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 12, 12);
}

void RestoreTipWidget::resizeEvent(QResizeEvent *event)
{
    DWidget::resizeEvent(event);
    if (isVisible())
        reposition();
}

void RestoreTipWidget::reposition()
{
    QWidget *p = parentWidget();
    if (!p)
        return;

    int tipW = width();
    int tipH = height();
    int margin = 10;
    int x = (p->width() - tipW) / 2;
    int y = p->height() - tipH - margin;
    move(x, y);
}

void RestoreTipWidget::onFontChanged()
{
    qCDebug(appLog) << "Restore tip font changed, readjusting";
    adjustSize();
    update();
}

void RestoreTipWidget::onUpdateTheme()
{
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::DarkType) {
        m_backgroundColor = QColor(45, 45, 45);
        m_textColor = QColor(200, 200, 200);
    } else {
        m_backgroundColor = QColor(248, 248, 248);
        m_textColor = QColor(80, 80, 80);
    }
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, m_textColor);
    pal.setColor(QPalette::Text, m_textColor);
    pal.setColor(QPalette::ButtonText, m_textColor);
    setPalette(pal);

    refreshTipIcon();
    update();
}

void RestoreTipWidget::refreshTipIcon()
{
    if (m_iconLabel == nullptr || m_alertIcon.isNull())
        return;

    DDciIcon::Theme theme = (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType)
                            ? DDciIcon::Dark : DDciIcon::Light;
    m_iconLabel->setPixmap(m_alertIcon.pixmap(qApp->devicePixelRatio(), 16, theme, DDciIcon::Normal));
}
