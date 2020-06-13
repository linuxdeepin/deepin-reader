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
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ScaleMenu.h"
#include "DocSheet.h"

ScaleMenu::ScaleMenu(QWidget *parent) : CustomMenu(parent)
{
    initActions();
}

void ScaleMenu::initActions()
{
    addSeparator();
    const QList<qreal> &scaleFactorlst = DocSheet::scaleFactorList();
    for (int i = 0; i < scaleFactorlst.size(); ++i) {
        QAction *scaleFAction = createAction(QString::number(scaleFactorlst.at(i) * 100) + "%", SLOT(onScaleFactor()), true);
        m_actionGroup << scaleFAction;
    }
}

void ScaleMenu::clearAllChecked()
{
    for (QAction *action : m_actionGroup)
        action->setChecked(false);
}

void ScaleMenu::readCurDocParam(DocSheet *docSheet)
{
    m_sheet = docSheet;
    clearAllChecked();
    const QList<qreal> &scaleFactorlst = DocSheet::scaleFactorList();
    int index = scaleFactorlst.indexOf(docSheet->operation().scaleFactor);
    if (index >= 0) m_actionGroup[index]->setChecked(true);
}

void ScaleMenu::onScaleFactor()
{
    const QList<qreal> &scaleFactorlst = DocSheet::scaleFactorList();
    int index = m_actionGroup.indexOf(dynamic_cast<QAction *>(sender()));
    if (index >= 0 && index < scaleFactorlst.size()) {
        m_sheet->setScaleMode(Dr::ScaleFactorMode);
        m_sheet->setScaleFactor(scaleFactorlst.at(index));
    }
}
