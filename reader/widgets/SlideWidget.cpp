// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SlideWidget.h"
#include "MainWindow.h"
#include "DocSheet.h"
#include "SlidePlayWidget.h"
#include "Application.h"
#include "ReaderImageThreadPoolManager.h"
#include "ddlog.h"

#include <DGuiApplicationHelper>
#include <DApplication>
#include <DTitlebar>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QDesktopWidget>
#else
#include <QScreen>
#include <QGuiApplication>
#endif

#include <QPropertyAnimation>
#include <QTimer>
#include <QDebug>

SlideWidget::SlideWidget(DocSheet *docsheet)
    : DWidget(MainWindow::windowContainSheet(docsheet))
    , m_docSheet(docsheet)
{
    qCDebug(appLog) << "SlideWidget created, initial page:" << docsheet->currentIndex();
    initControl();
    initImageControl();
    show();
    qCDebug(appLog) << "SlideWidget initialized";
}

SlideWidget::~SlideWidget()
{
    // qCDebug(appLog) << "SlideWidget destroyed";
    setWidgetState(false);
}

void SlideWidget::initControl()
{
    qCDebug(appLog) << "Initializing slide widget";
    m_offset = 0;
    m_curPageIndex = m_docSheet->currentIndex();
    m_preIndex = m_curPageIndex;

    setMouseTracking(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWidgetState(true);
    if (parentWidget()) {
        qCDebug(appLog) << "Parent widget is valid";
        parentWidget()->stackUnder(this);
        connect(parentWidget(), &QObject::destroyed, this, &SlideWidget::onParentDestroyed);
    }
    
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QDesktopWidget *dk = QApplication::desktop();
    int screenNum = dk->screenNumber(parentWidget()); //该应用在哪块屏幕上显示
    int width; //幻灯片宽
    int height; //幻灯片高
    if (screenNum < dApp->screens().count() && screenNum >= 0) {
        width = dApp->screens().at(screenNum)->size().width();
        height = dApp->screens().at(screenNum)->size().height();
    } else {
        //若screenNum不合法，默认主屏幕
        width = dApp->primaryScreen()->size().width();
        height = dApp->primaryScreen()->size().height();
    }
    qCInfo(appLog) << QString("screenNum:%1 width:%2 height:%3").arg(screenNum).arg(width).arg(height);
#else
    QScreen *screen = QApplication::primaryScreen();
    int width = screen->size().width();
    int height = screen->size().height();
#endif

    this->setGeometry(0, 0, width, height);

    m_loadSpinner = new DSpinner(this);
    m_loadSpinner->setFixedSize(40, 40);
    m_loadSpinner->stop();
    m_loadSpinner->hide();

    m_slidePlayWidget = new SlidePlayWidget(this);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalPreBtnClicked,  this, &SlideWidget::onPreBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalPlayBtnClicked, this, &SlideWidget::onPlayBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalNextBtnClicked, this, &SlideWidget::onNextBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalExitBtnClicked, this, &SlideWidget::onExitBtnClicked);
    m_slidePlayWidget->setFixedWidth(250);
    m_slidePlayWidget->move((width - m_slidePlayWidget->width()) / 2, height - 100);
#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::NormalMode) {
        qCDebug(appLog) << "Normal mode";
        m_slidePlayWidget->setFixedWidth(250);
        m_slidePlayWidget->move((width - m_slidePlayWidget->width()) / 2, height - 100);
    } else {
        qCDebug(appLog) << "Compact mode";
        m_slidePlayWidget->setFixedWidth(194);
        m_slidePlayWidget->move((width - m_slidePlayWidget->width()) / 2, height - 100);
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
            qCDebug(appLog) << "Normal mode";
            m_slidePlayWidget->setFixedWidth(250);
            m_slidePlayWidget->move((width - m_slidePlayWidget->width()) / 2, height - 100);
        } else {
            qCDebug(appLog) << "Compact mode";
            m_slidePlayWidget->setFixedWidth(194);
            m_slidePlayWidget->move((width - m_slidePlayWidget->width()) / 2, height - 100);
        }
    });
#endif
    onFetchImage(m_curPageIndex);
    onFetchImage(m_preIndex);
}

