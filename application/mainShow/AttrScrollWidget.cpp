#include "AttrScrollWidget.h"

#include <QDebug>
#include <DTextBrowser>
#include <DFontSizeManager>
#include "docview/commonstruct.h"
#include "frame/DocummentFileHelper.h"
#include "utils/utils.h"
#include <DFontSizeManager>
#include "CustomControl/DFMGlobal.h"

#define LAEBL_TEXT_WIDTH    280

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
    vLayout->setContentsMargins(10, 10, 10, 10);

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

    if (sData == "") {
        DLabel *labelText = new DLabel(this);
        DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
        labelText->setText(tr("unknown"));
        layout->addWidget(labelText, index, 1);
    } else {
        DFrame *frame = addTitleFrame(sData);
        frame->setFrameShape(QFrame::NoFrame);
        layout->addWidget(frame, index, 1);
    }
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

DFrame *AttrScrollWidget::addTitleFrame(const QString &sData)
{
    DFrame *m_textShowFrame = new DFrame(this);

    QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
    QString t = DFMGlobal::elideText(sData, QSize(LAEBL_TEXT_WIDTH, 60), QTextOption::WrapAnywhere, font, Qt::ElideMiddle, 0);
    QStringList labelTexts = t.split("\n");
    const int maxLineCount = 3;

    int textHeight = 0;

    QVBoxLayout *hLayout = new QVBoxLayout;
    for (int i = 0; i < labelTexts.length(); i++) {
        if (i > (maxLineCount - 1)) {
            break;
        }
        QString labelText = labelTexts.at(i);
        QLabel *label = new QLabel(labelText, m_textShowFrame);
        DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);

        label->setAlignment(Qt::AlignLeft);

        textHeight += label->fontInfo().pixelSize() + 10;

        hLayout->addWidget(label);
        if (i < (labelTexts.length() - 1) && i != (maxLineCount - 1)) {
            if (label->fontMetrics().width(labelText) > (LAEBL_TEXT_WIDTH - 10)) {
                label->setFixedWidth(LAEBL_TEXT_WIDTH);
            }
        } else {
            // the final line of file name label, with a edit btn.
            if (labelTexts.length() >= maxLineCount) {
                for (int idx = i + 1; idx < labelTexts.length(); idx++) {
                    labelText += labelTexts.at(idx);
                }
            }

            if (label->fontMetrics().width(labelText) > LAEBL_TEXT_WIDTH && labelTexts.length() >= maxLineCount) {
                labelText = label->fontMetrics().elidedText(labelText, Qt::ElideMiddle, LAEBL_TEXT_WIDTH);
            }
            label->setText(labelText);
        }
    }
    hLayout->addStretch(1);

    QHBoxLayout *textShowLayout = new QHBoxLayout;
    textShowLayout->setContentsMargins(0, 0, 0, 0);
    textShowLayout->setSpacing(0);

    m_textShowFrame->setLayout(textShowLayout);

    textShowLayout->addLayout(hLayout);

    textShowLayout->addStretch(1);

    m_textShowFrame->setFixedHeight(textHeight);

    return m_textShowFrame;
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
