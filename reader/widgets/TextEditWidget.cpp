// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TextEditWidget.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "Model.h"
#include "TransparentTextEdit.h"
#include "SheetBrowser.h"
#include "Application.h"
#include "Utils.h"
#include "DBusObject.h"
#include "ddlog.h"

#include <QDebug>

#include <DPlatformWindowHandle>
#include <DWindowManagerHelper>

#include <QHBoxLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QLinearGradient>
#include <QPainterPath>
#include <QPointF>
#include <QMenu>

TextEditShadowWidget::TextEditShadowWidget(QWidget *parent)
    : DWidget(parent)
{
    qCDebug(appLog) << "TextEditShadowWidget created, parent:" << parent;
    setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

    setAttribute(Qt::WA_TranslucentBackground);

    setMaximumSize(QSize(m_maxwidth, m_maxheight));

    QHBoxLayout *pHLayoutContant = new QHBoxLayout;

    pHLayoutContant->setContentsMargins(12, 12, 12, 12);

    this->setLayout(pHLayoutContant);

    m_TextEditWidget = new TextEditWidget(this);

    pHLayoutContant->addWidget(m_TextEditWidget);

    m_TextEditWidget->m_brower = dynamic_cast<SheetBrowser *>(parent);

    setObjectName("TextEditShadowWidget");

    connect(m_TextEditWidget, &TextEditWidget::sigCloseNoteWidget, this, &TextEditShadowWidget::slotCloseNoteWidget);
    qCDebug(appLog) << "TextEditShadowWidget initWidget end";
}

TextEditWidget *TextEditShadowWidget::getTextEditWidget()
{
    // qCDebug(appLog) << "getTextEditWidget";
    return m_TextEditWidget;
}

void TextEditShadowWidget::showWidget(const QPoint &point)
{
    qCDebug(appLog) << "showWidget";
    QPoint pos = mapToParent(mapFromGlobal(point)); //先转换为相对于父对象的坐标

    //如果下边超出程序则向上移动
    if (pos.y() + m_maxheight > m_TextEditWidget->m_brower->height()) {
        pos.setY(pos.y() - (pos.y() + m_maxheight - m_TextEditWidget->m_brower->height()));
    }
    //如果右边超出程序则向左移动
    if (pos.x() + m_maxwidth > m_TextEditWidget->m_brower->width()) {
        pos.setX(pos.x() - (pos.x() + m_maxwidth - m_TextEditWidget->m_brower->width()));
    }

    move(pos);

    raise();

    show();

    m_TextEditWidget->setEditFocus();
    qCDebug(appLog) << "showWidget end";
}


void TextEditShadowWidget::slotCloseNoteWidget(bool isEsc)
{
    qCDebug(appLog) << "slotCloseNoteWidget";
    if (isEsc) {
        qCDebug(appLog) << "slotCloseNoteWidget isEsc";
        close();
    } else if (nullptr != m_TextEditWidget->getTextEdit()
               && !m_TextEditWidget->getTextEdit()->hasFocus()
               && !m_TextEditWidget->hasFocus()) {
        qCDebug(appLog) << "slotCloseNoteWidget hasFocus";
        close();
    }
    qCDebug(appLog) << "slotCloseNoteWidget end";
}

TextEditWidget::TextEditWidget(DWidget *parent)
    : BaseWidget(parent)
{
    qCDebug(appLog) << "TextEditWidget created";
    setAttribute(Qt::WA_TranslucentBackground);

    initWidget();

    m_showMenuTimer = new  QTimer(this);
    m_showMenuTimer->setInterval(1000);
    connect(m_showMenuTimer, &QTimer::timeout, this, [ = ] {
        m_showMenuTimer->stop();
        this->onShowMenu();
    });

    connect(DBusObject::instance(), &DBusObject::sigTouchPadEventSignal, this, &TextEditWidget::onTouchPadEvent);
    connect(m_pTextEdit, &TransparentTextEdit::sigCloseNoteWidget, this, &TextEditWidget::sigCloseNoteWidget);
    qCDebug(appLog) << "TextEditWidget initWidget end";
}

void TextEditWidget::onShowMenu()
{
    qCDebug(appLog) << "onShowMenu";
    if (this->isVisible() && m_pTextEdit) {
        qCDebug(appLog) << "onShowMenu m_pTextEdit";
        QMenu *menu =  m_pTextEdit->createStandardContextMenu();
        if (menu) {
            qCDebug(appLog) << "onShowMenu menu";
            menu->exec(QCursor::pos());
            delete  menu;
            menu = nullptr;
        }
    }
    qCDebug(appLog) << "onShowMenu end";
}

void TextEditWidget::setEditText(const QString &note)
{
    qCDebug(appLog) << "Setting edit text, length:" << note.length();
    m_pTextEdit->clear();
    m_pTextEdit->setPlainText(note);
    m_strNote = note;

    QTextCursor cursor = m_pTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_pTextEdit->setTextCursor(cursor);
}

void TextEditWidget::setAnnotation(deepin_reader::Annotation *annotation)
{
    qCDebug(appLog) << "Setting annotation, type:" << (annotation ? annotation->type() : -1);
    m_annotation = annotation;
}

void TextEditWidget::setEditFocus()
{
    // qCDebug(appLog) << "setEditFocus";
    /**
     * fixbug:111746
     * wayland上，立即将焦点设置到编辑框内，但是此时会偶现激活窗口，这样焦点就离开了编辑框，触发关闭注释弹框的关闭逻辑
     * 延时100ms将焦点设置到编辑框内，此时已经出现偶现的激活窗口，这时就保证了焦点设在编辑框内，不会触发关闭注释弹框的关闭逻辑
     */
    QEventLoop loop;
    QTimer::singleShot(100, &loop, &QEventLoop::quit);
    loop.exec();

    m_pTextEdit->setFocus();
}

