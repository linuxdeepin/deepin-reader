#include "AttrScrollWidget.h"

#include <QDebug>
#include <DFontSizeManager>
#include <DTextBrowser>
#include <DLabel>
#include <QGridLayout>

#include "pdfControl/docview/CommonStruct.h"
#include "pdfControl/docview/DocummentProxy.h"
#include "utils/Utils.h"
#include "DocSheet.h"
#include "widgets/WordWrapLabel.h"

DWIDGET_USE_NAMESPACE
#define LAEBL_TEXT_WIDTH   165
AttrScrollWidget::AttrScrollWidget(DocSheet *sheet, DWidget *parent)
    : DFrame(parent)
    , m_leftminwidth(60)
{
    setFrameShape(QFrame::NoFrame);

    installEventFilter(this);
    auto gridLayout = new QGridLayout;
    gridLayout->setContentsMargins(0, 6, 0, 6);
    gridLayout->setSpacing(3);

//    stFileInfo fileInfo;
//    sheet->docBasicInfo(fileInfo);
//    QLocale locale;
//    QFontMetrics fm(font());
//    //用最长字符来计算左侧最小宽度
//    if (locale.language() == QLocale::English) {
//        m_leftminwidth = fm.horizontalAdvance(("Time modified"));
//    } else if (locale.language() == QLocale::Chinese) {
//        m_leftminwidth = fm.horizontalAdvance("页面大小");
//    }

//    createLabel(gridLayout, 0, tr("Location"), fileInfo.strFilepath);
//    createLabel(gridLayout, 1, tr("Subject"), fileInfo.strTheme);
//    createLabel(gridLayout, 2, tr("Author"), fileInfo.strAuther);
//    createLabel(gridLayout, 3, tr("Keywords"), fileInfo.strKeyword);
//    createLabel(gridLayout, 4, tr("Producer"), fileInfo.strProducter);
//    createLabel(gridLayout, 5, tr("Creator"), fileInfo.strCreater);
//    createLabel(gridLayout, 6, tr("Time created"), fileInfo.CreateTime);
//    createLabel(gridLayout, 7, tr("Time modified"), fileInfo.ChangeTime);
//    createLabel(gridLayout, 8, tr("Format"), fileInfo.strFormat);
//    createLabel(gridLayout, 9, tr("Pages"), QString("%1").arg(fileInfo.iNumpages));
//    createLabel(gridLayout, 10, tr("Optimized"), fileInfo.boptimization);
//    createLabel(gridLayout, 11, tr("Security"), fileInfo.bsafe);
//    QString sPaperSize = QString("%1*%2").arg(fileInfo.iWidth).arg(fileInfo.iHeight);
//    createLabel(gridLayout, 12, tr("Page size"), sPaperSize);
//    createLabel(gridLayout, 13, tr("File size"), Utils::getInputDataSize(static_cast<qint64>(fileInfo.size)));

    auto vLayout = new QVBoxLayout;
    vLayout->setContentsMargins(10, 10, 10, 10);

    auto basicLabel = new DLabel(tr("Basic info"));
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
    label->setMaximumWidth(140);
    label->setMinimumWidth(70 + 20);
    layout->addWidget(label, index, 0);

    QString text = sData.isEmpty() ? tr("Unknown") : sData;
    WordWrapLabel *labelText = new WordWrapLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setFixedWidth(LAEBL_TEXT_WIDTH);
    labelText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    labelText->setText(text);
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
    labelText->setMaximumWidth(260);
    QString strText = sData.toString("yyyy/MM/dd HH:mm:ss");
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
