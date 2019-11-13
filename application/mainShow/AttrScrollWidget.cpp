#include "AttrScrollWidget.h"
#include <DFrame>
#include <QDebug>
#include <DTextBrowser>

#include "docview/commonstruct.h"
#include "frame/DocummentFileHelper.h"
#include "utils/utils.h"

AttrScrollWidget::AttrScrollWidget(DWidget *parent)
    : DScrollArea (parent)
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto gridLayout = new QGridLayout;
    gridLayout->setContentsMargins(0, 0, 0, 0);

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

    QFont font("SourceHanSansSC-Medium", 14);
    auto basicLabel = new DLabel(tr("basic information"));
    basicLabel->setFont(font);
    vLayout->addWidget(basicLabel);

    vLayout->addItem(gridLayout);

    auto frame = new DFrame;
    frame->setMinimumWidth(278);
    this->setWidget(frame);
    frame->setLayout(vLayout);
    frame->adjustSize();
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const QString &sData)
{
    QFont font("SourceHanSansSC-Normal", 12);

    DLabel *label = new DLabel(objName, this);
    label->setFont(font);

    label->setAlignment(Qt::AlignTop);
    label->setMinimumWidth(60);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    labelText->setFont(font);
    labelText->setMaximumWidth(200);
    labelText->setText(sData);
    labelText->setAlignment(Qt::AlignTop);
    labelText->setWordWrap(true);
    layout->addWidget(labelText, index, 1);
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const QDateTime &sData)
{
    QFont font("SourceHanSansSC-Normal", 12);

    DLabel *label = new DLabel(objName, this);
    label->setFont(font);
    label->setAlignment(Qt::AlignTop);
    label->setMinimumWidth(60);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    labelText->setFont(font);
    labelText->setMaximumWidth(200);
    QString strText = getTime(sData);
    labelText->setText(strText);
    labelText->setAlignment(Qt::AlignTop);
    labelText->setWordWrap(true);
    layout->addWidget(labelText, index, 1);
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const bool &bData)
{
    QFont font("SourceHanSansSC-Normal", 12);

    DLabel *label = new DLabel(objName, this);
    label->setFont(font);
    label->setAlignment(Qt::AlignTop);
    label->setMinimumWidth(60);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    labelText->setFont(font);
    labelText->setMaximumWidth(200);
    labelText->setText(bData ? tr("Yes") : tr("No"));
    labelText->setAlignment(Qt::AlignTop);
    labelText->setWordWrap(true);
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
