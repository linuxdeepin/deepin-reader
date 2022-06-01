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
#include "ShortCutShow.h"
#include "Application.h"
#include "Global.h"
#include "DocSheet.h"

#include <QProcess>
#include <QDesktopWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ShortCutShow::ShortCutShow(QObject *parent)
    : QObject(parent)
{

}

void ShortCutShow::setSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        initPDF();
    else if (Dr::DJVU == sheet->fileType())
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

    KeyDataList<ShortCutType> listType {
        {ShortCutType::Settings,    tr("Settings")},
        {ShortCutType::File,        tr("Files")},
        {ShortCutType::Display,     tr("Display")},
        {ShortCutType::Tools,       tr("Tools")},
        {ShortCutType::Edit,        tr("Edit")},
    };
    for(auto pair : listType)
    {
        ShortCutType type = pair.first;
        QString groupName = pair.second;

        QJsonObject group;
        group.insert("groupName", groupName);
        QJsonArray items;

        for (const auto &d : m_shortcutMap[type]) {
            QJsonObject jsonItem;
            jsonItem.insert("name", d.second);
            jsonItem.insert("value", d.first);
            items.append(jsonItem);
        }

        group.insert("groupItems", items);
        jsonGroups.append(group);
    }

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
    initPDF();
    //remove Dr::key_ctrl_f
    m_shortcutMap[ShortCutType::Tools].removeKey(Dr::key_ctrl_f);
    m_shortcutMap[ShortCutType::Tools].removeKey(Dr::key_alt_h);
    m_shortcutMap[ShortCutType::Tools].removeKey(Dr::key_alt_a);
}

void ShortCutShow::initPDF()
{
    m_shortcutMap = {
        //Settings
        {ShortCutType::Settings, {
             {Dr::key_f1              , tr("Help")               },
             {Dr::val_ctrl_shift_slash, tr("Display shortcuts")  }}},

        //File
        {ShortCutType::File, {
             {Dr::key_ctrl_o          , tr("Open")               },
             {Dr::key_ctrl_shift_s    , tr("Save as")            },
             {Dr::key_ctrl_p          , tr("Print")              },
             {Dr::key_ctrl_s          , tr("Save")               }}},

        //Display
        {ShortCutType::Display, {
             {Dr::key_ctrl_m          , tr("Thumbnails")         },
             {Dr::key_ctrl_1          , tr("1:1 size")           },
             {Dr::key_ctrl_2          , tr("Fit height")         },
             {Dr::key_ctrl_3          , tr("Fit width")          },
             {Dr::key_ctrl_r          , tr("Rotate left")        },
             {Dr::key_ctrl_shift_r    , tr("Rotate right")       },
             {Dr::val_ctrl_equal      , tr("Zoom in")            },
             {Dr::val_ctrl_smaller    , tr("Zoom out")           },
             {Dr::key_pgUp            , tr("Page up")            },
             {Dr::key_pgDown          , tr("Page down")          },
             {Dr::key_ctrl_home       , tr("Move to the beginning") },
             {Dr::key_ctrl_end        , tr("Move to the end")  },
             {Dr::key_esc             , tr("Exit")               }}},

        //Tools
        {ShortCutType::Tools, {
             {Dr::key_alt_1           , tr("Select text")        },
             {Dr::key_alt_2           , tr("Hand tool")          },
             {Dr::key_ctrl_d          , tr("Add bookmark")       },
             {Dr::key_alt_a           , tr("Add annotation")     },
             {Dr::key_alt_h           , tr("Highlight")          },
             {Dr::key_alt_z           , tr("Magnifier")          },
             {Dr::key_ctrl_f          , tr("Search")             },
             {Dr::key_f5              , tr("Slide show")         },
             {Dr::key_f11             , tr("Fullscreen")         }}},

        //Edit
        {ShortCutType::Edit, {
             {Dr::key_ctrl_c          , tr("Copy")               },
             {Dr::key_ctrl_x          , tr("Cut")                },
             {Dr::key_ctrl_v          , tr("Paste")              },
             {Dr::key_delete          , tr("Delete")             },
             {Dr::key_ctrl_z          , tr("Undo")               },
             {Dr::key_ctrl_a          , tr("Select all")         }}},
    };
}
