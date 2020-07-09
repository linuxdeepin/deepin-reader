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
#include "ModuleHeader.h"
#include "Global.h"
#include "DocSheet.h"

#include <QProcess>
#include <QDesktopWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Application.h"

ShortCutShow::ShortCutShow(QObject *parent)
    : QObject(parent)
{

}

void ShortCutShow::setSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        initPDF();
    else if (Dr::DjVu == sheet->fileType())
        initDJVU();
    else
        initPDF();
}

void ShortCutShow::show()
{
    QRect rect = qApp->desktop()->geometry();
    QPoint pos(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);

    QJsonObject shortcutObj;
    QJsonArray jsonGroups;
    QString strvalue;
    //Settings
    QJsonObject settingsJsonGroup;
    settingsJsonGroup.insert("groupName", tr("Settings"));
    QJsonArray settingsJsonItems;

    for (const QString &shortcutname : Settingsnames) {
        auto it = shortcutmap.find(shortcutname);
        if (it != shortcutmap.end())
            strvalue = *it;
        else {
            continue;
        }
        QJsonObject jsonItem;
        jsonItem.insert("name", shortcutname);
        jsonItem.insert("value", strvalue);
        settingsJsonItems.append(jsonItem);
    }

    settingsJsonGroup.insert("groupItems", settingsJsonItems);
    jsonGroups.append(settingsJsonGroup);

    //Files
    QJsonObject filesJsonGroup;
    filesJsonGroup.insert("groupName", tr("Files"));
    QJsonArray filesJsonItems;

    for (const QString &shortcutname : Filesnames) {
        auto it = shortcutmap.find(shortcutname);
        if (it != shortcutmap.end())
            strvalue = *it;
        else {
            continue;
        }
        QJsonObject jsonItem;
        jsonItem.insert("name", shortcutname);
        jsonItem.insert("value", strvalue);
        filesJsonItems.append(jsonItem);
    }

    filesJsonGroup.insert("groupItems", filesJsonItems);
    jsonGroups.append(filesJsonGroup);

    //Display
    QJsonObject displayJsonGroup;
    displayJsonGroup.insert("groupName", tr("Display"));
    QJsonArray displayJsonItems;
    for (const QString &shortcutname : Displaynames) {
        auto it = shortcutmap.find(shortcutname);
        if (it != shortcutmap.end())
            strvalue = *it;
        else {
            continue;
        }
        QJsonObject jsonItem;
        jsonItem.insert("name", shortcutname);
        jsonItem.insert("value", strvalue);
        displayJsonItems.append(jsonItem);
    }

    displayJsonGroup.insert("groupItems", displayJsonItems);
    jsonGroups.append(displayJsonGroup);

    //Tools
    QJsonObject toolJsonGroup;
    toolJsonGroup.insert("groupName", tr("Tools"));
    QJsonArray toolJsonItems;

    for (const QString &shortcutname : Toolsnames) {

        auto it = shortcutmap.find(shortcutname);
        if (it != shortcutmap.end())
            strvalue = *it;
        else {
            continue;
        }
        QJsonObject jsonItem;
        jsonItem.insert("name", shortcutname);
        jsonItem.insert("value", strvalue);
        toolJsonItems.append(jsonItem);
    }

    toolJsonGroup.insert("groupItems", toolJsonItems);
    jsonGroups.append(toolJsonGroup);

    //Edit
    QJsonObject editorJsonGroup;
    editorJsonGroup.insert("groupName", tr("Edit"));
    QJsonArray editorJsonItems;
    for (const QString &shortcutname : Editnames) {

        auto it = shortcutmap.find(shortcutname);
        if (it != shortcutmap.end())
            strvalue = *it;
        else {
            continue;
        }
        QJsonObject jsonItem;
        jsonItem.insert("name", shortcutname);
        jsonItem.insert("value", strvalue);
        editorJsonItems.append(jsonItem);
    }

    editorJsonGroup.insert("groupItems", editorJsonItems);
    jsonGroups.append(editorJsonGroup);


    shortcutObj.insert("shortcut", jsonGroups);

    QJsonDocument doc(shortcutObj);

    QStringList shortcutString;
    QString param1 = "-j=" + QString(doc.toJson().data());
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());

    shortcutString << param1 << param2;

    QProcess shortcutViewProcess;
    shortcutViewProcess.startDetached("deepin-shortcut-viewer", shortcutString);
}

