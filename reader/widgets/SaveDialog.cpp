// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SaveDialog.h"
#include "ddlog.h"
#include <QDebug>
#include <QFontMetrics>
#include <QGuiApplication>

#include <DDialog>

DWIDGET_USE_NAMESPACE

namespace {
constexpr int kMaxFileNameWidth = 400;
}

SaveDialog::SaveDialog(QObject *parent)
    : QObject(parent)
{
    qCDebug(appLog) << "SaveDialog created, parent:" << parent;
}

int SaveDialog::showExitDialog(QString fileName, QWidget  *parent)
{
    qCDebug(appLog) << "Showing save dialog for file:" << fileName << ", parent:" << parent;
    QFontMetrics fm(QGuiApplication::font());
    QString showName = fm.elidedText(fileName, Qt::ElideMiddle, kMaxFileNameWidth);
    DDialog dlg(tr("Save the changes to \"%1\"?").arg(showName), "", parent);
    dlg.setIcon(QIcon::fromTheme("deepin-reader"));
    dlg.addButtons(QStringList() <<  tr("Cancel", "button") << tr("Discard", "button"));
    dlg.addButton(tr("Save", "button"), true, DDialog::ButtonRecommend);
    QMargins mar(12, 12, 12, 12);
    dlg.setContentLayoutContentsMargins(mar);
    int nRes = dlg.exec();
    qCDebug(appLog) << "Save dialog result:" << nRes;
    return nRes;
}

int SaveDialog::showTipDialog(const QString &content, QWidget *parent)
{
    qCDebug(appLog) << "Showing tip dialog, content:" << content;
    DDialog dlg(content, "", parent);
    dlg.setIcon(QIcon::fromTheme("deepin-reader"));
    dlg.addButtons(QStringList() <<  tr("Cancel"));
    dlg.addButton(tr("Delete"), true, DDialog::ButtonRecommend);
    QMargins mar(12, 12, 12, 12);
    dlg.setContentLayoutContentsMargins(mar);
    int nRes = dlg.exec();
    return nRes;
}
