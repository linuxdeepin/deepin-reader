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
#include "main/MainTabWidgetEx.h"

ScaleWidget::ScaleWidget(DWidget *parent)
    : CustomWidget("ScaleWidget", parent)
{
    initWidget();
    m_pKeyMsgList << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_equal << KeyStr::g_ctrl_smaller;
    dataList = {10, 25, 50, 75, 100, 125, 150, 175, 200, 300, 400, 500};
}

ScaleWidget::~ScaleWidget()
{
}

int ScaleWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK || msgType == MSG_TAB_SHOW_FILE_CHANGE) {
        SetComboBoxMax();
    } else if (msgType == MSG_FILE_FIT_SCALE) {
        SetFitScale(msgContent);
        return MSG_OK;
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        onShortKey(msgContent);

        if (m_pKeyMsgList.contains(msgContent)) {
            return MSG_OK;
        }
    }
    return MSG_NO_OK;
}

void ScaleWidget::initWidget()
{
    auto m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    scaleComboBox = new DComboBox();
    connect(scaleComboBox, SIGNAL(currentIndexChanged(const QString &)), SLOT(SlotCurrentTextChanged(const QString &)));
    scaleComboBox->setInsertPolicy(QComboBox::NoInsert);
    scaleComboBox->setDuplicatesEnabled(false); //  重复项 不允许添加
    scaleComboBox->setFixedWidth(100);
    scaleComboBox->setEditable(true);

    QLineEdit *edit = scaleComboBox->lineEdit();
    connect(edit, SIGNAL(returnPressed()), SLOT(SlotReturnPressed()));

    DIconButton *pPreBtn = new DIconButton(DStyle::SP_DecreaseElement);
    pPreBtn->setFixedSize(QSize(24, 24));
    connect(pPreBtn, SIGNAL(clicked()), SLOT(slotPrevScale()));

    DIconButton *pNextBtn = new DIconButton(DStyle::SP_IncreaseElement);
    pNextBtn->setFixedSize(QSize(24, 24));
    connect(pNextBtn, SIGNAL(clicked()), SLOT(slotNextScale()));

    m_layout->addWidget(pPreBtn);
    m_layout->addWidget(scaleComboBox);
    m_layout->addWidget(pNextBtn);
}

void ScaleWidget::onShortKey(const QString &keyType)
{
    if (keyType == KeyStr::g_ctrl_smaller) {
        slotPrevScale();
    } else if (keyType == KeyStr::g_ctrl_larger || keyType == KeyStr::g_ctrl_equal) {
        slotNextScale();
    }
}

void ScaleWidget::slotPrevScale()
{
    m_nCurrentIndex --;
    if (m_nCurrentIndex > -1) {
        QString sText = scaleComboBox->itemText(m_nCurrentIndex);
        scaleComboBox->setCurrentText(sText);
        scaleComboBox->setCurrentIndex(m_nCurrentIndex);
    }
}

void ScaleWidget::slotNextScale()
{
    m_nCurrentIndex ++;
    if (m_nCurrentIndex < scaleComboBox->count()) {
        QString sText = scaleComboBox->itemText(m_nCurrentIndex);
        scaleComboBox->setCurrentText(sText);
        scaleComboBox->setCurrentIndex(m_nCurrentIndex);
    }
}

void ScaleWidget::SlotCurrentTextChanged(const QString &sText)
{
    int nIndex = sText.lastIndexOf("%");
    if (nIndex == -1) {
        scaleComboBox->setCurrentText(sText + "%");
    }

    QString sTempText = scaleComboBox->currentText();
    int nTempIndex = scaleComboBox->findText(sTempText);
    if (nTempIndex != -1) {
        m_nCurrentIndex = nTempIndex;
    }

    bool bOk = false;
    QString sTempData = sTempText.mid(0, nIndex);
    double dValue = sTempData.toDouble(&bOk);
    if (bOk && dValue > 10.0 && dValue <= m_nMaxScale) {
        QJsonObject obj;
        obj.insert("content", QString::number(dValue));
        obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

        QJsonDocument doc(obj);

        dApp->m_pModelService->notifyMsg(MSG_FILE_SCALE, doc.toJson(QJsonDocument::Compact));

        emit sigScaleChanged();
    }
}

//  combobox 敲了回车
void ScaleWidget::SlotReturnPressed()
{
    QString sTempText = scaleComboBox->currentText();
    int nIndex = scaleComboBox->findText(sTempText, Qt::MatchExactly);
    if (nIndex == -1) {     //  列表中没有输入的选项
        SlotCurrentTextChanged(sTempText);

        nIndex = sTempText.lastIndexOf("%");
        if (nIndex != -1) {
            sTempText = sTempText.mid(0, nIndex);
        }

        bool bOk = false;
        double dValue = sTempText.toDouble(&bOk);
        if (bOk && dValue > 10.0 && dValue <= m_nMaxScale) {

            dataList.append(dValue);
            qSort(dataList.begin(), dataList.end());

            m_nCurrentIndex = dataList.indexOf(dValue);
            dataList.removeOne(dValue);

            m_nCurrentIndex--;

            scaleComboBox->setCurrentText(sTempText + "%");
        }
    }
}

void ScaleWidget::SetComboBoxMax()
{
    QString sPath = MainTabWidgetEx::Instance()->qGetCurPath();

    auto _proxy = MainTabWidgetEx::Instance()->getCurFileAndProxy(sPath);
    if (_proxy) {
        double dMax = _proxy->getMaxZoomratio();

        scaleComboBox->blockSignals(true);

        int nTempMax = dMax * 100;
        if (nTempMax != m_nMaxScale) {
            scaleComboBox->clear();
            m_nMaxScale = nTempMax;

            foreach (int iData, dataList) {
                if (iData <= m_nMaxScale) {
                    scaleComboBox->addItem(QString::number(iData) + "%");
                }
            }
        }

        FileDataModel fdm = MainTabWidgetEx::Instance()->qGetFileData(sPath);
        int nScale = fdm.qGetData(Scale);
        if (nScale == 0) {
            nScale = 100;
        }
        m_nCurrentIndex = dataList.indexOf(nScale);

        scaleComboBox->setCurrentIndex(m_nCurrentIndex);
        if (m_nCurrentIndex == -1) {
            dataList.append(nScale);
            qSort(dataList.begin(), dataList.end());

            m_nCurrentIndex = dataList.indexOf(nScale);
            dataList.removeOne(nScale);

            m_nCurrentIndex--;

            QString sCurText = QString::number(nScale) + "%";
            scaleComboBox->setCurrentText(sCurText);
        }

        scaleComboBox->blockSignals(false);
    }
}

void ScaleWidget::SetFitScale(const QString &msgContent)
{
    double dTemp = msgContent.toDouble() * 100;
    int nScale = dTemp;
    QString sCurText = QString::number(nScale) + "%";

    int nIndex = scaleComboBox->findText(sCurText);
    if (nIndex != -1) {
        m_nCurrentIndex = nIndex;
        scaleComboBox->setCurrentIndex(m_nCurrentIndex);
    } else {
        scaleComboBox->setCurrentIndex(-1);
        scaleComboBox->setCurrentText(sCurText);
    }
}
