// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EncryptionPage.h"
#include "Utils.h"
#include "Application.h"
#include <QDebug>

#include <DFontSizeManager>
#include <DApplication>

#include <QBoxLayout>
#include <DGuiApplicationHelper>

EncryptionPage::EncryptionPage(QWidget *parent)
    : DWidget(parent)
{
    qDebug() << "EncryptionPage created, parent:" << parent;
    InitUI();
    InitConnection();
}

EncryptionPage::~EncryptionPage()
{
    qDebug() << "EncryptionPage destroyed";
}

void EncryptionPage::InitUI()
{
    qDebug() << "Initializing encryption page UI";
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
    m_nextbutton->setText(tr("OK", "button"));
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
    qDebug() << "Encryption page UI initialized";
}

void EncryptionPage::InitConnection()
{
    qDebug() << "Setting up encryption page signal connections";
    connect(m_password, &DPasswordEdit::textChanged, this, &EncryptionPage::onPasswordChanged);
    connect(m_nextbutton, &DPushButton::clicked, this, &EncryptionPage::nextbuttonClicked);

    connect(dApp, SIGNAL(sigSetPasswdFocus()), this, SLOT(onSetPasswdFocus()));
}

void EncryptionPage::nextbuttonClicked()
{
    qDebug() << "Password submitted (length:" << m_password->text().length() << "chars)";
    emit sigExtractPassword(m_password->text());
}

void EncryptionPage::wrongPassWordSlot()
{
    qWarning() << "Wrong password entered";
    m_password->clear();
    m_password->setAlert(true);
    m_password->showAlertMessage(tr("Wrong password"));
    m_password->lineEdit()->setFocus(Qt::TabFocusReason);
}

void EncryptionPage::onPasswordChanged()
{
    qDebug() << "Password field changed, empty:" << m_password->text().isEmpty();
    if (m_password->isAlert()) {
        m_password->setAlert(false);
        m_password->hideAlertMessage();
        qDebug() << "Encryption page theme updated";
    }

    if (m_password->text().isEmpty()) {
        m_nextbutton->setDisabled(true);
    } else {
        m_nextbutton->setEnabled(true);
    }
}

void EncryptionPage::onSetPasswdFocus()
{
    if (this->isVisible() && m_password)
        m_password->lineEdit()->setFocus(Qt::TabFocusReason);
}

void EncryptionPage::onUpdateTheme()
{
    qDebug() << "Updating encryption page theme";
    DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Window, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
}
