#include "FileAttrWidget.h"
#include <DWidgetUtil>
#include "utils/utils.h"
#include "docview/docummentproxy.h"
#include <DIconButton>
#include <QFileInfo>

FileAttrWidget::FileAttrWidget(CustomWidget *parent)
    : CustomWidget("FileAttrWidget", parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(QSize(400, 600));
    setAttribute(Qt::WA_ShowModal, true); //  模态对话框， 属性设置

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
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

    QString szTitle = "";
    dproxy->title(szTitle);
    if (szTitle == "") {
        szTitle = info.baseName();
    }
    labelFileName->setText(szTitle);
    labelFileTitle->setText(szTitle);

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

void FileAttrWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bClickDown = true;
        //获得鼠标的初始位置
        mouseStartPoint = event->globalPos();
        //获得窗口的初始位置
        windowTopLeftPoint = this->frameGeometry().topLeft();
    }
    CustomWidget::mousePressEvent(event);
}

void FileAttrWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bClickDown) {
        //获得鼠标移动的距离
        QPoint distance = event->globalPos() - mouseStartPoint;
        //改变窗口的位置
        this->move(windowTopLeftPoint + distance);
    }
    CustomWidget::mouseMoveEvent(event);
}

void FileAttrWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bClickDown = false;
    }
    CustomWidget::mouseReleaseEvent(event);
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
    auto gridLayout = new QGridLayout;
    gridLayout->setSpacing(6);

    labelFileTitle = createLabel(gridLayout, 0, tr("Title") + ":");
    labelFilePath = createLabel(gridLayout, 1, tr("Location") + ":");
    labelTheme = createLabel(gridLayout, 2, tr("Theme") + ":");
    labelAuthor = createLabel(gridLayout, 3, tr("Author") + ":");
    labelKeyWord = createLabel(gridLayout, 4, tr("Keywords") + ":");
    labelProducer = createLabel(gridLayout, 5, tr("Producers") + ":");
    labelCreator = createLabel(gridLayout, 6, tr("Creator") + ":");
    labelCreateTime =  createLabel(gridLayout, 7, tr("Create Time") + ":");
    labelUpdateTime = createLabel(gridLayout, 8, tr("Update Time") + ":");
    labelFormat = createLabel(gridLayout, 9, tr("Format") + ":");
    labelPageNumber = createLabel(gridLayout, 10, tr("Page's Number") + ":");
    labelBetter = createLabel(gridLayout, 11, tr("Optimize") + ":");
    labelSafe = createLabel(gridLayout, 12, tr("Security") + ":");
    labelPaperSize = createLabel(gridLayout, 13, tr("Paper Size") + ":");
    labelSize = createLabel(gridLayout, 14, tr("File Size") + ":");

    auto labelWidget = new DWidget(this);
    auto vbLayout = new QVBoxLayout;
    vbLayout->addWidget(new DLabel(tr("file basic info")));
    vbLayout->addItem(gridLayout);
    labelWidget->setLayout(vbLayout);

    m_pVBoxLayout->addWidget(labelWidget);
}

void FileAttrWidget::initCloseBtn()
{
    auto layout = new QHBoxLayout;
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
    labelImage->setFixedSize(QSize(94, 113));

    auto layout = new QHBoxLayout;
    layout->addStretch(1);
    layout->addWidget(labelImage);
    layout->addStretch(1);

    labelFileName = new DLabel("", this);
    labelFileName->setAlignment(Qt::AlignCenter);

    auto vlayout = new QVBoxLayout;
    vlayout->addStretch(1);
    vlayout->addItem(layout);
    vlayout->addWidget(labelFileName);
    vlayout->addStretch(1);

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
    QString sDate = CreateTime.toString("yyyy年MM月dd日");
    QString sWeekday = CreateTime.toString("ddd");
    QString sTime = CreateTime.toString("HH时mm分ss秒");
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

int FileAttrWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更

    }
    return 0;
}
