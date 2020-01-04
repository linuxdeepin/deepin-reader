/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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
#include "ShortCutShow.h"

#include <QProcess>
#include <QDesktopWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "application.h"
#include "subjectObserver/ModuleHeader.h"

ShortCutShow::ShortCutShow(QObject *parent)
    : QObject(parent)
{

}

void ShortCutShow::show(const QStringList &shortcutnames, const QStringList &windowKeymaps)
{
    QRect rect = qApp->desktop()->geometry();
    QPoint pos(rect.x() + rect.width() / 2,
               rect.y() + rect.height() / 2);

    QJsonObject shortcutObj;
    QJsonArray jsonGroups;

    QJsonObject windowJsonGroup;
    windowJsonGroup.insert("groupName", "Window");
    QJsonArray windowJsonItems;

    int index = 0;
    for (const QString &shortcutname : shortcutnames) {

        QJsonObject jsonItem;
        jsonItem.insert("name", shortcutname);
        jsonItem.insert("value", windowKeymaps.at(index));
        windowJsonItems.append(jsonItem);
        index++;
    }

    windowJsonGroup.insert("groupItems", windowJsonItems);
    jsonGroups.append(windowJsonGroup);
    shortcutObj.insert("shortcut", jsonGroups);

    QJsonDocument doc(shortcutObj);

    QStringList shortcutString;
    QString param1 = "-j=" + QString(doc.toJson().data());
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());

    shortcutString << param1 << param2;

    QProcess shortcutViewProcess;
    shortcutViewProcess.startDetached("deepin-shortcut-viewer", shortcutString);
}