void SlideWidget::initImageControl()
{
    qCDebug(appLog) << "Initializing image control";
    m_imageTimer = new QTimer(this);
    m_imageTimer->setInterval(3000);
    connect(m_imageTimer, &QTimer::timeout, this, &SlideWidget::onImageShowTimeOut);
    //    m_imageTimer->start();

    m_imageAnimation = new QPropertyAnimation(this, "", this);
    m_imageAnimation->setEasingCurve(QEasingCurve::Linear);
    m_imageAnimation->setDuration(500);
    connect(m_imageAnimation, &QPropertyAnimation::valueChanged, this, &SlideWidget::onImagevalueChanged);
    connect(m_imageAnimation, &QPropertyAnimation::finished, this, &SlideWidget::onImageAniFinished);
    qCDebug(appLog) << "Image control initialized";
}

void SlideWidget::onImagevalueChanged(const QVariant &variant)
{
    qCDebug(appLog) << "Image value changed:" << variant.toInt();
    m_offset = variant.toInt();
    update();
}

void SlideWidget::onImageAniFinished()
{
    qCDebug(appLog) << "Image animation finished";
    bool autoplay = m_slidePlayWidget->getPlayStatus();
    if (autoplay) {
        qCDebug(appLog) << "Autoplay is true, starting image timer";
        m_imageTimer->start();
    } else {
        qCDebug(appLog) << "Autoplay is false, stopping image timer";
        m_imageTimer->stop();
    }
}

void SlideWidget::onParentDestroyed()
{
    qCDebug(appLog) << "Parent destroyed";
    m_parentIsDestroyed = true;
}

void SlideWidget::paintEvent(QPaintEvent *event)
{
    // qCDebug(appLog) << "Painting slide widget";
    DWidget::paintEvent(event);

    QPainter painter(this);

    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    painter.fillRect(this->rect(), Qt::black);

    int roffset = 0;

    if (m_blefttoright) {
        // qCDebug(appLog) << "Left to right";
        roffset = m_offset - width();
        painter.drawPixmap(m_offset, 0, m_lpixmap);
        painter.drawPixmap(roffset, 0, m_rpixmap);
    } else {
        // qCDebug(appLog) << "Right to left";
        roffset = m_offset + width();
        painter.drawPixmap(m_offset, 0, m_lpixmap);
        painter.drawPixmap(roffset, 0, m_rpixmap);
    }

    if (m_rpixmap.isNull()) {
        // qCDebug(appLog) << "RPixmap is null";
        if (m_curPageIndex == m_preIndex) roffset = 0;
        m_loadSpinner->move(roffset + (this->width() - m_loadSpinner->width()) / 2, (this->height() - m_loadSpinner->height()) / 2);
        m_loadSpinner->start();
        m_loadSpinner->show();
    } else {
        qCDebug(appLog) << "RPixmap is not null";
        m_loadSpinner->hide();
        m_loadSpinner->stop();
    }
}

void SlideWidget::mouseMoveEvent(QMouseEvent *event)
{
    // qCDebug(appLog) << "Mouse move event";
    DWidget::mouseMoveEvent(event);
    m_slidePlayWidget->showControl();
}

void SlideWidget::setWidgetState(bool full)
{
    qCDebug(appLog) << "Setting widget state, fullscreen:" << full;
    if (m_parentIsDestroyed || !parentWidget())
        return;

    if (full) {
        qCDebug(appLog) << "Setting widget state to fullscreen";
        m_nOldState = parentWidget()->windowState(); // 保存幻灯片播放前的windowstate
        if (!m_nOldState.testFlag(Qt::WindowFullScreen)) {
            parentWidget()->setWindowState(parentWidget()->windowState() | Qt::WindowFullScreen);
        }
    } else if (parentWidget()->windowState().testFlag(Qt::WindowFullScreen) && !m_nOldState.testFlag(Qt::WindowFullScreen)) {
        qCDebug(appLog) << "Setting widget state to normal";
        parentWidget()->setWindowState(parentWidget()->windowState() & ~Qt::WindowFullScreen);
        m_nOldState = parentWidget()->windowState();
    }
}

void SlideWidget::onPreBtnClicked()
{
    qCDebug(appLog) << "Previous button clicked, current page:" << m_curPageIndex;
    //已到第一页时，再次点击上一页按钮
    if(m_curPageIndex <= 0) {
        m_slidePlayWidget->updateProcess(m_curPageIndex - 1, m_docSheet->pageCount());
        return;
    }

    m_preIndex = m_curPageIndex;

    m_curPageIndex--;

    if (m_curPageIndex >= m_docSheet->pageCount()) {
        qCDebug(appLog) << "Current page index is greater than page count, setting to 0";
        m_curPageIndex = 0;
    } else if (m_curPageIndex < 0) {
        qCDebug(appLog) << "Current page index is less than 0, setting to page count - 1";
        m_curPageIndex = m_docSheet->pageCount() - 1;
    }

    playImage();
    qCDebug(appLog) << "Previous button clicked end";
}

