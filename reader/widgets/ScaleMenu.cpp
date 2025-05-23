// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ScaleMenu.h"
#include "DocSheet.h"
#include <QDebug>

ScaleMenu::ScaleMenu(QWidget *parent) : DMenu(parent)
{
    qDebug() << "ScaleMenu created, parent:" << parent;
}

void ScaleMenu::readCurDocParam(DocSheet *docSheet)
{
    qDebug() << "Reading document parameters";
    if (docSheet == nullptr) {
        qDebug() << "No document sheet found, return";
        return;
    }

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
    qDebug() << "Current scale mode:" << docSheet->operation().scaleMode
             << ", layout mode:" << docSheet->operation().layoutMode
             << ", scale factor:" << docSheet->operation().scaleFactor;
}

void ScaleMenu::onTwoPage()
{
    qDebug() << "Two page mode selected, checked:" << m_pTwoPageAction->isChecked();
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
    qDebug() << "Fit height mode selected";
    if (m_sheet)
        m_sheet->setScaleMode(Dr::FitToPageHeightMode);
}

void ScaleMenu::onFiteW()
{
    qDebug() << "Fit width mode selected";
    if (m_sheet)
        m_sheet->setScaleMode(Dr::FitToPageWidthMode);
}

void ScaleMenu::onDefaultPage()
{
    qDebug() << "Default size (1:1) selected";
    if (m_sheet)
        m_sheet->setScaleMode(Dr::FitToPageDefaultMode);
}

void ScaleMenu::onFitPage()
{
    qDebug() << "Fit page mode selected";
    if (m_sheet)
        m_sheet->setScaleMode(Dr::FitToPageWorHMode);
}

void ScaleMenu::onScaleFactor()
{
    qDebug() << "Scale factor selected";
    if (m_sheet == nullptr)
        return;

    const QList<qreal> &scaleFactorlst = m_sheet->scaleFactorList();
    int index = m_actionGroup.indexOf(dynamic_cast<QAction *>(sender()));
    if (index >= 0 && index < scaleFactorlst.size()) {
        m_sheet->setScaleMode(Dr::ScaleFactorMode);
        m_sheet->setScaleFactor(scaleFactorlst.at(index));
    }
}

QAction *ScaleMenu::createAction(const QString &objName, const char *member, bool checkable)
{
    qDebug() << "Creating action:" << objName << ", checkable:" << checkable;
    QAction *action = new QAction(objName, this);
    action->setObjectName(objName);
    action->setCheckable(checkable);
    connect(action, SIGNAL(triggered()), this, member);
    this->addAction(action);
    return action;
}
