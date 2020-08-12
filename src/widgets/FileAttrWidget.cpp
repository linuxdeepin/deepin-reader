#include "FileAttrWidget.h"

#include <DFontSizeManager>
#include <DFrame>
#include <DScrollArea>
#include <DWindowCloseButton>
#include <QFileInfo>
#include <DWidgetUtil>
#include <QVBoxLayout>

#include "AttrScrollWidget.h"
#include "widgets/WordWrapLabel.h"


#include "Application.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "Utils.h"

class ImageWidget : public DWidget
{
public:
    ImageWidget(DWidget *parent)
        : DWidget(parent) {

          };

    void setPixmap(const QPixmap &pixmap)
    {
        if (!pixmap.isNull()) {
            m_pixmap = pixmap;
            update();
        }
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        DWidget::paintEvent(event);
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter.drawPixmap((this->width() - m_pixmap.width()) / 2, (this->height() - m_pixmap.height()) / 2, m_pixmap);
    }

private:
    QPixmap m_pixmap;
};

FileAttrWidget::FileAttrWidget(DWidget *parent)
    : DAbstractDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal, true);
    setFixedSize(QSize(300, 642));
    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 10);
    this->setLayout(m_pVBoxLayout);
    initWidget();
}

FileAttrWidget::~FileAttrWidget()
{
}

void FileAttrWidget::setFileAttr(DocSheet *sheet)
{
    if (sheet == nullptr)
        return;

    QImage image;
    bool rl = sheet->getImage(0, image, 94, 113, Qt::KeepAspectRatio);
    if (rl) {
        if (frameImage) {
            const QPixmap &pix = Utils::roundQPixmap(QPixmap::fromImage(image), 8);
            frameImage->setPixmap(pix);
        }
    }

    QFileInfo info(sheet->filePath());
    QString szTitle = info.fileName();

    addTitleFrame(szTitle);

    m_pVBoxLayout->addSpacing(40);

    DFrame *infoframe = new DFrame;
    auto frameLayout = new QVBoxLayout;
    frameLayout->setSpacing(0);
    frameLayout->setContentsMargins(0, 5, 0, 5);
    infoframe->setLayout(frameLayout);

    auto scroll = new DScrollArea(this);
    QPalette palette = scroll->viewport()->palette();
    palette.setBrush(QPalette::Background, Qt::NoBrush);
    scroll->viewport()->setPalette(palette);

    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidget(new AttrScrollWidget(sheet, this));
    scroll->setWidgetResizable(true);

    frameLayout->addWidget(scroll);

    QVBoxLayout *scrolllayout = new QVBoxLayout;
    scrolllayout->setSpacing(0);
    scrolllayout->setContentsMargins(10, 0, 10, 0);
    scrolllayout->addWidget(infoframe);

    m_pVBoxLayout->addLayout(scrolllayout, 1);
}

void FileAttrWidget::addTitleFrame(const QString &sData)
{
    WordWrapLabel *labelText = new WordWrapLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setFixedWidth(this->width());
    labelText->setMargin(20);
    labelText->setAlignment(Qt::AlignCenter);
    labelText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    labelText->setText(sData);
    m_pVBoxLayout->addWidget(labelText);
}

void FileAttrWidget::showScreenCenter()
{
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
    layout->addStretch(0);

    auto closeButton = new DWindowCloseButton(this);
    closeButton->setFixedSize(QSize(50, 50));
    closeButton->setIconSize(QSize(50, 50));
    connect(closeButton, &DWindowCloseButton::clicked, this, &DAbstractDialog::close);

    layout->addWidget(closeButton);

    m_pVBoxLayout->addItem(layout);
}

void FileAttrWidget::initImageLabel()
{
    frameImage = new ImageWidget(this);
    frameImage->setFixedSize(98, 117);

    auto vlayout = new QVBoxLayout;
    vlayout->setAlignment(Qt::AlignCenter);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(frameImage);

    m_pVBoxLayout->addItem(vlayout);
}