void SlideWidget::onPlayBtnClicked()
{
    qCDebug(appLog) << "Play button clicked, new state:" << !m_slidePlayWidget->getPlayStatus();
    if (m_slidePlayWidget->getPlayStatus()) {
        qCDebug(appLog) << "Play button clicked, play status is true";
        //最后一页点播放时，则返回开始播放
        if(m_curPageIndex >= m_docSheet->pageCount() - 1) {
            m_curPageIndex = 0;
            m_preIndex = 0;
            playImage();
        }
        m_canRestart = true;
        m_imageTimer->start();
    } else {
        qCDebug(appLog) << "Play button clicked, play status is false";
        m_imageTimer->stop();
    }
    qCDebug(appLog) << "Play button clicked end";
}

void SlideWidget::onNextBtnClicked()
{
    qCDebug(appLog) << "Next button clicked, current page:" << m_curPageIndex;
    //已到最后一页时，再次点击下一页按钮
    if(m_curPageIndex >= m_docSheet->pageCount() - 1) {
        qCDebug(appLog) << "Current page index is greater than page count, setting to 0";
        m_slidePlayWidget->updateProcess(m_curPageIndex + 1, m_docSheet->pageCount());
        return;
    }
    m_preIndex = m_curPageIndex;

    m_curPageIndex++;

    if (m_curPageIndex >= m_docSheet->pageCount()) {
        qCDebug(appLog) << "Current page index is greater than page count, setting to 0";
        m_curPageIndex = 0;
    } else if (m_curPageIndex < 0) {
        qCDebug(appLog) << "Current page index is less than 0, setting to page count - 1";
        m_curPageIndex = m_docSheet->pageCount() - 1;
    }

    playImage();
    qCDebug(appLog) << "Next button clicked end";
}

void SlideWidget::onExitBtnClicked()
{
    qCDebug(appLog) << "Exit button clicked";
    m_docSheet->closeSlide();
}

void SlideWidget::playImage()
{
    qCDebug(appLog) << "Playing image, from:" << m_preIndex << "to:" << m_curPageIndex
             << "direction:" << (m_blefttoright ? "right" : "left");
    m_imageAnimation->stop();

    if (m_preIndex <= m_curPageIndex) { //正序切换(包括第一页切到最后一页)
        qCDebug(appLog) << "Playing image, from:" << m_preIndex << "to:" << m_curPageIndex
                 << "direction:" << "right";
        m_blefttoright = false;
        m_imageAnimation->setStartValue(0);
        m_imageAnimation->setEndValue(0 - width());
    } else { //逆序切换(包括最后一页切到第一页)
        qCDebug(appLog) << "Playing image, from:" << m_preIndex << "to:" << m_curPageIndex
                 << "direction:" << "left";
        m_blefttoright = true;
        m_imageAnimation->setStartValue(0);
        m_imageAnimation->setEndValue(width());
    }

    m_lpixmap = m_rpixmap;
    m_rpixmap = QPixmap();

    m_slidePlayWidget->updateProcess(m_curPageIndex, m_docSheet->pageCount());
    onFetchImage(m_curPageIndex);
    m_imageAnimation->start();
    m_imageTimer->stop();
    qCDebug(appLog) << "Playing image end";
}

void SlideWidget::onImageShowTimeOut()
{
    qCDebug(appLog) << "Image show timeout, moving to next page";
    m_preIndex = m_curPageIndex;

    m_curPageIndex++;

    if (m_curPageIndex >= m_docSheet->pageCount()) {
        qCDebug(appLog) << "Current page index is greater than page count, setting to 0";
        if (m_canRestart) {
            m_curPageIndex = 0;

        } else {
            qCDebug(appLog) << "Current page index is greater than page count, setting to page count - 1";
            m_curPageIndex = m_docSheet->pageCount() - 1;

            m_slidePlayWidget->setPlayStatus(false);

            return;
        }
    }

    m_canRestart = false;

    playImage();
    qCDebug(appLog) << "Image show timeout end";
}