TransparentTextEdit *TextEditWidget::getTextEdit() const
{
    // qCDebug(appLog) << "getTextEdit";
    return m_pTextEdit;
}

void TextEditWidget::hideEvent(QHideEvent *event)
{
    // qCDebug(appLog) << "TextEditWidget hiding";
    BaseWidget::hideEvent(event);
    QString sText = m_pTextEdit->toPlainText().trimmed();

    if (m_annotation == nullptr)
        return;

    if (m_annotation->type() == deepin_reader::Annotation::AText &&  sText.isEmpty() && !event->spontaneous()) {
        emit sigRemoveAnnotation(m_annotation, !m_annotation->contents().isEmpty());
    } else if (m_annotation->contents() != sText) {
        emit sigUpdateAnnotation(m_annotation, sText);
    }

    emit sigHide();
    // qCDebug(appLog) << "TextEditWidget hiding end";
}

void TextEditWidget::initWidget()
{
    qCDebug(appLog) << "TextEditWidget initWidget";
    setFixedSize(QSize(254, 320));
    setMinimumHeight(310);
    setMaximumHeight(320);

    QHBoxLayout *pHLayoutContant = new QHBoxLayout;
    pHLayoutContant->setContentsMargins(20, 20, 6, 20);

    m_pTextEdit = new TransparentTextEdit(this);
    connect(m_pTextEdit, SIGNAL(sigNeedShowTips(const QString &, int)), this, SIGNAL(sigNeedShowTips(const QString &, int)));

    pHLayoutContant->addWidget(m_pTextEdit);
    this->setLayout(pHLayoutContant);

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(4, 4);
    shadowEffect->setColor(QColor(0, 0, 0, 100));
    shadowEffect->setBlurRadius(16);
    setGraphicsEffect(shadowEffect);

    onBlurWindowChanged();
    QObject::connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasCompositeChanged, this, &TextEditWidget::onBlurWindowChanged);
    qCDebug(appLog) << "TextEditWidget initWidget end";
}

void TextEditWidget::onBlurWindowChanged()
{
    qCDebug(appLog) << "onBlurWindowChanged";
    if (DWindowManagerHelper::instance()->hasComposite()) {
        qCDebug(appLog) << "onBlurWindowChanged hasComposite";
        this->graphicsEffect()->setEnabled(true);
        parentWidget()->layout()->setContentsMargins(12, 12, 12, 12);

    } else {
        qCDebug(appLog) << "onBlurWindowChanged no composite";
        this->graphicsEffect()->setEnabled(false);
        parentWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    }
    qCDebug(appLog) << "onBlurWindowChanged end";
}

void TextEditWidget::onTouchPadEvent(QString name, QString direction, int fingers)
{
    qCDebug(appLog) << "onTouchPadEvent";
    Q_UNUSED(name)

    if (fingers == 0) {
        if (direction == "up") {
            m_showMenuTimer->stop();
        } else if (direction == "down") {
            if (!m_showMenuTimer->isActive()) {
                m_showMenuTimer->start();
            }
        }
    }
    qCDebug(appLog) << "onTouchPadEvent end";
}

void TextEditWidget::paintEvent(QPaintEvent *event)
{
    // qCDebug(appLog) << "TextEditWidget paintEvent";
    BaseWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    QPainterPath clippath;
    clippath.setFillRule(Qt::WindingFill);
    int minRadius = 0;
    int maxRadius = 0;
    if (DWindowManagerHelper::instance()->hasComposite()) {
        minRadius = 16;
        maxRadius = 32;
    }
    clippath.moveTo(minRadius, 0);
    clippath.lineTo(this->width() - minRadius, 0);
    clippath.arcTo(this->width() - minRadius * 2, 0, minRadius * 2, minRadius * 2, 90, -90);
    clippath.lineTo(this->width(), this->height() - minRadius * 2);
    clippath.arcTo(this->width() - maxRadius * 2, this->height() - maxRadius * 2, maxRadius * 2, maxRadius * 2, 0, -90);
    clippath.lineTo(minRadius, this->height());
    clippath.arcTo(0, this->height() - minRadius * 2, minRadius * 2, minRadius * 2, 270, -90);
    clippath.lineTo(0, minRadius);
    clippath.arcTo(0, 0, minRadius * 2, minRadius * 2, 180, -90);
    clippath.closeSubpath();
    painter.setClipPath(clippath);

    QLinearGradient linearGrad(QPointF(0, 0), QPointF(this->width(), this->height()));
    linearGrad.setColorAt(0, QColor(255, 244, 196));
    linearGrad.setColorAt(0.48, QColor(255, 238, 189));
    linearGrad.setColorAt(0.90, QColor(255, 229, 161));
    linearGrad.setColorAt(0.96, QColor(255, 251, 225));
    linearGrad.setColorAt(1, QColor(255, 251, 225));
    painter.setBrush(linearGrad);
    painter.setPen(Qt::NoPen);
    painter.drawRect(this->rect());

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColor(0, 0, 0, 51));
    painter.drawPath(clippath);
    // qCDebug(appLog) << "TextEditWidget paintEvent end";
}

void TextEditWidget::focusOutEvent(QFocusEvent *event)
{
    // qCDebug(appLog) << "TextEditWidget focus out, reason:" << event->reason();
    BaseWidget::focusOutEvent(event);

    Q_EMIT sigCloseNoteWidget();
}
