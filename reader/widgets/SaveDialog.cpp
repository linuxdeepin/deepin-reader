// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SaveDialog.h"
#include "ddlog.h"
#include <QDebug>

#include <DDialog>

DWIDGET_USE_NAMESPACE

SaveDialog::SaveDialog(QObject *parent)
    : QObject(parent)
{
    qCDebug(appLog) << "SaveDialog created, parent:" << parent;
}

int SaveDialog::showExitDialog(QString fileName, QWidget  *parent)
{
    qCDebug(appLog) << "Showing save dialog for file:" << fileName << ", parent:" << parent;
    DDialog dlg(tr("Save the changes to \"%1\"?").arg(fileName), "", parent);
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
