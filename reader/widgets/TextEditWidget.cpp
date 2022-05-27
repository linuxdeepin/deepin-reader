/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     duanxiaohui <wangzhixuan@uniontech.com>
 *
 * Maintainer: duanxiaohui <duanxiaohui@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "TextEditWidget.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "Model.h"
#include "TransparentTextEdit.h"
#include "SheetBrowser.h"
#include "Application.h"
#include "Utils.h"
#include "DBusObject.h"
#include "SheetBrowser.h"

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
    setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

    setAttribute(Qt::WA_TranslucentBackground);

    const int margin = 12;
    setFixedWidth(254 + 2 * margin);
    updateHeight();

    QHBoxLayout *pHLayoutContant = new QHBoxLayout;

    pHLayoutContant->setMargin(margin);

    this->setLayout(pHLayoutContant);

    m_TextEditWidget = new TextEditWidget(this);

    pHLayoutContant->addWidget(m_TextEditWidget);

    m_TextEditWidget->m_brower = dynamic_cast<SheetBrowser *>(parent);

    setObjectName("TextEditShadowWidget");

    connect(m_TextEditWidget, &TextEditWidget::sigCloseNoteWidget, this, &TextEditShadowWidget::slotCloseNoteWidget);
}

TextEditWidget *TextEditShadowWidget::getTextEditWidget()
{
    return m_TextEditWidget;
}

void TextEditShadowWidget::showWidget(const QPoint &point)
{
    QPoint pos = mapToParent(mapFromGlobal(point)); //先转换为相对于父对象的坐标

    //如果下边超出程序则向上移动
    if (pos.y() + this->height() > m_TextEditWidget->m_brower->height()) {
        pos.setY(pos.y() - (pos.y() + this->height() - m_TextEditWidget->m_brower->height()));
    }
    //如果右边超出程序则向左移动
    if (pos.x() + this->width() > m_TextEditWidget->m_brower->width()) {
        pos.setX(pos.x() - (pos.x() + this->width() - m_TextEditWidget->m_brower->width()));
    }

    move(pos);

    raise();

    show();

    m_TextEditWidget->setEditFocus();
}

void TextEditShadowWidget::updateHeight()
{
    //多窗口的对应不同的window
    SheetBrowser *view = this->window()->findChild<SheetBrowser *>();
    if(!view)
        return;
    int h = qMin(320, view->height());
    if(this->height() != h)
        setFixedHeight(h);
}

void TextEditShadowWidget::slotCloseNoteWidget(bool isEsc)
{
    if (isEsc) {
        close();
    } else if (nullptr != m_TextEditWidget->getTextEdit()
               && !m_TextEditWidget->getTextEdit()->hasFocus()
               && !m_TextEditWidget->hasFocus()) {
        close();
    }
}

TextEditWidget::TextEditWidget(DWidget *parent)
    : BaseWidget(parent)
{
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
}

void TextEditWidget::onShowMenu()
{
    if (this->isVisible() && m_pTextEdit) {
        QMenu *menu =  m_pTextEdit->createStandardContextMenu();
        if (menu) {
            menu->exec(QCursor::pos());
            delete  menu;
            menu = nullptr;
        }
    }
}

void TextEditWidget::setEditText(const QString &note)
{
    m_pTextEdit->clear();
    m_pTextEdit->setPlainText(note);
    m_strNote = note;

    QTextCursor cursor = m_pTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_pTextEdit->setTextCursor(cursor);
}

void TextEditWidget::setAnnotation(deepin_reader::Annotation *annotation)
{
    m_annotation = annotation;
}

void TextEditWidget::setEditFocus()
{
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
    return m_pTextEdit;
}

void TextEditWidget::hideEvent(QHideEvent *event)
{
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
}

void TextEditWidget::initWidget()
{
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
}

void TextEditWidget::onBlurWindowChanged()
{
    if (DWindowManagerHelper::instance()->hasComposite()) {
        this->graphicsEffect()->setEnabled(true);
        parentWidget()->layout()->setMargin(12);

    } else {
        this->graphicsEffect()->setEnabled(false);
        parentWidget()->layout()->setMargin(0);
    }
}

void TextEditWidget::onTouchPadEvent(QString name, QString direction, int fingers)
{
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
}

void TextEditWidget::paintEvent(QPaintEvent *event)
{
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
}

void TextEditWidget::focusOutEvent(QFocusEvent *event)
{
    BaseWidget::focusOutEvent(event);

    Q_EMIT sigCloseNoteWidget();
}
