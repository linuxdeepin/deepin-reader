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
#include "ModuleHeader.h"
#include "TransparentTextEdit.h"

#include <QHBoxLayout>
#include <DPlatformWindowHandle>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QLinearGradient>

NoteShadowViewWidget::NoteShadowViewWidget(QWidget *parent)
    : DWidget(parent)
{
    setWindowFlag(Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    initWidget();
}

void NoteShadowViewWidget::initWidget()
{
    setFixedSize(QSize(278, 344));

    QHBoxLayout *pHLayoutContant = new QHBoxLayout;
    pHLayoutContant->setMargin(12);

    m_noteViewWidget = new NoteViewWidget(this);

    pHLayoutContant->addWidget(m_noteViewWidget);
    this->setLayout(pHLayoutContant);

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(m_noteViewWidget);
    shadowEffect->setOffset(4, 4);
    shadowEffect->setColor(QColor(0, 0, 0, 100));
    shadowEffect->setBlurRadius(16);
    m_noteViewWidget->setGraphicsEffect(shadowEffect);
}

NoteViewWidget *NoteShadowViewWidget::getNoteViewWidget()
{
    return m_noteViewWidget;
}

void NoteShadowViewWidget::showWidget(const QPoint &point)
{
    move(point - QPoint(12, 12));
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
}

void NoteViewWidget::hideEvent(QHideEvent *event)
{
    //  原来是有注释的, 被删除了
    if (m_nWidgetType == NOTE_HIGHLIGHT) {
        fileNoteHideEvent();
    } else {
        pageNoteHideEvent();
    }
    CustomWidget::hideEvent(event);
}

void NoteViewWidget::initWidget()
{
    m_nWidgetType = NOTE_HIGHLIGHT;
    setFixedSize(QSize(254, 320));

    QHBoxLayout *pHLayoutContant = new QHBoxLayout;
    pHLayoutContant->setContentsMargins(20, 22, 6, 22);

    m_pTextEdit = new TransparentTextEdit(this);
    connect(m_pTextEdit, SIGNAL(sigNeedShowTips(const QString &, int)), this, SIGNAL(sigNeedShowTips(const QString &, int)));

    pHLayoutContant->addWidget(m_pTextEdit);
    this->setLayout(pHLayoutContant);
}

//  高亮注释 处理
void NoteViewWidget::fileNoteHideEvent()
{
    QString sText = m_pTextEdit->toPlainText().trimmed();
    if (m_pNoteUuid == "") {
        if (sText != "") {
            QString msgContent = sText + Constant::sQStringSep + m_pNotePage;
            emit sigNeedAddHighLightAnnotation(msgContent);
        }
    } else {
        if (sText == "" && m_strNote != "") {   //  原来有内容, 被删了, 删除高亮
            QString msgContent = m_pNoteUuid + Constant::sQStringSep + m_pNotePage;

            emit sigNoteViewMsg(MSG_NOTE_DELETE_CONTENT, msgContent);

        } else if (sText != m_strNote) {
            QString msgContent = m_pNoteUuid  + Constant::sQStringSep +
                                 sText + Constant::sQStringSep +
                                 m_pNotePage;
            emit sigNoteViewMsg(MSG_NOTE_UPDATE_CONTENT, msgContent);
        }
        m_strNote = sText;
    }
}

//  页面注释处理
void NoteViewWidget::pageNoteHideEvent()
{
    if (m_pNoteUuid != "") {
        QString sText = m_pTextEdit->toPlainText().trimmed();
        if (m_strNote == "") {
            if (sText != "") {
                QString msgContent = m_pNoteUuid  + Constant::sQStringSep +
                                     sText + Constant::sQStringSep +
                                     m_pNotePage;
                emit sigNoteViewMsg(MSG_NOTE_PAGE_ADD_CONTENT, msgContent);
            } else {
                QString sContent = m_pNoteUuid + Constant::sQStringSep + m_pNotePage + Constant::sQStringSep + "1";
                emit sigNoteViewMsg(MSG_NOTE_PAGE_DELETE_CONTENT, sContent);
            }
        } else {
            if (sText == "") {
                QString sContent = m_pNoteUuid + Constant::sQStringSep + m_pNotePage + Constant::sQStringSep + "0";
                emit sigNoteViewMsg(MSG_NOTE_PAGE_DELETE_CONTENT, sContent);
            } else if (sText != m_strNote) {  //  只有 和 原来已有注释内容不一样, 才会提示 保存
                QString msgContent = m_pNoteUuid  + Constant::sQStringSep +
                                     sText + Constant::sQStringSep +
                                     m_pNotePage;
                emit sigNoteViewMsg(MSG_NOTE_PAGE_UPDATE_CONTENT, msgContent);
            }
        }
    }
}

void NoteViewWidget::setNotePage(const QString &pNotePage)
{
    m_pNotePage = pNotePage;
}

void NoteViewWidget::setWidgetType(const int &nWidgetType)
{
    m_nWidgetType = nWidgetType;
}

void NoteViewWidget::setNoteUuid(const QString &pNoteUuid)
{
    m_pNoteUuid = pNoteUuid;
}

void NoteViewWidget::paintEvent(QPaintEvent *event)
{
    CustomWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    QPainterPath clippath;
    clippath.setFillRule(Qt::WindingFill);
    int minRadius = 16;
    int maxRadius = 32;
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
