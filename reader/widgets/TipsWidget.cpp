// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TipsWidget.h"
#include "ddlog.h"

#include <QDebug>

#include <DPlatformWindowHandle>
#include <DGuiApplicationHelper>
#include <DFontSizeManager>
#include <DApplication>

#include <QPainter>
#include <QRectF>

TipsWidget::TipsWidget(QWidget *parent) : DWidget(parent)
{
    qCDebug(appLog) << "TipsWidget created, parent:" << parent;
    m_parent = parent;
    setWindowFlags(Qt::ToolTip);
    initWidget();
    onUpdateTheme();
}

void TipsWidget::initWidget()
{
    qCDebug(appLog) << "TipsWidget initWidget";
    m_tbMargin = 8;
    m_lrMargin = 10;
    m_maxLineCount = 10;
    m_alignment = Qt::AlignLeft | Qt::AlignVCenter;
    setFixedWidth(254);
    setWindowOpacity(0.97);
    DPlatformWindowHandle handle(this);
    handle.setWindowRadius(8);

    DFontSizeManager::instance()->bind(this, DFontSizeManager::T8);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &TipsWidget::onUpdateTheme);

    m_timer.setInterval(300);
    connect(&m_timer, &QTimer::timeout, this, &TipsWidget::onTimeOut);
}

void TipsWidget::onUpdateTheme()
{
    // qCDebug(appLog) << "Updating tips widget theme";
    DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Window, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
}

void TipsWidget::setText(const QString &text)
{
    // qCDebug(appLog) << "Setting tips text, length:" << text.length();
    m_text = text;
    m_text.replace(QChar('\n'), QString(""));
    m_text.replace(QChar('\t'), QString(""));
    update();
}

void TipsWidget::setAlignment(Qt::Alignment alignment)
{
    // qCDebug(appLog) << "Setting tips alignment";
    m_alignment = alignment;
    update();
}

void TipsWidget::setLeftRightMargin(int margin)
{
    // qCDebug(appLog) << "Setting tips left right margin";
    m_lrMargin = margin;
    update();
}

void TipsWidget::setTopBottomMargin(int margin)
{
    // qCDebug(appLog) << "Setting tips top bottom margin";
    m_tbMargin = margin;
    update();
}

void TipsWidget::setMaxLineCount(int maxLinecnt)
{
    // qCDebug(appLog) << "Setting tips max line count";
    m_maxLineCount = maxLinecnt;
    update();
}

void TipsWidget::paintEvent(QPaintEvent *event)
{
    // qCDebug(appLog) << "TipsWidget paintEvent";
    DWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    QTextOption option;
    const QString &text = m_text;
    adjustContent(text);
    option.setAlignment(m_alignment);
    option.setWrapMode(QTextOption::WrapAnywhere);
    painter.setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().text().color()));
    painter.drawText(QRect(m_lrMargin, m_tbMargin, this->width() -  2 * m_lrMargin, this->height() -  2 * m_tbMargin), text, option);
}

void TipsWidget::adjustContent(const QString &text)
{
    // qCDebug(appLog) << "Adjusting tips content";
    QFontMetricsF fontMetris(this->font());
    int wordHeight = static_cast<int>(fontMetris.boundingRect(QRectF(0, 0, this->width() - 2 * m_lrMargin, 0),
                                                              static_cast<int>(m_alignment | Qt::TextWrapAnywhere), text).height() + 2 * m_tbMargin);
    if (this->height() == wordHeight) return;
    setFixedHeight(wordHeight);
}

void TipsWidget::showEvent(QShowEvent *event)
{
    // qCDebug(appLog) << "TipsWidget showing, autoChecked:" << m_autoChecked;
    DWidget::showEvent(event);
    if (m_autoChecked)
        m_timer.start();
}

void TipsWidget::hideEvent(QHideEvent *event)
{
    // qCDebug(appLog) << "TipsWidget hiding";
    DWidget::hideEvent(event);
    m_timer.stop();
}

void TipsWidget::setAutoChecked(bool autoChecked)
{
    // qCDebug(appLog) << "Setting tips auto checked";
    m_autoChecked = autoChecked;
}

void TipsWidget::onTimeOut()
{
    // qCDebug(appLog) << "TipsWidget timeout check, visible:" << isVisible();
    if (this->isVisible() && ((m_parent && !m_parent->rect().contains(m_parent->mapFromGlobal(QCursor::pos()))) || DApplication::widgetAt(QCursor::pos()) == nullptr)) {
        this->hide();
    }
}
