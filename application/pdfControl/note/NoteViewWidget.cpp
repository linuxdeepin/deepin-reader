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

#include "business/FileDataManager.h"
#include "CustomControl/CustomClickLabel.h"
#include "utils/PublicFunction.h"
#include "docview/docummentproxy.h"

#include "business/bridge/IHelper.h"

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

    dApp->m_pModelService->addObserver(this);
}

NoteViewWidget::~NoteViewWidget()
{
    dApp->m_pModelService->removeObserver(this);
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
    if (m_nWidgetType == NOTE_HIGHLIGHT) {
        __FileNoteHideEvent();
    } else {
        __PageNoteHideEvent();
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

//  高亮注释 处理
void NoteViewWidget::__FileNoteHideEvent()
{
    QString sText = m_pTextEdit->toPlainText().trimmed();
    if (m_pNoteUuid == "") {
        if (sText != "") {
            QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
            QString msgContent = sCurPath + Constant::sQStringSep + sText + Constant::sQStringSep + m_pNotePage;
            sendMsg(MSG_NOTE_ADD_CONTENT, msgContent);
        }
    } else {
        if (sText == "" && m_strNote != "") {   //  原来有内容, 被删了, 删除高亮
            QString msgContent = m_pNoteUuid + Constant::sQStringSep + m_pNotePage;
            dApp->m_pHelper->qDealWithData(MSG_NOTE_DELETE_CONTENT, msgContent);
        } else if (sText != m_strNote) {
            QString msgContent = sText + Constant::sQStringSep +
                                 m_pNoteUuid + Constant::sQStringSep +
                                 m_pNotePage;
            dApp->m_pHelper->qDealWithData(MSG_NOTE_UPDATE_CONTENT, msgContent);
        }
        m_strNote = sText;
    }
}


//  页面注释处理
void NoteViewWidget::__PageNoteHideEvent()
{
    if (m_pNoteUuid != "") {
        QString sText = m_pTextEdit->toPlainText().trimmed();
        if (m_strNote == "") {
            if (sText != "") {
                QString msgContent = sText + Constant::sQStringSep +
                                     m_pNoteUuid + Constant::sQStringSep +
                                     m_pNotePage;

                dApp->m_pHelper->qDealWithData(MSG_NOTE_PAGE_ADD_CONTENT, msgContent);
            } else {
                auto pHelper = DocummentProxy::instance();
                if (pHelper) {
                    pHelper->removeAnnotation(m_pNoteUuid);
                }
            }
        } else {
            if (sText == "") {
                QString msgContent = m_pNoteUuid + Constant::sQStringSep + m_pNotePage;
                dApp->m_pHelper->qDealWithData(MSG_NOTE_PAGE_DELETE_CONTENT, msgContent);
            } else if (sText != m_strNote) {  //  只有 和 原来已有注释内容不一样, 才会提示 保存
                QString msgContent = sText + Constant::sQStringSep +
                                     m_pNoteUuid + Constant::sQStringSep +
                                     m_pNotePage;
                dApp->m_pHelper->qDealWithData(MSG_NOTE_PAGE_UPDATE_CONTENT, msgContent);
            }
        }
    }
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

void NoteViewWidget::setWidgetType(const int &nWidgetType)
{
    m_nWidgetType = nWidgetType;
}

void NoteViewWidget::setNoteUuid(const QString &pNoteUuid)
{
    m_pNoteUuid = pNoteUuid;
}
