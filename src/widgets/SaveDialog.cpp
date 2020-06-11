/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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

#include "ModuleHeader.h"

SaveDialog::SaveDialog(QObject *parent)
    : QObject(parent)
{

}

int SaveDialog::showExitDialog()
{
    DDialog dlg(tr("Do you want to save the changes?"), "");
    dlg.setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    dlg.addButtons(QStringList() <<  tr("Cancel") << tr("Discard"));
    dlg.addButton(tr("Save"), true, DDialog::ButtonRecommend);
    QMargins mar(0, 0, 0, 0);
    dlg.setContentLayoutContentsMargins(mar);
    int nRes = dlg.exec();
    return nRes;
}

int SaveDialog::showTipDialog(const QString &content)
{
    DDialog dlg(content, "");
    dlg.setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    dlg.addButtons(QStringList() <<  tr("Cancel"));
    dlg.addButton(tr("Ensure"), true, DDialog::ButtonRecommend);
    QMargins mar(0, 0, 0, 0);
    dlg.setContentLayoutContentsMargins(mar);
    int nRes = dlg.exec();
    return nRes;
}
