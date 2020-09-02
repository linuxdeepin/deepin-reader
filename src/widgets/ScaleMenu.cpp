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

}

void ScaleMenu::readCurDocParam(DocSheet *docSheet)
{
    if (docSheet == nullptr)
        return;

    m_sheet = docSheet;
    m_pTwoPageAction    = createAction(tr("Two-Page View"), SLOT(onTwoPage()), true);
    m_pFitDefaultAction = createAction(tr("1:1 size"), SLOT(onDefaultPage()), true);
    m_pFitWorHAction    = createAction(tr("Fit Page"), SLOT(onFitPage()), true);
    m_pFiteHAction      = createAction(tr("Fit Height"), SLOT(onFiteH()), true);
    m_pFiteWAction      = createAction(tr("Fit Width"), SLOT(onFiteW()), true);

    addSeparator();
    const QList<qreal> &scaleFactorlst = m_sheet->scaleFactorList();
    for (int i = 0; i < scaleFactorlst.size(); ++i) {
        QAction *scaleFAction = createAction(QString::number(scaleFactorlst.at(i) * 100) + "%", SLOT(onScaleFactor()), true);
        m_actionGroup << scaleFAction;
    }

    m_actionGroup << m_pTwoPageAction;
    m_actionGroup << m_pFitDefaultAction;
    m_actionGroup << m_pFitWorHAction;
    m_actionGroup << m_pFiteHAction;
    m_actionGroup << m_pFiteWAction;

    int index = scaleFactorlst.indexOf(docSheet->operation().scaleFactor);
    if (index >= 0)
        m_actionGroup[index]->setChecked(true);

    m_pTwoPageAction->setChecked(docSheet->operation().layoutMode == Dr::TwoPagesMode);
    m_pFitDefaultAction->setChecked(docSheet->operation().scaleMode == Dr::FitToPageDefaultMode);
    m_pFitWorHAction->setChecked(docSheet->operation().scaleMode == Dr::FitToPageWorHMode);
    m_pFiteWAction->setChecked(docSheet->operation().scaleMode == Dr::FitToPageWidthMode);
    m_pFiteHAction->setChecked(docSheet->operation().scaleMode == Dr::FitToPageHeightMode);
}

void ScaleMenu::onTwoPage()
{
    if (m_sheet == nullptr)
        return;

    if (m_pTwoPageAction->isChecked())
        m_sheet->setLayoutMode(Dr::TwoPagesMode);
    else
        m_sheet->setLayoutMode(Dr::SinglePageMode);
    m_sheet->setScaleMode(Dr::FitToPageWidthMode);
}

void ScaleMenu::onFiteH()
{
    if (m_sheet)
        m_sheet->setScaleMode(Dr::FitToPageHeightMode);
}

void ScaleMenu::onFiteW()
{
    if (m_sheet)
        m_sheet->setScaleMode(Dr::FitToPageWidthMode);
}

void ScaleMenu::onDefaultPage()
{
    if (m_sheet)
        m_sheet->setScaleMode(Dr::FitToPageDefaultMode);
}

void ScaleMenu::onFitPage()
{
    if (m_sheet)
        m_sheet->setScaleMode(Dr::FitToPageWorHMode);
}

void ScaleMenu::onScaleFactor()
{
    if (m_sheet == nullptr)
        return;

    const QList<qreal> &scaleFactorlst = m_sheet->scaleFactorList();
    int index = m_actionGroup.indexOf(dynamic_cast<QAction *>(sender()));
    if (index >= 0 && index < scaleFactorlst.size()) {
        m_sheet->setScaleMode(Dr::ScaleFactorMode);
        m_sheet->setScaleFactor(scaleFactorlst.at(index));
    }
}
