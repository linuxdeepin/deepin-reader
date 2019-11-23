#include "AttrScrollWidget.h"
#include <DFrame>
#include <QDebug>
#include <DTextBrowser>
#include <DFontSizeManager>
#include "docview/commonstruct.h"
#include "frame/DocummentFileHelper.h"
#include "utils/utils.h"
#include <DFontSizeManager>

AttrScrollWidget::AttrScrollWidget(DWidget *parent)
    : DFrame(parent)
{
    auto gridLayout = new QGridLayout;
    gridLayout->setContentsMargins(0, 6, 0, 6);
    gridLayout->setSpacing(3);

    stFileInfo fileInfo;
    DocummentFileHelper::instance()->docBasicInfo(fileInfo);

    createLabel(gridLayout, 0, tr("Location"), fileInfo.strFilepath);
    createLabel(gridLayout, 1, tr("Theme"), fileInfo.strTheme);
    createLabel(gridLayout, 2, tr("Author"), fileInfo.strAuther);
    createLabel(gridLayout, 3, tr("Keywords"), fileInfo.strKeyword);
    createLabel(gridLayout, 4, tr("Producers"), fileInfo.strProducter);
    createLabel(gridLayout, 5, tr("Creator"), fileInfo.strCreater);
    createLabel(gridLayout, 6, tr("Create Time"), fileInfo.CreateTime);
    createLabel(gridLayout, 7, tr("Update Time"), fileInfo.ChangeTime);
    createLabel(gridLayout, 8, tr("Format"), fileInfo.strFormat);
    createLabel(gridLayout, 9, tr("Page's Number"), QString("%1").arg(fileInfo.iNumpages));
    createLabel(gridLayout, 10, tr("Optimize"), fileInfo.boptimization);
    createLabel(gridLayout, 11, tr("Security"), fileInfo.bsafe);

    QString sPaperSize = QString("%1*%2mm").arg(fileInfo.iWidth).arg(fileInfo.iHeight);
    createLabel(gridLayout, 12, tr("Paper Size"), sPaperSize);

    createLabel(gridLayout, 13, tr("File Size"), Utils::getInputDataSize(fileInfo.size));

    auto vLayout = new QVBoxLayout;
    vLayout->setContentsMargins(10, 0, 10, 0);

    auto basicLabel = new DLabel(tr("basic information"));
    DFontSizeManager::instance()->bind(basicLabel, DFontSizeManager::T6);
    vLayout->addWidget(basicLabel);

    vLayout->addItem(gridLayout);

    vLayout->addStretch(1);

    this->setLayout(vLayout);
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const QString &sData)
{
    DLabel *label = new DLabel(objName, this);
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);
    label->setAlignment(Qt::AlignTop);
    label->setMaximumWidth(120);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setMaximumWidth(280);
    labelText->setText(sData);
    labelText->setAlignment(Qt::AlignTop);
    labelText->setWordWrap(true);
    layout->addWidget(labelText, index, 1);
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const QDateTime &sData)
{
    DLabel *label = new DLabel(objName, this);
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);
    label->setAlignment(Qt::AlignTop);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setMaximumWidth(280);
    QString strText = getTime(sData);
    labelText->setText(strText);
    labelText->setAlignment(Qt::AlignTop);
    labelText->setWordWrap(true);
    layout->addWidget(labelText, index, 1);
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const bool &bData)
{
    DLabel *label = new DLabel(objName, this);
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);
    label->setAlignment(Qt::AlignTop);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setText(bData ? tr("Yes") : tr("No"));
    labelText->setAlignment(Qt::AlignTop);
    layout->addWidget(labelText, index, 1);
}

//  文件时间设置
QString AttrScrollWidget::getTime(const QDateTime &inTime)
{
    QString sYearText = tr("year"), sMonthText = tr("month"), sDayText = tr("day");
    QString sHourText = tr("hour"), sMinText = tr("minute"), sSecondText = tr("second");

    QString sYear = inTime.toString("yyyy"), sMonth = inTime.toString("MM"), sDay = inTime.toString("dd");
    QString sWeekday = inTime.toString("ddd");
    QString sHour = inTime.toString("HH"), sMin = inTime.toString("mm"), sSec = inTime.toString("ss");

    QString sDate = sYear + sYearText + sMonth + sMonthText + sDay + sDayText;
    QString sTime = sHour + sHourText + sMin + sMinText + sSec + sSecondText;

//    sDate = inTime.toString("yyyy年MM月dd日");
//    sTime = inTime.toString("HH时mm分ss秒");
    return QString("%1 %2 %3").arg(sDate).arg(sWeekday).arg(sTime);
}
