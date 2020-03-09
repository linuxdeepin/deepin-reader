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
}

ScaleWidget::~ScaleWidget()
{
}

int ScaleWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK || msgType == MSG_TAB_SHOW_FILE_CHANGE) {
        SetComboBoxMax(msgContent);
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
    int nCurIndex = scaleComboBox->currentIndex();
    nCurIndex --;
    if (nCurIndex > -1) {
        QString sText = scaleComboBox->itemText(nCurIndex);
        scaleComboBox->setCurrentText(sText);
        scaleComboBox->setCurrentIndex(nCurIndex);
    }
}

void ScaleWidget::slotNextScale()
{
    int nCurIndex = scaleComboBox->currentIndex();
    nCurIndex++;
    if (nCurIndex < scaleComboBox->count()) {
        QString sText = scaleComboBox->itemText(nCurIndex);
        scaleComboBox->setCurrentText(sText);
        scaleComboBox->setCurrentIndex(nCurIndex);
    }
}

void ScaleWidget::SlotCurrentTextChanged(const QString &sText)
{
    int nIndex = sText.lastIndexOf("%");
    if (nIndex == -1) {
        scaleComboBox->setCurrentText(sText + "%");
    }

    QString sTempText = scaleComboBox->currentText();

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

        scaleComboBox->setCurrentIndex(-1);
        nIndex = sTempText.lastIndexOf("%");
        if (nIndex == -1) {
            sTempText = sTempText + "%";
        }
        scaleComboBox->setCurrentText(sTempText);
    }
}



void ScaleWidget::SetComboBoxMax(const QString &sPath)
{
    auto _proxy = MainTabWidgetEx::Instance()->getCurFileAndProxy(sPath);
    if (_proxy) {
        double dMax = _proxy->getMaxZoomratio();

        m_nMaxScale = dMax * 100;

        scaleComboBox->blockSignals(true);

        QList<int> dataList = {10, 25, 50, 75, 100, 125, 150, 175, 200, 300, 400, 500};
        foreach (int iData, dataList) {
            if (iData <= m_nMaxScale) {
                scaleComboBox->addItem(QString::number(iData) + "%");
            }
        }

        FileDataModel fdm = MainTabWidgetEx::Instance()->qGetFileData(sPath);
        int nScale = fdm.qGetData(Scale);
        if (nScale == 0) {
            nScale = 100;
        }

        int nIndex = dataList.indexOf(nScale);
        if (nIndex != -1) {
            scaleComboBox->setCurrentIndex(nIndex);
        } else {
            scaleComboBox->setCurrentIndex(0);
        }
        scaleComboBox->setCurrentText(QString::number(nScale) + "%");

        scaleComboBox->blockSignals(false);
    }
}

void ScaleWidget::SetFitScale(const QString &msgContent)
{
    scaleComboBox->setCurrentIndex(-1);

    double dTemp = msgContent.toDouble() * 100;
    int nScale = dTemp;
    scaleComboBox->setCurrentText(QString::number(nScale) + "%");
}