void ShortCutShow::initDJVU()
{
    windowKeymaps.clear();
    shortcutnames.clear();
    Settingsnames.clear();
    Filesnames.clear();
    Displaynames.clear();
    Toolsnames.clear();
    Editnames.clear();

    windowKeymaps << Dr::key_esc  << Dr::key_f1
                  << Dr::key_pgUp << Dr::key_pgDown << Dr::key_ctrl_o << Dr::key_alt_harger
                  << Dr::key_ctrl_smaller << Dr::key_ctrl_wheel << Dr::key_ctrl_shift_s
                  << Dr::key_ctrl_p << Dr::key_ctrl_s << Dr::key_ctrl_m << Dr::key_ctrl_1 << Dr::key_ctrl_2
                  << Dr::key_ctrl_3 << Dr::key_ctrl_r << Dr::key_ctrl_shift_r << Dr::key_alt_1 << Dr::key_alt_2
                  << Dr::key_ctrl_d << Dr::key_delete << Dr::key_alt_z
                  << Dr::key_f5 << Dr::key_f11
                  << Dr::key_ctrl_c << Dr::key_ctrl_x << Dr::key_ctrl_v << Dr::key_ctrl_z << Dr::key_ctrl_a << "Ctrl+Shift+?";

    shortcutnames << tr("Exit") << tr("Help")
                  << tr("Page up") << tr("Page down") << tr("Open") << tr("Zoom in")
                  << tr("Zoom out") << tr("Zoom in/Zoom out") << tr("Save as") << tr("Print")
                  << tr("Save") << tr("Thumbnails") << tr("1:1 size") << tr("Fit height")
                  << tr("Fit width") << tr("Rotate left") << tr("Rotate right") << tr("Select text")
                  << tr("Hand tool") << tr("Add bookmark") << tr("Delete") << tr("Magnifier") << tr("Slide show") << tr("Fullscreen")
                  << tr("Copy") << tr("Cut") << tr("Paste") << tr("Undo") << tr("Select all") << tr("Display shortcuts");

    Settingsnames << tr("Help") << tr("Display shortcuts");
    Filesnames    << tr("Open") << tr("Save as") << tr("Print") << tr("Save");
    Displaynames  << tr("Thumbnails") << tr("1:1 size") << tr("Fit height") << tr("Fit width") << tr("Rotate left") << tr("Rotate right")
                  << tr("Zoom in") << tr("Zoom out") << tr("Page up") << tr("Page down")  << tr("Exit") ;
    Toolsnames    << tr("Select text") << tr("Hand tool") << tr("Add bookmark") << tr("Add annotation") << tr("Highlight") << tr("Delete")
                  << tr("Magnifier") << tr("Search") << tr("Slide show") << tr("Fullscreen") ;
    Editnames     << tr("Copy") << tr("Cut") << tr("Paste") << tr("Delete") << tr("Save") << tr("Undo") << tr("Select all");

    int index = 0;
    foreach (QString strname, shortcutnames) {
        shortcutmap.insert(strname, windowKeymaps.at(index));
        index++;
    }
}

void ShortCutShow::initPDF()
{
    windowKeymaps.clear();
    shortcutnames.clear();
    Settingsnames.clear();
    Filesnames.clear();
    Displaynames.clear();
    Toolsnames.clear();
    Editnames.clear();

    windowKeymaps << /*Dr::key_f11 << */Dr::key_esc  << Dr::key_f1
                  << Dr::key_ctrl_f << "PageUp" << "PageDown" << Dr::key_ctrl_o << Dr::key_alt_harger
                  << Dr::key_ctrl_smaller << Dr::key_ctrl_wheel << Dr::key_ctrl_shift_s
                  << Dr::key_ctrl_p << Dr::key_ctrl_s << Dr::key_ctrl_m << Dr::key_ctrl_1 << Dr::key_ctrl_2
                  << Dr::key_ctrl_3 << Dr::key_ctrl_r << Dr::key_ctrl_shift_r << Dr::key_alt_1 << Dr::key_alt_2
                  << Dr::key_ctrl_d << Dr::key_alt_a << Dr::key_alt_h << "Delete" << Dr::key_alt_z
                  << Dr::key_f5 << Dr::key_f11
                  << Dr::key_ctrl_c << Dr::key_ctrl_x << Dr::key_ctrl_v << Dr::key_ctrl_z << Dr::key_ctrl_a << "Ctrl+Shift+?";

    shortcutnames << tr("Exit") << tr("Help")
                  << tr("Search") << tr("Page up") << tr("Page down") << tr("Open") << tr("Zoom in")
                  << tr("Zoom out") << tr("Zoom in/Zoom out") << tr("Save as") << tr("Print")
                  << tr("Save") << tr("Thumbnails") << tr("1:1 size") << tr("Fit height")
                  << tr("Fit width") << tr("Rotate left") << tr("Rotate right") << tr("Select text")
                  << tr("Hand tool") << tr("Add bookmark") << tr("Add annotation") << tr("Highlight")
                  << tr("Delete") << tr("Magnifier") << tr("Slide show") << tr("Fullscreen") << tr("Copy") << tr("Cut") << tr("Paste")
                  << tr("Undo") << tr("Select all") << tr("Display shortcuts");

    Settingsnames << tr("Help") << tr("Display shortcuts");
    Filesnames    << tr("Open") << tr("Save as") << tr("Print") << tr("Save");
    Displaynames  << tr("Thumbnails") << tr("1:1 size") << tr("Fit height") << tr("Fit width") << tr("Rotate left") << tr("Rotate right")
                  << tr("Zoom in") << tr("Zoom out") << tr("Page up") << tr("Page down")  << tr("Exit") ;
    Toolsnames    << tr("Select text") << tr("Hand tool") << tr("Add bookmark") << tr("Add annotation") << tr("Highlight") << tr("Delete")
                  << tr("Magnifier") << tr("Search") << tr("Slide show") << tr("Fullscreen")  ;
    Editnames     << tr("Copy") << tr("Cut") << tr("Paste") << tr("Delete") << tr("Save") << tr("Undo") << tr("Select all");

    int index = 0;
    foreach (QString strname, shortcutnames) {
        shortcutmap.insert(strname, windowKeymaps.at(index));
        index++;
    }
}
