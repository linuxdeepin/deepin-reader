/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#include "SaveDialog.h"

#include <DDialog>

DWIDGET_USE_NAMESPACE

SaveDialog::SaveDialog(QObject *parent)
    : QObject(parent)
{

}

int SaveDialog::showExitDialog(QString fileName, QWidget  *parent)
{
    DDialog dlg(tr("Save the changes to \"%1\"?").arg(fileName), "", parent);
    dlg.setIcon(QIcon::fromTheme("deepin-reader"));
    dlg.addButtons(QStringList() <<  tr("Cancel", "button") << tr("Discard", "button"));
    dlg.addButton(tr("Save", "button"), true, DDialog::ButtonRecommend);
    QMargins mar(12, 12, 12, 12);
    dlg.setContentLayoutContentsMargins(mar);
    int nRes = dlg.exec();
    return nRes;
}

int SaveDialog::showTipDialog(const QString &content, QWidget *parent)
{
    DDialog dlg(content, "", parent);
    dlg.setIcon(QIcon::fromTheme("deepin-reader"));
    dlg.addButtons(QStringList() <<  tr("Cancel"));
    dlg.addButton(tr("Delete"), true, DDialog::ButtonRecommend);
    QMargins mar(12, 12, 12, 12);
    dlg.setContentLayoutContentsMargins(mar);
    int nRes = dlg.exec();
    return nRes;
}
