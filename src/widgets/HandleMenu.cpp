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
#include "HandleMenu.h"
#include "DocSheet.h"
#include <QDebug>

HandleMenu::HandleMenu(DWidget *parent)
    : CustomMenu(parent)
{
    initActions();
}

void HandleMenu::initActions()
{
    m_docSheet = nullptr;
    QActionGroup *actionGroup = new QActionGroup(this);

    m_textAction = new QAction(tr("Select Tool"), this);
    connect(m_textAction, SIGNAL(triggered()), SLOT(onSelectText()));
    m_textAction->setObjectName("defaultshape");
    m_textAction->setCheckable(true);

    actionGroup->addAction(m_textAction);
    addAction(m_textAction);

    m_handAction = new QAction(tr("Hand Tool"), this);
    connect(m_handAction, SIGNAL(triggered()), SLOT(onHandTool()));
    m_handAction->setObjectName("handleshape");
    m_handAction->setCheckable(true);

    actionGroup->addAction(m_handAction);
    addAction(m_handAction);
}

void HandleMenu::onHandTool()
{
    if (m_docSheet)
        m_docSheet->setMouseShape(Dr::MouseShapeHand);
}

void HandleMenu::onSelectText()
{
    if (m_docSheet)
        m_docSheet->setMouseShape(Dr::MouseShapeNormal);
}

void HandleMenu::readCurDocParam(DocSheet *docSheet)
{
    m_docSheet = docSheet;
    if (!docSheet)
        return;
    if (m_docSheet->operation().mouseShape == Dr::MouseShapeNormal)
        m_textAction->setChecked(true);
    else if (m_docSheet->operation().mouseShape == Dr::MouseShapeHand)
        m_handAction->setChecked(true);
}
