#include "FileAttrWidget.h"
#include <DWidgetUtil>
#include "utils/utils.h"
#include "docview/docummentproxy.h"
#include <DIconButton>
#include "translator/Frame.h"
#include "translator/MainShow.h"

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
    stFileInfo fileInfo;
    DocummentProxy::instance()->docBasicInfo(fileInfo);

    QImage image;
    bool rl = DocummentProxy::instance()->getImage(0, image, 94, 113);
    if (rl) {
        labelImage->setPixmap(QPixmap::fromImage(image));
    }

    QString szTitle = fileInfo.strTheme;
    if (szTitle == "") {
        QString filePath = fileInfo.strFilepath;
        int nLastPos = filePath.lastIndexOf("/");
        nLastPos++;
        szTitle = filePath.mid(nLastPos);
    }
    labelFileName->setText(szTitle);

    setFileInfoPath(fileInfo.strFilepath);

    labelTheme->setText(fileInfo.strTheme);

    labelAuthor->setText(fileInfo.strAuther);

    labelCreator->setText(fileInfo.strCreater);

    setFileInfoTime(fileInfo.CreateTime, fileInfo.ChangeTime);

    labelFormat->setText(fileInfo.strFormat);
    labelPageNumber->setText(QString::number(fileInfo.iNumpages));

    labelBetter->setText(QString::number(fileInfo.boptimization));

    labelSafe->setText(QString::number(fileInfo.bsafe));

    labelPaperSize->setText(QString::number(fileInfo.iWidth) + "x" + QString::number(fileInfo.iHeight) + tr("mm"));

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
    QGridLayout *gridLayout = new QGridLayout;

    createLabel(gridLayout, 0, MainShow::TITLE + ":");
    labelFilePath = createLabel(gridLayout, 1, MainShow::LOCATION + ":");
    labelTheme = createLabel(gridLayout, 2, MainShow::THEME + ":");
    labelAuthor = createLabel(gridLayout, 3, MainShow::AUTHOR + ":");
    labelKeyWord = createLabel(gridLayout, 4, MainShow::KEYWORDS + ":");
    labelProducer = createLabel(gridLayout, 5, MainShow::PRODUCRES + ":");
    labelCreator = createLabel(gridLayout, 6, MainShow::CREATOR + ":");
    labelCreateTime =  createLabel(gridLayout, 7, MainShow::CREATE_TIME + ":");
    labelUpdateTime = createLabel(gridLayout, 8, MainShow::UPDATE_TIME + ":");
    labelFormat = createLabel(gridLayout, 9, MainShow::FORMAT + ":");
    labelPageNumber = createLabel(gridLayout, 10, MainShow::PAGE_NUMBERS + ":");
    labelBetter = createLabel(gridLayout, 11, MainShow::OPTIMIZE + ":");
    labelSafe = createLabel(gridLayout, 12, MainShow::SECURITY + ":");
    labelPaperSize = createLabel(gridLayout, 13, MainShow::PAPER_SIZE + ":");
    labelSize = createLabel(gridLayout, 14, MainShow::FILE_SIZE + ":");

    DWidget *labelWidget = new DWidget(this);
    QVBoxLayout *vbLayout = new QVBoxLayout;
    vbLayout->addWidget(new DLabel(MainShow::FILE_BASIC_INFO));
    vbLayout->addItem(gridLayout);
    labelWidget->setLayout(vbLayout);

    m_pVBoxLayout->addWidget(labelWidget);
}

void FileAttrWidget::initCloseBtn()
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(1);

    DIconButton *closeButton = new DIconButton(DStyle::SP_CloseButton, this);
    closeButton->setToolTip(Frame::BTN_CLOSE);
    connect(closeButton, &DIconButton::clicked, this, &FileAttrWidget::slotBtnCloseClicked);

    layout->addWidget(closeButton);

    m_pVBoxLayout->addItem(layout);
}

void FileAttrWidget::initImageLabel()
{
    labelImage = new DLabel(this);
    labelImage->setFixedSize(QSize(94, 113));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(1);
    layout->addWidget(labelImage);
    layout->addStretch(1);

    labelFileName = new DLabel("", this);
    labelFileName->setAlignment(Qt::AlignCenter);

    QVBoxLayout *vlayout = new QVBoxLayout;
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

//  文件　位置
void FileAttrWidget::setFileInfoPath(const QString &inputPath)
{
    labelFilePath->setText(inputPath);
    int nHeight = labelFilePath->height();
    labelFilePath->setFixedHeight(nHeight * 2);
    labelFilePath->setWordWrap(true);
    labelFilePath->adjustSize();
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
