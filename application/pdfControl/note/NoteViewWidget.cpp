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

#include <QHBoxLayout>
#include <DPlatformWindowHandle>

#include "TransparentTextEdit.h"

#include "CustomControl/CustomClickLabel.h"
#include "utils/PublicFunction.h"

NoteViewWidget::NoteViewWidget(CustomWidget *parent)
    : CustomWidget(QString("NoteViewWidget"), parent)
{
    setWindowFlag(Qt::Popup);
    setFixedSize(QSize(250, 320));
    DPlatformWindowHandle handle(this);
    int radius = 18;
    handle.setWindowRadius(radius);

    initWidget();
    initConnections();
    slotUpdateTheme();

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

NoteViewWidget::~NoteViewWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

int NoteViewWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
    return 0;
}

void NoteViewWidget::setEditText(const QString &note)
{
    if (m_pTextEdit) {
        m_pTextEdit->clear();
        //m_pTextEdit->setText(note);
        m_pTextEdit->setPlainText(note);
        m_strNote = note;
    }
}

void NoteViewWidget::showWidget(const QPoint &point)
{
    move(point);
    show();
    raise();
}

void NoteViewWidget::hideEvent(QHideEvent *event)
{
    //  原来是有注释的, 被删除了
    QString t_contant = m_pTextEdit->toPlainText().trimmed();
    if (m_strNote != "" && t_contant == "") {
        sendMsg(MSG_NOTE_DLTNOTECONTANT, m_pNoteUuid);
        m_strNote = t_contant;
    } else {
        QString t_contant = m_pTextEdit->toPlainText().trimmed();  //  注释内容
        if (t_contant != m_strNote) {  //  只有 和 原来已有注释内容不一样, 才会提示 保存
            QString msgContent = "";
            if (m_pNoteUuid != "") {  //  已经高亮
                msgContent = t_contant + Constant::sQStringSep + m_pNoteUuid +
                             Constant::sQStringSep + m_pNotePage;
            } else {
                msgContent = t_contant + Constant::sQStringSep + m_pHighLightPointAndPage;
            }
            sendMsg(MSG_NOTE_ADDCONTANT, msgContent);
        }
    }
    CustomWidget::hideEvent(event);
}

// 初始化界面
void NoteViewWidget::initWidget()
{
    QPalette plt = this->palette();
    plt.setColor(QPalette::Background, QColor(QString("#FFFBE1")));
    this->setPalette(plt);

    m_pCloseLab = new CustomClickLabel("");
    m_pCloseLab->setFixedSize(QSize(24, 24));
    connect(m_pCloseLab, SIGNAL(clicked()), this, SLOT(close()));

    auto m_pHLayoutClose = new QHBoxLayout;
    m_pHLayoutClose->setContentsMargins(224, 4, 6, 4);
    m_pHLayoutClose->addStretch(0);
    m_pHLayoutClose->addWidget(m_pCloseLab);

    auto m_pHLayoutContant = new QHBoxLayout;
    m_pHLayoutContant->setContentsMargins(25, 0, 24, 29);
    m_pHLayoutContant->addStretch(0);

    m_pTextEdit = new TransparentTextEdit(this);
    m_pHLayoutContant->addWidget(m_pTextEdit);

    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    m_pVLayout->addItem(m_pHLayoutClose);
    m_pVLayout->addItem(m_pHLayoutContant);

    this->setLayout(m_pVLayout);
}

void NoteViewWidget::initConnections()
{
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

//  主题变了
void NoteViewWidget::slotUpdateTheme()
{
    QString sClose = PF::getImagePath("close", Pri::g_icons);
    m_pCloseLab->setPixmap(Utils::renderSVG(sClose, QSize(24, 24)));
}

void NoteViewWidget::setNotePage(const QString &pNotePage)
{
    m_pNotePage = pNotePage;
}

void NoteViewWidget::setNoteUuid(const QString &pNoteUuid)
{
    m_pNoteUuid = pNoteUuid;
}

void NoteViewWidget::setPointAndPage(const QString &pointAndPage)
{
    m_pHighLightPointAndPage = pointAndPage;
}
