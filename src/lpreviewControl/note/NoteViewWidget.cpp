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
//#include "utils/PublicFunction.h"
#include "DocSheet.h"

NoteViewWidget::NoteViewWidget(DWidget *parent)
    : CustomWidget(parent)
{

    m_nWidgetType = NOTE_HIGHLIGHT;

    setWindowFlag(Qt::Popup);
    int tW = 250;
    int tH = 320;

    setFixedSize(QSize(tW, tH));
    DPlatformWindowHandle handle(this);
    int radius = 18;
    handle.setWindowRadius(radius);

    initWidget();
    initConnections();
    slotUpdateTheme();

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &NoteViewWidget::slotUpdateTheme);
}

NoteViewWidget::~NoteViewWidget()
{

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
    int tW = 24;
    int tH = 24;
    m_nWidgetType = NOTE_HIGHLIGHT;

    m_pCloseLab->setFixedSize(QSize(tW, tH));
    connect(m_pCloseLab, SIGNAL(clicked()), this, SLOT(close()));

    auto m_pHLayoutClose = new QHBoxLayout;
    m_pHLayoutClose->setContentsMargins(224, 4, 6, 4);
    m_pHLayoutClose->addStretch(0);
    m_pHLayoutClose->addWidget(m_pCloseLab);

    auto m_pHLayoutContant = new QHBoxLayout;
    m_pHLayoutContant->setContentsMargins(25, 0, 24, 29);
    m_pHLayoutContant->addStretch(0);

    m_pTextEdit = new TransparentTextEdit(this);
    connect(m_pTextEdit, SIGNAL(sigNeedShowTips(const QString &, int)), this, SIGNAL(sigNeedShowTips(const QString &, int)));

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
}

//  高亮注释 处理
void NoteViewWidget::__FileNoteHideEvent()
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
void NoteViewWidget::__PageNoteHideEvent()
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

//  主题变了
void NoteViewWidget::slotUpdateTheme()
{
//    QString sClose = PF::getImagePath("close", Pri::g_icons);
    QIcon closeIcon = /*PF::getIcon*/QIcon::fromTheme(Pri::g_module + "close");
    int tW = 24;
    int tH = 24;

    m_pCloseLab->setPixmap(closeIcon.pixmap(QSize(tW, tH)));//Utils::renderSVG(sClose, QSize(tW, tH)));
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
