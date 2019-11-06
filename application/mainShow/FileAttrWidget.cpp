#include "FileAttrWidget.h"

#include "docview/docummentproxy.h"

FileAttrWidget::FileAttrWidget(DWidget *parent)
    : DAbstractDialog(parent)
{
    setAttribute(Qt::WA_ShowModal, true); //  模态对话框， 属性设置
    setFixedSize(QSize(360, 650));

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);

    initWidget();
}

//  各个 对应的 label 赋值
void FileAttrWidget::setFileAttr()
{
    auto dproxy = DocummentProxy::instance();
    if (nullptr == dproxy) {
        return;
    }

    stFileInfo fileInfo;
    dproxy->docBasicInfo(fileInfo);

    QImage image;
    bool rl = dproxy->getImage(0, image, 94, 113);
    if (rl) {
        labelImage->setPixmap(QPixmap::fromImage(image));
    }

    QFileInfo info(fileInfo.strFilepath);
    QString szTitle = info.fileName();

    labelFileName->setText(szTitle);

    labelFilePath->setText(fileInfo.strFilepath);
    labelFilePath->setWordWrap(true);

    labelTheme->setText(fileInfo.strTheme);

    labelAuthor->setText(fileInfo.strAuther);
    labelKeyWord->setText(fileInfo.strKeyword);
    labelProducer->setText(fileInfo.strProducter);
    labelCreator->setText(fileInfo.strCreater);

    setFileInfoTime(fileInfo.CreateTime, fileInfo.ChangeTime);

    labelFormat->setText(fileInfo.strFormat);
    labelPageNumber->setText(QString::number(fileInfo.iNumpages));

    labelBetter->setText(QString::number(fileInfo.boptimization));

    labelSafe->setText(QString::number(fileInfo.bsafe));

    labelPaperSize->setText(QString::number(fileInfo.iWidth) + "x" + QString::number(fileInfo.iHeight) + "mm");

    labelSize->setText(Utils::getInputDataSize(fileInfo.size));
}

void FileAttrWidget::showScreenCenter()
{
    setFileAttr();

    Dtk::Widget::moveToCenter(this);
    this->show();
}

void FileAttrWidget::initWidget()
{
    m_pVBoxLayout->addStretch(1);

    initCloseBtn();

    initImageLabel();

    initLabels();

    m_pVBoxLayout->addStretch(1);
}

//  初始化 所有的label 显示
void FileAttrWidget::initLabels()
{
    auto gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);

    labelFilePath = createLabel(gridLayout, 0, tr("Location") + ":");
    labelTheme = createLabel(gridLayout, 1, tr("Theme") + ":");
    labelAuthor = createLabel(gridLayout, 2, tr("Author") + ":");
    labelKeyWord = createLabel(gridLayout, 3, tr("Keywords") + ":");
    labelProducer = createLabel(gridLayout, 4, tr("Producers") + ":");
    labelCreator = createLabel(gridLayout, 5, tr("Creator") + ":");
    labelCreateTime =  createLabel(gridLayout, 6, tr("Create Time") + ":");
    labelUpdateTime = createLabel(gridLayout, 7, tr("Update Time") + ":");
    labelFormat = createLabel(gridLayout, 8, tr("Format") + ":");
    labelPageNumber = createLabel(gridLayout, 9, tr("Page's Number") + ":");
    labelBetter = createLabel(gridLayout, 10, tr("Optimize") + ":");
    labelSafe = createLabel(gridLayout, 11, tr("Security") + ":");
    labelPaperSize = createLabel(gridLayout, 12, tr("Paper Size") + ":");
    labelSize = createLabel(gridLayout, 13, tr("File Size") + ":");

    auto vbLayout = new QVBoxLayout;
    vbLayout->addWidget(new DLabel(tr("file basic info")));
    vbLayout->addItem(gridLayout);

    auto childDlg = new DFrame();
    childDlg->setLayout(vbLayout);
    m_pVBoxLayout->addWidget(childDlg);
}

void FileAttrWidget::initCloseBtn()
{
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch(1);

    auto closeButton = new DIconButton(DStyle::SP_CloseButton, this);
    closeButton->setToolTip(tr("close"));
    connect(closeButton, &DIconButton::clicked, this, &FileAttrWidget::slotBtnCloseClicked);

    layout->addWidget(closeButton);

    m_pVBoxLayout->addItem(layout);
}

void FileAttrWidget::initImageLabel()
{
    labelImage = new DLabel(this);
    labelImage->setAlignment(Qt::AlignCenter);

    labelFileName = new DLabel("", this);
    labelFileName->setAlignment(Qt::AlignCenter);

    auto vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(0, 6, 0, 30);
    vlayout->addWidget(labelImage);
    vlayout->addWidget(labelFileName);

    m_pVBoxLayout->addItem(vlayout);
}

DLabel *FileAttrWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName)
{
    DLabel *label = new DLabel(objName, this);
    label->setAlignment(Qt::AlignTop);
    label->setFixedWidth(100);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    labelText->setAlignment(Qt::AlignTop);
    layout->addWidget(labelText, index, 1);

    return  labelText;
}

//  文件时间设置
void FileAttrWidget::setFileInfoTime(const QDateTime &CreateTime, const QDateTime &ChangeTime)
{
    QString sYearText = tr("year"), sMonthText = tr("month"), sDayText = tr("day");
    QString sHourText = tr("hour"), sMinText = tr("minute"), sSecondText = tr("second");

    QString sYear = CreateTime.toString("yyyy"), sMonth = CreateTime.toString("MM"), sDay = CreateTime.toString("dd");
    QString sWeekday = CreateTime.toString("ddd");
    QString sHour = CreateTime.toString("HH"), sMin = CreateTime.toString("mm"), sSec = CreateTime.toString("ss");

    QString sDate = sYear + sYearText + sMonth + sMonthText + sDay + sDayText;
    QString sTime = sHour + sHourText + sMin + sMinText + sSec + sSecondText;

    sDate = CreateTime.toString("yyyy年MM月dd日");
    sTime = CreateTime.toString("HH时mm分ss秒");
    labelCreateTime->setText(QString("%1 %2 %3").arg(sDate).arg(sWeekday).arg(sTime));

    sDate = ChangeTime.toString("yyyy年MM月dd日");
    sWeekday = ChangeTime.toString("ddd");
    sTime = ChangeTime.toString("HH时mm分ss秒");
    labelUpdateTime->setText(QString("%1 %2 %3").arg(sDate).arg(sWeekday).arg(sTime));
}

void FileAttrWidget::slotBtnCloseClicked()
{
    this->close();
}