QPixmap SlideWidget::drawImage(const QPixmap &srcImage)
{
    qCDebug(appLog) << "Drawing image";
    QPixmap pixmap(static_cast<int>(this->width() * dApp->devicePixelRatio()), static_cast<int>(this->height() * dApp->devicePixelRatio()));
    pixmap.setDevicePixelRatio(dApp->devicePixelRatio());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    qreal iwidth = srcImage.width();
    qreal iheight = srcImage.height();
    painter.drawPixmap(static_cast<int>((pixmap.width() - iwidth) * 0.5 / dApp->devicePixelRatio()),
                       static_cast<int>((pixmap.height() - iheight) * 0.5 / dApp->devicePixelRatio()), srcImage);
    qCDebug(appLog) << "Drawing image end";
    return pixmap;
}

void SlideWidget::mousePressEvent(QMouseEvent *event)
{
    // qCDebug(appLog) << "Mouse press event";
    DWidget::mousePressEvent(event);
    if (event->button() == Qt::RightButton)
        onExitBtnClicked();
    else if (event->button() == Qt::LeftButton)
        m_slidePlayWidget->showControl();
    // qCDebug(appLog) << "Mouse press event end";
}

void SlideWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // qCDebug(appLog) << "Mouse release event";
    DWidget::mouseReleaseEvent(event);
    onNextBtnClicked();
}

void SlideWidget::wheelEvent(QWheelEvent *event)
{
    // qCDebug(appLog) << "Wheel event";
    DWidget::wheelEvent(event);
    if (event->angleDelta().y() > 0) {
        // qCDebug(appLog) << "Wheel event, angle delta y is greater than 0";
        onPreBtnClicked();
    } else {
        // qCDebug(appLog) << "Wheel event, angle delta y is less than 0";
        onNextBtnClicked();
    }
}

void SlideWidget::handleKeyPressEvent(const QString &sKey)
{
    // qCDebug(appLog) << "Handling key press event:" << sKey;
    if (sKey == Dr::key_space) {
        // qCDebug(appLog) << "Handling key press event, key is space";
        bool autoplay = m_slidePlayWidget->getPlayStatus();
        m_slidePlayWidget->setPlayStatus(!autoplay);
    } else if (sKey == Dr::key_left || sKey == Dr::key_up) {
        // qCDebug(appLog) << "Handling key press event, key is left or up";
        onPreBtnClicked();
    } else if (sKey == Dr::key_right || sKey == Dr::key_down) {
        // qCDebug(appLog) << "Handling key press event, key is right or down";
        onNextBtnClicked();
    }
    // qCDebug(appLog) << "Handling key press event end";
}

void SlideWidget::onFetchImage(int index)
{
    qCDebug(appLog) << "Fetching image for page:" << index;
    const QPixmap &pix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, index);

    if (!pix.isNull() && qMax(pix.width(), pix.height()) == qMin(this->width() - 40, this->height() - 20)) {
        qCDebug(appLog) << "Fetching image for page:" << index << "is not null";
        onUpdatePageImage(index);
        return;
    }

    ReaderImageParam_t tParam;
    tParam.pageIndex = index;
    tParam.sheet = m_docSheet;
    tParam.receiver = this;
    tParam.maxPixel = qMin(this->width() - 40, this->height() - 20);
    tParam.boundedRect = QSize(this->width(), this->height());
    tParam.slotFun = "onUpdatePageImage";
    ReaderImageThreadPoolManager::getInstance()->addgetDocImageTask(tParam);
    qCDebug(appLog) << "Fetching image for page:" << index << "end";
}

void SlideWidget::onUpdatePageImage(int pageIndex)
{
    qCDebug(appLog) << "Updating page image:" << pageIndex
             << "current:" << m_curPageIndex << "previous:" << m_preIndex;
    if (pageIndex == m_preIndex) {
        qCDebug(appLog) << "Updating page image, previous:" << pageIndex;
        const QPixmap &lPix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, pageIndex);
        m_lpixmap = drawImage(lPix);
        update();
    }

    if (pageIndex == m_curPageIndex) {
        qCDebug(appLog) << "Updating page image, current:" << pageIndex;
        const QPixmap &rPix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, pageIndex);
        m_rpixmap = drawImage(rPix);
        update();
    }
    qCDebug(appLog) << "Updating page image end";
}
