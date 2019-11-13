#include "FileAttrWidget.h"

#include "frame/DocummentFileHelper.h"
#include "AttrScrollWidget.h"
#include <DWindowCloseButton>
#include <QFileInfo>
#include "controller/DataManager.h"

FileAttrWidget::FileAttrWidget(DWidget *parent)
    : DAbstractDialog(parent)
{
    setAttribute(Qt::WA_ShowModal, true); //  模态对话框， 属性设置
    setFixedSize(QSize(300, 642));

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);

    initWidget();
}

//  各个 对应的 label 赋值
void FileAttrWidget::setFileAttr()
{
    auto dproxy = DocummentFileHelper::instance();
    if (nullptr == dproxy) {
        return;
    }

    QImage image;
    bool rl = dproxy->getImage(0, image, 94, 114);
    if (rl) {
        labelImage->setPixmap(QPixmap::fromImage(image));
    }

    QString filePath = DataManager::instance()->strOnlyFilePath();
    QFileInfo info(filePath);
    QString szTitle = info.fileName();

    labelFileName->setText(szTitle);

    m_pVBoxLayout->addWidget(new AttrScrollWidget);
}

void FileAttrWidget::showScreenCenter()
{
    setFileAttr();

    Dtk::Widget::moveToCenter(this);
    this->show();
}

void FileAttrWidget::initWidget()
{
    initCloseBtn();

    initImageLabel();
}

void FileAttrWidget::initCloseBtn()
{
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch(1);

    auto closeButton = new DWindowCloseButton(this);
    closeButton->setFixedSize(QSize(50, 50));
    closeButton->setIconSize(QSize(50, 50));
    closeButton->setToolTip(tr("close"));
    connect(closeButton, &DWindowCloseButton::clicked, this, &FileAttrWidget::slotBtnCloseClicked);

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

void FileAttrWidget::slotBtnCloseClicked()
{
    this->close();
}
