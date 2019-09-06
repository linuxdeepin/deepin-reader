#include "FileAttrWidget.h"
#include <DWidgetUtil>
#include <QDebug>

FileAttrWidget::FileAttrWidget(CustomWidget *parent)
    : CustomWidget("FileAttrWidget", parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(QSize(400, 500));
    setAttribute(Qt::WA_ShowModal, true); //  模态对话框， 属性设置

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);

    initWidget();

    //  桌面 中心显示
    Dtk::Widget::moveToCenter(this);
}

//  各个 对应的 label 赋值
void FileAttrWidget::setFileAttr() {}

void FileAttrWidget::showScreenCenter()
{
    Dtk::Widget::moveToCenter(this);
    this->show();
}

void FileAttrWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bClickDown = true;
        //获得鼠标的初始位置
        mouseStartPoint = event->globalPos();
        //mouseStartPoint = event->pos();
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
        //QPoint distance = event->pos() - mouseStartPoint;
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

    createLabel(gridLayout, 0, tr("Title:"));
    createLabel(gridLayout, 1, tr("Location:"));
    createLabel(gridLayout, 2, tr("Theme:"));
    createLabel(gridLayout, 3, tr("Author:"));
    createLabel(gridLayout, 4, tr("Keywords:"));
    createLabel(gridLayout, 5, tr("Producers:"));
    createLabel(gridLayout, 6, tr("Creator:"));
    createLabel(gridLayout, 7, tr("Create Time:"));
    createLabel(gridLayout, 8, tr("Update Time:"));
    createLabel(gridLayout, 9, tr("Format:"));
    createLabel(gridLayout, 10, tr("Page's Number:"));
    createLabel(gridLayout, 11, tr("Optimize:"));
    createLabel(gridLayout, 12, tr("Security:"));
    createLabel(gridLayout, 13, tr("Paper Size:"));
    createLabel(gridLayout, 14, tr("File Size:"));

    DWidget *labelWidget = new DWidget();
    labelWidget->setLayout(gridLayout);

    m_pVBoxLayout->addWidget(labelWidget);
}

void FileAttrWidget::initCloseBtn()
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(1);

    DPushButton *closeBtn = new DPushButton("", this);
    closeBtn->setToolTip(tr("close"));
    connect(closeBtn, &DPushButton::clicked, this, &FileAttrWidget::slotBtnCloseClicked);

    layout->addWidget(closeBtn);

    m_pVBoxLayout->addItem(layout);
}

void FileAttrWidget::initImageLabel()
{
    DLabel *imagelabel = new DLabel(this);
    imagelabel->setObjectName("image");
    imagelabel->setFixedSize(QSize(94, 113));
    imagelabel->setPixmap(QPixmap(":/resources/image/maganifier.svg"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(1);
    layout->addWidget(imagelabel);
    layout->addStretch(1);

    DLabel *nameLabel = new DLabel("11111111111", this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setObjectName("imageName");

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addStretch(1);
    vlayout->addItem(layout);
    vlayout->addWidget(nameLabel);
    vlayout->addStretch(1);

    m_pVBoxLayout->addItem(vlayout);
}

void FileAttrWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName)
{
    DLabel *label = new DLabel(objName, this);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setFixedWidth(100);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    labelText->setObjectName(objName);
    labelText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(labelText, index, 1);

    m_labelList.append(labelText);
}

void FileAttrWidget::slotBtnCloseClicked()
{
    this->close();
}

int FileAttrWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
