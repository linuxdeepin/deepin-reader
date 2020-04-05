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
    : CustomWidget("ScaleWidget", parent)
{
    initWidget();
    m_pKeyMsgList << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_equal << KeyStr::g_ctrl_smaller << KeyStr::g_ctrl_1;

    dataList = {10, 25, 50, 75, 100, 125, 150, 175, 200, 300, 400, 500};
}

ScaleWidget::~ScaleWidget()
{
}

int ScaleWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_FILE_FIT_SCALE) {
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
    int tW = 120;
    int tH = 0;
//    dApp->adaptScreenView(tW, tH);
    scaleComboBox->setFixedWidth(tW);
    scaleComboBox->setEditable(true);

    QLineEdit *edit = scaleComboBox->lineEdit();
    connect(edit, SIGNAL(returnPressed()), SLOT(SlotReturnPressed()));

    DIconButton *pPreBtn = new DIconButton(DStyle::SP_DecreaseElement);
    tW = 24;
    tH = 24;
//    dApp->adaptScreenView(tW, tH);
    pPreBtn->setFixedSize(QSize(tW, tH));
    connect(pPreBtn, SIGNAL(clicked()), SLOT(slotPrevScale()));

    DIconButton *pNextBtn = new DIconButton(DStyle::SP_IncreaseElement);
    pNextBtn->setFixedSize(QSize(tW, tH));
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
    } else if (keyType == KeyStr::g_ctrl_1) {   // 恢复 100 比例
        SlotCurrentTextChanged("100");
    }
}

void ScaleWidget::slotPrevScale()
{
    QString cuttext = scaleComboBox->currentText();
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
        scaleComboBox->setCurrentIndex(curindex);
    }
}

void ScaleWidget::slotNextScale()
{
    QString inputtext = scaleComboBox->currentText();
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
        scaleComboBox->setCurrentIndex(curindex);
    }
}

void ScaleWidget::SlotCurrentTextChanged(const QString &sText)
{
    int nIndex = sText.lastIndexOf("%");
    if (nIndex == -1) {
        QString sssTemp = sText + "%";
        scaleComboBox->setCurrentText(sssTemp);
        nIndex = sssTemp.lastIndexOf("%");
    }

    QString sTempText = scaleComboBox->currentText();
    bool bOk = false;
    QString sTempData = sTempText.mid(0, nIndex);
    double dValue = sTempData.toDouble(&bOk);
    if (bOk && dValue >= 10.0 && dValue <= m_nMaxScale) {
        qInfo() <<   "     scale  dValue:" << dValue;
        QJsonObject obj;
        QString str{""};
        str = QString::number(dValue) + Constant::sQStringSep + QString::number(1);
        obj.insert("content", str);//QString::number(dValue));
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
                scaleComboBox->setCurrentIndex(curindex);
            scaleComboBox->setCurrentText(sShowText);

        }
    }
}

void ScaleWidget::setSheet(DocSheet *sheet)
{
    if (nullptr == sheet)
        return;

    auto _proxy = sheet->getDocProxy();
    if (_proxy) {
        double dMax = _proxy->getMaxZoomratio();

        scaleComboBox->blockSignals(true);

        int nTempMax = static_cast<int>(dMax) * 100;

        if (nTempMax != m_nMaxScale) {  //  判断当前最大显示 是否 和之前一样, 不一样, 清楚item, 重新添加
            scaleComboBox->clear();
            m_nMaxScale = nTempMax;

            foreach (int iData, dataList) {
                if (iData <= m_nMaxScale) {
                    scaleComboBox->addItem(QString::number(iData) + "%");
                }
            }
        }

        FileDataModel fdm = sheet->qGetFileData();
        double nScale = fdm.qGetData(Scale);
        if (static_cast<int>(nScale) == 0) {
            nScale = 100;
        }
        int index = -1;
        for (int i = 0; i < dataList.size(); i++) {
            if (qAbs(dataList.at(i) - nScale) < 0.001) {
                index = i;
                break;
            }
        }
        dataList.indexOf(static_cast<int>(nScale));
        scaleComboBox->setCurrentIndex(index);
        if (index == -1) {
            QString sCurText = QString::number(nScale) + "%";
            scaleComboBox->setCurrentText(sCurText);
        }
        scaleComboBox->blockSignals(false);
    }
}

void ScaleWidget::SetFitScale(const QString &msgContent)
{
    scaleComboBox->blockSignals(true);

    double dTemp = msgContent.toDouble() * 100;
    int nScale = static_cast<int>(dTemp);
    QString sCurText = QString::number(nScale) + "%";

    qInfo() << "    sCurText1:" << sCurText;
    int nIndex = scaleComboBox->findText(sCurText);
    if (nIndex != -1) {
        m_nCurrentIndex = nIndex;
        scaleComboBox->setCurrentIndex(m_nCurrentIndex);
    } else {
        dataList.append(nScale);
        qSort(dataList.begin(), dataList.end());

        m_nCurrentIndex = dataList.indexOf(nScale);
        dataList.removeOne(nScale);

        m_nCurrentIndex--;

        scaleComboBox->setCurrentIndex(-1);
        scaleComboBox->setCurrentText(sCurText);
    }

    scaleComboBox->blockSignals(false);

    QJsonObject obj;
    QString str{""};
    str = QString::number(nScale) + Constant::sQStringSep + QString::number(0);
    obj.insert("content", str);//QString::number(dValue));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);
    QJsonDocument doc(obj);
    dApp->m_pModelService->notifyMsg(MSG_FILE_SCALE, doc.toJson(QJsonDocument::Compact));
}
