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
#include "NotesItemWidget.h"

#include <DApplication>
#include <DApplicationHelper>
#include <QClipboard>
#include <QTextLayout>
#include <QVBoxLayout>

#include "gof/bridge/IHelper.h"

NotesItemWidget::NotesItemWidget(DWidget *parent)
    : CustomItemWidget(QString("NotesItemWidget"), parent)
{
    initWidget();
    __InitConnections();
}

void NotesItemWidget::setTextEditText(const QString &contant)
{
    m_strNote = contant;
    if (m_pTextLab) {
        m_pTextLab->clear();

        QString note = m_strNote;

        note.replace(QChar('\n'), QString(""));
        note.replace(QChar('\t'), QString(""));
        m_pTextLab->setText(calcText(m_pTextLab->font(), note, m_pTextLab->size()));
    }
}

bool NotesItemWidget::bSelect()
{
    if (m_pPicture) {
        return m_pPicture->bSelect();
    }
    return false;
}

void NotesItemWidget::setBSelect(const bool &paint)
{
    if (m_pPicture) {
        m_pPicture->setSelect(paint);
    }
    m_bPaint = paint;
    update();
}

void NotesItemWidget::slotDltNoteContant()
{
    QString sContent = m_strUUid + Constant::sQStringSep + m_strPage;

    QString sRes = "";
    if (m_nNoteType == NOTE_HIGHLIGHT) {
        sRes = dApp->m_pHelper->qDealWithData(MSG_NOTE_DELETE_CONTENT, sContent);
    } else {
        sRes = dApp->m_pHelper->qDealWithData(MSG_NOTE_PAGE_DELETE_CONTENT, sContent);
    }
    if (sRes != "") {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(sRes.toLocal8Bit().data(), &error);
        if (error.error == QJsonParseError::NoError) {
            QJsonObject obj = doc.object();
            int nReturn = obj.value("return").toInt();
            if (nReturn == MSG_OK) {

                QJsonObject notifyObj;
                notifyObj.insert("content", m_strUUid);
                notifyObj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + NOTE_WIDGET);

                QJsonDocument notifyDoc(notifyObj);

                notifyMsg(MSG_NOTE_DELETE_ITEM, notifyDoc.toJson(QJsonDocument::Compact));
            }
        }
    }
}

void NotesItemWidget::slotCopyContant()
{
    if (m_pTextLab) {
        QString str = m_pTextLab->text();
        if (str != QString("")) {
            QClipboard *clipboard = DApplication::clipboard();  //获取系统剪贴板指针
            clipboard->setText(str);
        }
    }
}

void NotesItemWidget::slotShowContextMenu(const QPoint &)
{
    emit sigSelectItem(m_strUUid);

    if (m_menu == nullptr) {
        m_menu = new DMenu(this);
        DFontSizeManager::instance()->bind(m_menu, DFontSizeManager::T6);

        QAction *copyAction = m_menu->addAction(tr("Copy"));
        connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopyContant()));
        m_menu->addSeparator();

        QAction *dltItemAction = m_menu->addAction(tr("Remove annotation"));
        connect(dltItemAction, SIGNAL(triggered()), this, SLOT(slotDltNoteContant()));
    }

    if (m_menu) {
        m_menu->exec(QCursor::pos());
    }

}

void NotesItemWidget::slotUpdateTheme()
{
//    if (m_pPageNumber) {
//        m_pPageNumber->setForegroundRole(DPalette::TextTitle);
//    }
//    if (m_pTextLab) {
//        m_pTextLab->setForegroundRole(QPalette::BrightText);
//    }
}

QString NotesItemWidget::strPage() const
{
    return m_strPage;
}

void NotesItemWidget::setStrPage(const QString &strPage)
{
    m_strPage = strPage;
}

int NotesItemWidget::nNoteType() const
{
    return m_nNoteType;
}

void NotesItemWidget::setNNoteType(const int &nNoteType)
{
    m_nNoteType = nNoteType;
}

void NotesItemWidget::initWidget()
{
    auto t_vLayoutPicture = new QVBoxLayout;
    t_vLayoutPicture->setContentsMargins(0, 3, 0, 0);
    m_pPicture = new ImageLabel(this);
    m_pPicture->setFixedSize(QSize(48, 68));
    m_pPicture->setAlignment(Qt::AlignCenter);
    t_vLayoutPicture->addWidget(m_pPicture);
    t_vLayoutPicture->addStretch(1);

    m_pPageNumber = new PageNumberLabel(this);
    m_pPageNumber->setMinimumWidth(31);
    m_pPageNumber->setFixedHeight(18);
    m_pPageNumber->setForegroundRole(DPalette::WindowText);
    DFontSizeManager::instance()->bind(m_pPageNumber, DFontSizeManager::T8);

    m_pTextLab = new DLabel(this);
    m_pTextLab->setTextFormat(Qt::PlainText);
    m_pTextLab->setFixedHeight(54);
    m_pTextLab->setMinimumWidth(80);
    m_pTextLab->setFrameStyle(QFrame::NoFrame);
    m_pTextLab->setWordWrap(true);
    m_pTextLab->setAlignment(Qt::AlignLeft);
    m_pTextLab->setAlignment(Qt::AlignTop);
    m_pTextLab->setForegroundRole(DPalette::BrightText);
    DFontSizeManager::instance()->bind(m_pTextLab, DFontSizeManager::T9);

    auto hLine = new DHorizontalLine(this);

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(0, 0, 0, 0);
    t_hLayout->setSpacing(0);
    t_hLayout->addWidget(m_pPageNumber);

    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(15, 0, 10, 0);
    t_vLayout->setSpacing(0);
    t_vLayout->addItem(t_hLayout);
    t_vLayout->addWidget(m_pTextLab);
    t_vLayout->addStretch(1);
    t_vLayout->addWidget(hLine);

    auto m_pHLayout = new QHBoxLayout;

    m_pHLayout->setSpacing(1);
    m_pHLayout->setContentsMargins(0, 0, 10, 0);
    m_pHLayout->addItem(t_vLayoutPicture);
    m_pHLayout->addItem(t_vLayout);
    m_pHLayout->setSpacing(1);

    this->setLayout(m_pHLayout);
}

void NotesItemWidget::__InitConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotShowContextMenu(const QPoint &)));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

int NotesItemWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
    return 0;
}

void NotesItemWidget::paintEvent(QPaintEvent *e)
{
    CustomItemWidget::paintEvent(e);



    //  涉及到 主题颜色
    if (m_bPaint) {
        m_pPicture->setForegroundRole(DPalette::Highlight);
    } else {
        m_pPicture->setForegroundRole(QPalette::Shadow);
    }
}

void NotesItemWidget::resizeEvent(QResizeEvent *event)
{
    if (m_pTextLab) {
        QString note = m_strNote;

        note.replace(QChar('\n'), QString(""));
        note.replace(QChar('\t'), QString(""));
        m_pTextLab->setText(calcText(m_pTextLab->font(), note, m_pTextLab->size()));
    }

    CustomItemWidget::resizeEvent(event);
}
