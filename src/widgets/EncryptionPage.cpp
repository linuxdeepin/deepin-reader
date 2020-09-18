/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "EncryptionPage.h"
#include "Utils.h"

#include <DFontSizeManager>
#include <DApplication>

#include <QBoxLayout>
#include <DGuiApplicationHelper>
#include <QDebug>

EncryptionPage::EncryptionPage(QWidget *parent)
    : DWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    InitUI();
    InitConnection();
}

EncryptionPage::~EncryptionPage()
{

}

void EncryptionPage::InitUI()
{
    QPixmap m_encrypticon = QIcon::fromTheme("dr_compress_lock").pixmap(128, 128);
    DLabel *pixmaplabel = new DLabel(this);
    pixmaplabel->setPixmap(m_encrypticon);
    DLabel *stringinfolabel = new DLabel(this);

    DFontSizeManager::instance()->bind(stringinfolabel, DFontSizeManager::T5, QFont::DemiBold);
    stringinfolabel->setForegroundRole(DPalette::ToolTipText);
    stringinfolabel->setText(tr("Encrypted file, please enter the password"));

    m_password = new DPasswordEdit(this);
    m_password->setFixedSize(360, 36);
    QLineEdit *edit = m_password->lineEdit();
    edit->setObjectName("passwdEdit");
    edit->setPlaceholderText(tr("Password"));

    m_nextbutton = new DPushButton(this);
    m_nextbutton->setObjectName("ensureBtn");
    m_nextbutton->setFixedSize(360, 36);
    m_nextbutton->setText(tr("OK"));
    m_nextbutton->setDisabled(true);

    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    mainlayout->setSpacing(0);
    mainlayout->addStretch();
    mainlayout->addWidget(pixmaplabel, 0, Qt::AlignCenter);

    mainlayout->addSpacing(4);
    mainlayout->addWidget(stringinfolabel, 0, Qt::AlignCenter);

    mainlayout->addSpacing(30);
    mainlayout->addWidget(m_password, 0, Qt::AlignCenter);


    mainlayout->addSpacing(20);
    mainlayout->addWidget(m_nextbutton, 0, Qt::AlignCenter);

    mainlayout->addStretch();

    setAutoFillBackground(true);

    onUpdateTheme();
    m_password->lineEdit()->setAttribute(Qt::WA_InputMethodEnabled, false);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &EncryptionPage::onUpdateTheme);
}

void EncryptionPage::InitConnection()
{
    connect(m_password, &DPasswordEdit::textChanged, this, &EncryptionPage::onPasswordChanged);
    connect(m_nextbutton, &DPushButton::clicked, this, &EncryptionPage::nextbuttonClicked);

    connect(dApp, SIGNAL(sigSetPasswdFocus()), this, SLOT(onSetPasswdFocus()));
}

void EncryptionPage::nextbuttonClicked()
{
    emit sigExtractPassword(m_password->text());
}

void EncryptionPage::wrongPassWordSlot()
{
    m_password->clear();
    m_password->setAlert(true);
    m_password->showAlertMessage(tr("Wrong password"));
    m_password->lineEdit()->setFocus(Qt::TabFocusReason);
}

void EncryptionPage::onPasswordChanged()
{
    if (m_password->isAlert()) {
        m_password->setAlert(false);
        m_password->hideAlertMessage();
    }

    if (m_password->text().isEmpty()) {
        m_nextbutton->setDisabled(true);
    } else {
        m_nextbutton->setEnabled(true);
    }
}

void EncryptionPage::onSetPasswdFocus()
{
    if (this->isVisible() && m_password) {
        m_password->lineEdit()->setFocus(Qt::TabFocusReason);
    }
}

void EncryptionPage::onUpdateTheme()
{
    DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
}
