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
#include "ScaleWidget.h"

#include <QHBoxLayout>
#include <QLineEdit>

#include <DIconButton>
#include <DComboBox>

#include "docview/docummentproxy.h"
#include "DocSheet.h"

ScaleWidget::ScaleWidget(DWidget *parent)
    : DWidget(parent)
{
    initWidget();

    dataList = {10, 25, 50, 75, 100, 125, 150, 175, 200, 300, 400, 500};
}

ScaleWidget::~ScaleWidget()
{

}

void ScaleWidget::initWidget()
{
    auto m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    m_scaleComboBox = new DComboBox();
    m_scaleComboBox->setInsertPolicy(QComboBox::NoInsert);
    m_scaleComboBox->setDuplicatesEnabled(false); //  重复项 不允许添加
    int tW = 120;
    int tH = 0;

    m_scaleComboBox->setFixedWidth(tW);
    m_scaleComboBox->setEditable(true);

    QLineEdit *edit = m_scaleComboBox->lineEdit();
    connect(edit, SIGNAL(returnPressed()), SLOT(SlotReturnPressed()));

    DIconButton *pPreBtn = new DIconButton(DStyle::SP_DecreaseElement);
    tW = 24;
    tH = 24;

    pPreBtn->setFixedSize(QSize(tW, tH));
    connect(pPreBtn, SIGNAL(clicked()), SLOT(slotPrevScale()));

    DIconButton *pNextBtn = new DIconButton(DStyle::SP_IncreaseElement);
    pNextBtn->setFixedSize(QSize(tW, tH));
    connect(pNextBtn, SIGNAL(clicked()), SLOT(slotNextScale()));

    m_layout->addWidget(pPreBtn);
    m_layout->addWidget(m_scaleComboBox);
    m_layout->addWidget(pNextBtn);
}

void ScaleWidget::slotPrevScale()
{
    if (m_sheet.isNull())
        return;

    QString cuttext = m_scaleComboBox->currentText();
    int nIndex = cuttext.lastIndexOf("%");
    if (nIndex > 0)
        cuttext = cuttext.mid(0, nIndex);
    bool bok;
    int curindex = -1;
    double curscale = cuttext.toDouble(&bok);
    if (curscale <= dataList.front()) {
        return;
    }
    for (int i = 1; i < dataList.size(); i++) {
        if (dataList.at(i) >= curscale && dataList.at(i - 1) < curscale) {
            curindex = i - 1;
            break;
        }
    }
    if (bok) {
        m_scaleComboBox->setCurrentIndex(curindex);
    }
}

void ScaleWidget::slotNextScale()
{
    if (m_sheet.isNull())
        return;

    QString inputtext = m_scaleComboBox->currentText();
    int nIndex = inputtext.lastIndexOf("%");
    if (nIndex > 0)
        inputtext = inputtext.mid(0, nIndex);
    bool bok;
    double inputscale = inputtext.toDouble(&bok);
    int curindex = -1;
    if (inputscale >= dataList.back())
        return;
    for (int i = 0; i < dataList.size(); i++) {
        if (dataList.at(i) > inputscale) {
            curindex = i;
            break;
        }
    }
    if (bok) {
        m_scaleComboBox->setCurrentIndex(curindex);
    }
}

void ScaleWidget::SlotCurrentTextChanged(const QString &sText)
{
    if (m_sheet.isNull())
        return;

    int nIndex = sText.lastIndexOf("%");
    if (nIndex == -1) {
        QString sssTemp = sText + "%";
        m_scaleComboBox->setCurrentText(sssTemp);
        nIndex = sssTemp.lastIndexOf("%");
    }

    QString sTempText = m_scaleComboBox->currentText();
    bool bOk = false;
    QString sTempData = sTempText.mid(0, nIndex);
    double dValue = sTempData.toDouble(&bOk);

    if (bOk && dValue >= 10.0 && dValue <= m_nMaxScale) {
        m_sheet->setFit(NO_ADAPTE_State);
        m_sheet->setScale(dValue);
    }
}

//  combobox 敲了回车
void ScaleWidget::SlotReturnPressed()
{
    if (m_sheet.isNull())
        return;

    QString sTempText = m_scaleComboBox->currentText();

    int nIndex = m_scaleComboBox->findText(sTempText, Qt::MatchExactly);
    if (nIndex == -1) {     //  列表中没有输入的选项

        nIndex = sTempText.lastIndexOf("%");
        if (nIndex != -1) {
            sTempText = sTempText.mid(0, nIndex);
        }
        bool bOk = false;
        double dValue = sTempText.toDouble(&bOk);
        if (bOk && dValue >= 10.0 && dValue <= m_nMaxScale) {
            QString sEndValue = QString::number(dValue, 'f', 2);        //  保留2位小数点
            dValue = sEndValue.toDouble();

            QString sShowText = QString::number(dValue) + "%";
            SlotCurrentTextChanged(sShowText);

            int curindex = dataList.indexOf(static_cast<int>(dValue));
            if (curindex < 0)
                m_scaleComboBox->setCurrentIndex(curindex);
            m_scaleComboBox->setCurrentText(sShowText);

        }
    }

    m_sheet->setFit(NO_ADAPTE_State);
}

void ScaleWidget::setSheet(DocSheet *sheet)
{
    m_sheet = sheet;

    if (nullptr == sheet)
        return;

    disconnect(m_scaleComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(SlotCurrentTextChanged(const QString &)));

    auto _proxy = sheet->getDocProxy();
    if (_proxy) {
        double dMax = _proxy->getMaxZoomratio();

        m_scaleComboBox->blockSignals(true);

        int nTempMax = static_cast<int>(dMax) * 100;

        if (nTempMax != m_nMaxScale) {  //  判断当前最大显示 是否 和之前一样, 不一样, 清楚item, 重新添加
            m_scaleComboBox->clear();
            m_nMaxScale = nTempMax;

            foreach (int iData, dataList) {
                if (iData <= m_nMaxScale) {
                    m_scaleComboBox->addItem(QString::number(iData) + "%");
                }
            }
        }

        double nScale = m_sheet->getOper(Scale).toDouble();

        if (static_cast<int>(nScale) <= 0) {
            nScale = 100.0;
        }
        int index = -1;
        for (int i = 0; i < dataList.size(); i++) {
            if (qAbs(dataList.at(i) - nScale) < 0.001) {
                index = i;
                break;
            }
        }

        QString sCurText = m_sheet->getOper(Scale).toString() + "%";

        m_scaleComboBox->setCurrentText(sCurText);

        m_scaleComboBox->blockSignals(false);
    }

    connect(m_scaleComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(SlotCurrentTextChanged(const QString &)));
}

bool ScaleWidget::handleShortcut(QString shortcut)
{
    if (shortcut == KeyStr::g_ctrl_smaller) {
        slotPrevScale();
        return true;
    } else if (shortcut == KeyStr::g_ctrl_larger || shortcut == KeyStr::g_ctrl_equal) {
        slotNextScale();
        return true;
    } else if (shortcut == KeyStr::g_ctrl_1) {   // 恢复 100 比例
        SlotCurrentTextChanged("100");
        return true;
    }

    return false;
}

void ScaleWidget::clearComboBox()
{
    if (m_scaleComboBox) {
        m_scaleComboBox->lineEdit()->clear();
    }
}

void ScaleWidget::paintEvent(QPaintEvent *event)
{
    return;
}
