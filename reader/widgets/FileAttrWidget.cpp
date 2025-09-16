// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FileAttrWidget.h"
#include "AttrScrollWidget.h"
#include "WordWrapLabel.h"
#include "Application.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "Utils.h"
#include "ddlog.h"
#include <QDebug>

#include <DFontSizeManager>
#include <DFrame>
#include <DScrollArea>
#include <DWindowCloseButton>
#include <DWidgetUtil>

#include <QFileInfo>
#include <QVBoxLayout>
#include <DGuiApplicationHelper>

class ImageWidget : public DWidget
{
public:
    explicit ImageWidget(DWidget *parent)
        : DWidget(parent)
    {
        // qCDebug(appLog) << "ImageWidget created, parent:" << parent;
    }

    void setPixmap(const QPixmap &pixmap)
    {
        // qCDebug(appLog) << "Setting pixmap for ImageWidget";
        if (!pixmap.isNull()) {
            m_pixmap = pixmap;
            update();
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_pixmap;
};

void ImageWidget::paintEvent(QPaintEvent *event)
{
    // qCDebug(appLog) << "Painting ImageWidget";
    DWidget::paintEvent(event);
    QPainter painter(this);
    painter.drawPixmap(static_cast<int>(this->width() * 0.5 - m_pixmap.width() * 0.5 / m_pixmap.devicePixelRatioF()),  static_cast<int>(this->height() * 0.5 - m_pixmap.height() * 0.5 / m_pixmap.devicePixelRatioF()), m_pixmap);
}

FileAttrWidget::FileAttrWidget(DWidget *parent)
    : DAbstractDialog(parent)
{
    qCDebug(appLog) << "FileAttrWidget created, parent:" << parent;
    setFixedSize(QSize(300, 622));
    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 10);
    this->setLayout(m_pVBoxLayout);
    initWidget();
}

FileAttrWidget::~FileAttrWidget()
{
    // qCDebug(appLog) << "FileAttrWidget destroyed";
}

void FileAttrWidget::setFileAttr(DocSheet *sheet)
{
    qCDebug(appLog) << "Setting file attributes for sheet:" << (sheet ? "valid" : "null");
    if (sheet == nullptr)
        return;

    QSizeF rect = sheet->pageSizeByIndex(0); // 获取首页的真实大小
    qreal aspectRatio = rect.height() / rect.width(); // 原始图片的高宽比
    int tmpWidth = static_cast<int>(94 * dApp->devicePixelRatio());
    int tmpHeight = static_cast<int>(tmpWidth * aspectRatio);
    if (tmpHeight > static_cast<int>(113 * dApp->devicePixelRatio())) { // 使打印的图片保持原始图片的宽高比
        qCDebug(appLog) << "tmpHeight > static_cast<int>(113 * dApp->devicePixelRatio())";
        tmpHeight = static_cast<int>(113 * dApp->devicePixelRatio());
        tmpWidth = static_cast<int>(tmpHeight / aspectRatio);
    }
    QImage image = sheet->getImage(0, tmpWidth, tmpHeight);
    image.setDevicePixelRatio(dApp->devicePixelRatio());

    if (!image.isNull() && frameImage) {
        qCDebug(appLog) << "image is not null and frameImage is not null";
        const QPixmap &pix = Utils::roundQPixmap(QPixmap::fromImage(image), 8);
        frameImage->setPixmap(pix);
    }

    QFileInfo info(sheet->filePath());
    QString szTitle = info.fileName();

    addTitleFrame(szTitle);

    m_pVBoxLayout->addSpacing(10);

    DFrame *infoframe = new DFrame;
    auto frameLayout = new QVBoxLayout;
    frameLayout->setSpacing(0);
    frameLayout->setContentsMargins(0, 5, 0, 5);
    infoframe->setLayout(frameLayout);

    auto scroll = new DScrollArea(this);
    QPalette palette = scroll->viewport()->palette();
    palette.setBrush(QPalette::Window, Qt::NoBrush);
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
    qCDebug(appLog) << "File attributes set, image loaded:" << (!image.isNull() ? "success" : "failed");
}

void FileAttrWidget::addTitleFrame(const QString &sData)
{
    qCDebug(appLog) << "Adding title frame";
    WordWrapLabel *labelText = new WordWrapLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setFixedWidth(this->width());
    labelText->setContentsMargins(20, 20, 20, 20);
    labelText->setAlignment(Qt::AlignCenter);
    labelText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    labelText->setText(labelText->fontMetrics().elidedText(sData, Qt::ElideMiddle, 680));
    m_pVBoxLayout->addWidget(labelText);
    qCDebug(appLog) << "Adding title frame end";
}

void FileAttrWidget::showScreenCenter()
{
    qCDebug(appLog) << "Showing file attribute widget at screen center";
    Dtk::Widget::moveToCenter(this);
    this->show();
}

void FileAttrWidget::initWidget()
{
    qCDebug(appLog) << "Initializing file attribute widget";
    initCloseBtn();
    initImageLabel();
}

void FileAttrWidget::initCloseBtn()
{
    qCDebug(appLog) << "Initializing close button";
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch(0);

    auto closeButton = new DWindowCloseButton(this);
    closeButton->setFixedSize(QSize(50, 50));
    closeButton->setIconSize(QSize(50, 50));
    connect(closeButton, &DWindowCloseButton::clicked, this, &DAbstractDialog::close);

    layout->addWidget(closeButton);
#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::NormalMode) {
        closeButton->setFixedSize(QSize(50, 50));
        closeButton->setIconSize(QSize(50, 50));
    } else {
        closeButton->setFixedSize(QSize(40, 40));
        closeButton->setIconSize(QSize(40, 40));
        qCDebug(appLog) << "File attribute widget initialization completed";
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
            closeButton->setFixedSize(QSize(50, 50));
            closeButton->setIconSize(QSize(50, 50));
        } else {
            closeButton->setFixedSize(QSize(40, 40));
            closeButton->setIconSize(QSize(40, 40));
        }
    });
#endif
    m_pVBoxLayout->addItem(layout);
    qCDebug(appLog) << "Initializing close button end";
}

void FileAttrWidget::initImageLabel()
{
    qCDebug(appLog) << "Initializing image label";
    frameImage = new ImageWidget(this);
    frameImage->setFixedSize(98, 117);

    auto vlayout = new QVBoxLayout;
    vlayout->setAlignment(Qt::AlignCenter);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(frameImage);

    m_pVBoxLayout->addItem(vlayout);
    qCDebug(appLog) << "Initializing image label end";
}
