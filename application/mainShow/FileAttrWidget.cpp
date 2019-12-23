#include "FileAttrWidget.h"

#include <DFontSizeManager>
#include <DFrame>
#include <DWindowCloseButton>
#include <QFileInfo>
#include "AttrScrollWidget.h"
#include "controller/DataManager.h"
#include "frame/DocummentFileHelper.h"
#include "CustomControl/DFMGlobal.h"
#include "subjectObserver/ModuleHeader.h"
#include "subjectObserver/MsgHeader.h"
#include "controller/NotifySubject.h"
#include"controller/MsgSubject.h"

FileAttrWidget::FileAttrWidget(DWidget *parent)
    : DAbstractDialog(parent)
{
    setAttribute(Qt::WA_ShowModal, true);  //  模态对话框， 属性设置
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(QSize(300, 642));
    //  setWindowOpacity(0.95);
    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 10);
    this->setLayout(m_pVBoxLayout);

    initWidget();
    if (NotifySubject::getInstance())
        NotifySubject::getInstance()->addObserver(this);
    if (MsgSubject::getInstance())
        MsgSubject::getInstance()->addObserver(this);
}

FileAttrWidget::~FileAttrWidget()
{
    if (NotifySubject::getInstance())
        NotifySubject::getInstance()->removeObserver(this);
    if (MsgSubject::getInstance())
        MsgSubject::getInstance()->removeObserver(this);
}

//  各个 对应的 label 赋值
void FileAttrWidget::setFileAttr()
{
    auto dproxy = DocummentFileHelper::instance();
    if (nullptr == dproxy) {
        return;
    }

    QImage image;
    bool rl = dproxy->getImage(0, image, 94, 113);
    if (rl) {
        if (frameImage) {
            QPixmap pixmap = QPixmap::fromImage(image);
            frameImage->setBackgroundPix(pixmap);
        }
    }

    QString filePath = DataManager::instance()->strOnlyFilePath();
    QFileInfo info(filePath);
    QString szTitle = info.fileName();

    addTitleFrame(szTitle);

    m_pVBoxLayout->addSpacing(40);

    DFrame *infoframe = new DFrame(this);
//    infoframe->setFrameShape(QFrame::NoFrame);
    auto frameLayout = new QVBoxLayout;
    frameLayout->setSpacing(0);
    frameLayout->setContentsMargins(10, 0, 10, 0);
    infoframe->setLayout(frameLayout);

    auto scroll = new DScrollArea;
//    QPalette palette = scroll->viewport()->palette();
//    palette.setBrush(QPalette::Background, Qt::NoBrush);
//    scroll->viewport()->setPalette(palette);

    scroll->viewport()->setContentsMargins(0, 0, 0, 0);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidget(infoframe);
    scroll->setWidgetResizable(true);

    QVBoxLayout *scrolllayout = new QVBoxLayout;
    scrolllayout->setSpacing(0);
    scrolllayout->setContentsMargins(10, 0, 10, 0);
    scrolllayout->addWidget(scroll);

    qobject_cast<QVBoxLayout *>(scroll->widget()->layout())->setContentsMargins(0, 0, 0, 0);
    qobject_cast<QVBoxLayout *>(scroll->widget()->layout())->insertWidget(0, new AttrScrollWidget);

    m_pVBoxLayout->addLayout(scrolllayout, 10);
}

void FileAttrWidget::addTitleFrame(const QString &sData)
{
    QFrame *m_textShowFrame = new QFrame(this);

    QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
    QString t = DFMGlobal::elideText(sData, QSize(260, 60), QTextOption::WrapAnywhere, font,
                                     Qt::ElideMiddle, 0);
    QStringList labelTexts = t.split("\n");
    const int maxLineCount = 3;

    int textHeight = 0;
    QVBoxLayout *textShowLayout = new QVBoxLayout;
    for (int i = 0; i < labelTexts.length(); i++) {
        if (i > (maxLineCount - 1)) {
            break;
        }
        QString labelText = labelTexts.at(i);
        QLabel *label = new QLabel(labelText, m_textShowFrame);
        DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);

        label->setAlignment(Qt::AlignHCenter);
        QHBoxLayout *hLayout = new QHBoxLayout;

        textHeight += label->fontInfo().pixelSize() + 10;

        hLayout->addStretch(1);
        hLayout->addWidget(label);
        if (i < (labelTexts.length() - 1) && i != (maxLineCount - 1)) {
            if (label->fontMetrics().width(labelText) > (300 - 10)) {
                label->setFixedWidth(300);
            }
        } else {
            // the final line of file name label, with a edit btn.
            if (labelTexts.length() >= maxLineCount) {
                for (int idx = i + 1; idx < labelTexts.length(); idx++) {
                    labelText += labelTexts.at(idx);
                }
            }

            if (label->fontMetrics().width(labelText) > 300 &&
                    labelTexts.length() >= maxLineCount) {
                labelText = label->fontMetrics().elidedText(labelText, Qt::ElideMiddle, 260);
            }
            label->setText(labelText);
            hLayout->addSpacing(2);
        }
        textShowLayout->addLayout(hLayout);
        hLayout->addStretch(1);
    }

    textShowLayout->setContentsMargins(0, 6, 0, 0);
    textShowLayout->setSpacing(0);
    m_textShowFrame->setLayout(textShowLayout);
    textShowLayout->addStretch();

    m_textShowFrame->setFixedHeight(textHeight);

    m_pVBoxLayout->addWidget(m_textShowFrame);
}

void FileAttrWidget::showScreenCenter()
{
    setFileAttr();

    Dtk::Widget::moveToCenter(this);
    this->show();
}

int FileAttrWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_NOTIFY_KEY_MSG == msgType && KeyStr::g_esc == msgContent)
        close();
    return 0;
}

void FileAttrWidget::sendMsg(const int &, const QString &msgContent)
{

}

void FileAttrWidget::notifyMsg(const int &, const QString &msgContent)
{

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
    layout->addStretch(0);

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
    frameImage = new /*ImageFrame*/ ImageLabel(this);
    frameImage->setFixedSize(98, 117);

    auto vlayout = new QVBoxLayout;
    vlayout->setAlignment(Qt::AlignCenter);
    vlayout->setContentsMargins(0, 0, 0, 0);
    //    vlayout->setSpacing(10);
    vlayout->addWidget(/*labelImage*/ frameImage);

    m_pVBoxLayout->addItem(vlayout);
}

void FileAttrWidget::slotBtnCloseClicked()
{
    this->close();
}
