/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#include "NoteViewWidget.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "document/Model.h"
#include "TransparentTextEdit.h"
#include "browser/SheetBrowser.h"

#include <QHBoxLayout>
#include <DPlatformWindowHandle>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QLinearGradient>
#include <DWindowManagerHelper>

NoteShadowViewWidget::NoteShadowViewWidget(QWidget *parent)
    : DWidget(nullptr)
{
    setWindowFlag(Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    initWidget();
    m_noteViewWidget->m_brower = dynamic_cast<SheetBrowser *>(parent);
}

void NoteShadowViewWidget::initWidget()
{
    setMaximumSize(QSize(278, 344));

    QHBoxLayout *pHLayoutContant = new QHBoxLayout;
    pHLayoutContant->setMargin(12);
    this->setLayout(pHLayoutContant);

    m_noteViewWidget = new NoteViewWidget(this);
    pHLayoutContant->addWidget(m_noteViewWidget);
}

NoteViewWidget *NoteShadowViewWidget::getNoteViewWidget()
{
    return m_noteViewWidget;
}

void NoteShadowViewWidget::showWidget(const QPoint &point)
{
    move(point - QPoint(this->layout()->margin(), this->layout()->margin()));
    raise();
    show();
}


NoteViewWidget::NoteViewWidget(DWidget *parent)
    : CustomWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    initWidget();
}

NoteViewWidget::~NoteViewWidget()
{

}

void NoteViewWidget::setEditText(const QString &note)
{
    m_pTextEdit->clear();
    m_pTextEdit->setPlainText(note);
    m_strNote = note;

    QTextCursor cursor = m_pTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_pTextEdit->setTextCursor(cursor);
}

void NoteViewWidget::setAnnotation(deepin_reader::Annotation *annotation)
{
    m_annotation = annotation;
}

void NoteViewWidget::hideEvent(QHideEvent *event)
{
    CustomWidget::hideEvent(event);
    QString sText = m_pTextEdit->toPlainText().trimmed();

    if (m_annotation->type() == 1/*Text*/ &&  sText.isEmpty()) {
        emit sigRemoveAnnotation(m_annotation, !m_annotation->contents().isEmpty());
    } else if (m_annotation->contents() != sText) {
        emit sigUpdateAnnotation(m_annotation, sText);
    }

    emit sigHide();
}

void NoteViewWidget::initWidget()
{
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
    QObject::connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasCompositeChanged, this, &NoteViewWidget::onBlurWindowChanged);
}

void NoteViewWidget::onBlurWindowChanged()
{
    if (DWindowManagerHelper::instance()->hasComposite()) {
        this->graphicsEffect()->setEnabled(true);
        parentWidget()->layout()->setMargin(12);

    } else {
        this->graphicsEffect()->setEnabled(false);
        parentWidget()->layout()->setMargin(0);
    }
}

void NoteViewWidget::paintEvent(QPaintEvent *event)
{
    CustomWidget::paintEvent(event);
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
