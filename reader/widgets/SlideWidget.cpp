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
    qDebug() << "SlideWidget created, initial page:" << docsheet->currentIndex();
    initControl();
    initImageControl();
    show();
    qDebug() << "SlideWidget initialized";
}

SlideWidget::~SlideWidget()
{
    // qDebug() << "SlideWidget destroyed";
    setWidgetState(false);
}

void SlideWidget::initControl()
{
    qDebug() << "Initializing slide widget";
    m_offset = 0;
    m_curPageIndex = m_docSheet->currentIndex();
    m_preIndex = m_curPageIndex;

    setMouseTracking(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWidgetState(true);
    if (parentWidget()) {
        qDebug() << "Parent widget is valid";
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
    qInfo() << QString("screenNum:%1 width:%2 height:%3").arg(screenNum).arg(width).arg(height);
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
        qDebug() << "Normal mode";
        m_slidePlayWidget->setFixedWidth(250);
        m_slidePlayWidget->move((width - m_slidePlayWidget->width()) / 2, height - 100);
    } else {
        qDebug() << "Compact mode";
        m_slidePlayWidget->setFixedWidth(194);
        m_slidePlayWidget->move((width - m_slidePlayWidget->width()) / 2, height - 100);
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
            qDebug() << "Normal mode";
            m_slidePlayWidget->setFixedWidth(250);
            m_slidePlayWidget->move((width - m_slidePlayWidget->width()) / 2, height - 100);
        } else {
            qDebug() << "Compact mode";
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
    qDebug() << "Initializing image control";
    m_imageTimer = new QTimer(this);
    m_imageTimer->setInterval(3000);
    connect(m_imageTimer, &QTimer::timeout, this, &SlideWidget::onImageShowTimeOut);
    //    m_imageTimer->start();

    m_imageAnimation = new QPropertyAnimation(this, "", this);
    m_imageAnimation->setEasingCurve(QEasingCurve::Linear);
    m_imageAnimation->setDuration(500);
    connect(m_imageAnimation, &QPropertyAnimation::valueChanged, this, &SlideWidget::onImagevalueChanged);
    connect(m_imageAnimation, &QPropertyAnimation::finished, this, &SlideWidget::onImageAniFinished);
    qDebug() << "Image control initialized";
}

void SlideWidget::onImagevalueChanged(const QVariant &variant)
{
    qDebug() << "Image value changed:" << variant.toInt();
    m_offset = variant.toInt();
    update();
}

void SlideWidget::onImageAniFinished()
{
    qDebug() << "Image animation finished";
    bool autoplay = m_slidePlayWidget->getPlayStatus();
    if (autoplay) {
        qDebug() << "Autoplay is true, starting image timer";
        m_imageTimer->start();
    } else {
        qDebug() << "Autoplay is false, stopping image timer";
        m_imageTimer->stop();
    }
}

void SlideWidget::onParentDestroyed()
{
    qDebug() << "Parent destroyed";
    m_parentIsDestroyed = true;
}

void SlideWidget::paintEvent(QPaintEvent *event)
{
    // qDebug() << "Painting slide widget";
    DWidget::paintEvent(event);

    QPainter painter(this);

    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    painter.fillRect(this->rect(), Qt::black);

    int roffset = 0;

    if (m_blefttoright) {
        // qDebug() << "Left to right";
        roffset = m_offset - width();
        painter.drawPixmap(m_offset, 0, m_lpixmap);
        painter.drawPixmap(roffset, 0, m_rpixmap);
    } else {
        // qDebug() << "Right to left";
        roffset = m_offset + width();
        painter.drawPixmap(m_offset, 0, m_lpixmap);
        painter.drawPixmap(roffset, 0, m_rpixmap);
    }

    if (m_rpixmap.isNull()) {
        // qDebug() << "RPixmap is null";
        if (m_curPageIndex == m_preIndex) roffset = 0;
        m_loadSpinner->move(roffset + (this->width() - m_loadSpinner->width()) / 2, (this->height() - m_loadSpinner->height()) / 2);
        m_loadSpinner->start();
        m_loadSpinner->show();
    } else {
        qDebug() << "RPixmap is not null";
        m_loadSpinner->hide();
        m_loadSpinner->stop();
    }
}

void SlideWidget::mouseMoveEvent(QMouseEvent *event)
{
    // qDebug() << "Mouse move event";
    DWidget::mouseMoveEvent(event);
    m_slidePlayWidget->showControl();
}

void SlideWidget::setWidgetState(bool full)
{
    qDebug() << "Setting widget state, fullscreen:" << full;
    if (m_parentIsDestroyed || !parentWidget())
        return;

    if (full) {
        qDebug() << "Setting widget state to fullscreen";
        m_nOldState = parentWidget()->windowState(); // 保存幻灯片播放前的windowstate
        if (!m_nOldState.testFlag(Qt::WindowFullScreen)) {
            parentWidget()->setWindowState(parentWidget()->windowState() | Qt::WindowFullScreen);
        }
    } else if (parentWidget()->windowState().testFlag(Qt::WindowFullScreen) && !m_nOldState.testFlag(Qt::WindowFullScreen)) {
        qDebug() << "Setting widget state to normal";
        parentWidget()->setWindowState(parentWidget()->windowState() & ~Qt::WindowFullScreen);
        m_nOldState = parentWidget()->windowState();
    }
}

void SlideWidget::onPreBtnClicked()
{
    qDebug() << "Previous button clicked, current page:" << m_curPageIndex;
    //已到第一页时，再次点击上一页按钮
    if(m_curPageIndex <= 0) {
        m_slidePlayWidget->updateProcess(m_curPageIndex - 1, m_docSheet->pageCount());
        return;
    }

    m_preIndex = m_curPageIndex;

    m_curPageIndex--;

    if (m_curPageIndex >= m_docSheet->pageCount()) {
        qDebug() << "Current page index is greater than page count, setting to 0";
        m_curPageIndex = 0;
    } else if (m_curPageIndex < 0) {
        qDebug() << "Current page index is less than 0, setting to page count - 1";
        m_curPageIndex = m_docSheet->pageCount() - 1;
    }

    playImage();
    qDebug() << "Previous button clicked end";
}

void SlideWidget::onPlayBtnClicked()
{
    qDebug() << "Play button clicked, new state:" << !m_slidePlayWidget->getPlayStatus();
    if (m_slidePlayWidget->getPlayStatus()) {
        qDebug() << "Play button clicked, play status is true";
        //最后一页点播放时，则返回开始播放
        if(m_curPageIndex >= m_docSheet->pageCount() - 1) {
            m_curPageIndex = 0;
            m_preIndex = 0;
            playImage();
        }
        m_canRestart = true;
        m_imageTimer->start();
    } else {
        qDebug() << "Play button clicked, play status is false";
        m_imageTimer->stop();
    }
    qDebug() << "Play button clicked end";
}

void SlideWidget::onNextBtnClicked()
{
    qDebug() << "Next button clicked, current page:" << m_curPageIndex;
    //已到最后一页时，再次点击下一页按钮
    if(m_curPageIndex >= m_docSheet->pageCount() - 1) {
        qDebug() << "Current page index is greater than page count, setting to 0";
        m_slidePlayWidget->updateProcess(m_curPageIndex + 1, m_docSheet->pageCount());
        return;
    }
    m_preIndex = m_curPageIndex;

    m_curPageIndex++;

    if (m_curPageIndex >= m_docSheet->pageCount()) {
        qDebug() << "Current page index is greater than page count, setting to 0";
        m_curPageIndex = 0;
    } else if (m_curPageIndex < 0) {
        qDebug() << "Current page index is less than 0, setting to page count - 1";
        m_curPageIndex = m_docSheet->pageCount() - 1;
    }

    playImage();
    qDebug() << "Next button clicked end";
}

void SlideWidget::onExitBtnClicked()
{
    qDebug() << "Exit button clicked";
    m_docSheet->closeSlide();
}

void SlideWidget::playImage()
{
    qDebug() << "Playing image, from:" << m_preIndex << "to:" << m_curPageIndex
             << "direction:" << (m_blefttoright ? "right" : "left");
    m_imageAnimation->stop();

    if (m_preIndex <= m_curPageIndex) { //正序切换(包括第一页切到最后一页)
        qDebug() << "Playing image, from:" << m_preIndex << "to:" << m_curPageIndex
                 << "direction:" << "right";
        m_blefttoright = false;
        m_imageAnimation->setStartValue(0);
        m_imageAnimation->setEndValue(0 - width());
    } else { //逆序切换(包括最后一页切到第一页)
        qDebug() << "Playing image, from:" << m_preIndex << "to:" << m_curPageIndex
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
    qDebug() << "Playing image end";
}

void SlideWidget::onImageShowTimeOut()
{
    qDebug() << "Image show timeout, moving to next page";
    m_preIndex = m_curPageIndex;

    m_curPageIndex++;

    if (m_curPageIndex >= m_docSheet->pageCount()) {
        qDebug() << "Current page index is greater than page count, setting to 0";
        if (m_canRestart) {
            m_curPageIndex = 0;

        } else {
            qDebug() << "Current page index is greater than page count, setting to page count - 1";
            m_curPageIndex = m_docSheet->pageCount() - 1;

            m_slidePlayWidget->setPlayStatus(false);

            return;
        }
    }

    m_canRestart = false;

    playImage();
    qDebug() << "Image show timeout end";
}

QPixmap SlideWidget::drawImage(const QPixmap &srcImage)
{
    qDebug() << "Drawing image";
    QPixmap pixmap(static_cast<int>(this->width() * dApp->devicePixelRatio()), static_cast<int>(this->height() * dApp->devicePixelRatio()));
    pixmap.setDevicePixelRatio(dApp->devicePixelRatio());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    qreal iwidth = srcImage.width();
    qreal iheight = srcImage.height();
    painter.drawPixmap(static_cast<int>((pixmap.width() - iwidth) * 0.5 / dApp->devicePixelRatio()),
                       static_cast<int>((pixmap.height() - iheight) * 0.5 / dApp->devicePixelRatio()), srcImage);
    qDebug() << "Drawing image end";
    return pixmap;
}

void SlideWidget::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "Mouse press event";
    DWidget::mousePressEvent(event);
    if (event->button() == Qt::RightButton)
        onExitBtnClicked();
    else if (event->button() == Qt::LeftButton)
        m_slidePlayWidget->showControl();
    // qDebug() << "Mouse press event end";
}

void SlideWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "Mouse release event";
    DWidget::mouseReleaseEvent(event);
    onNextBtnClicked();
}

void SlideWidget::wheelEvent(QWheelEvent *event)
{
    // qDebug() << "Wheel event";
    DWidget::wheelEvent(event);
    if (event->angleDelta().y() > 0) {
        // qDebug() << "Wheel event, angle delta y is greater than 0";
        onPreBtnClicked();
    } else {
        // qDebug() << "Wheel event, angle delta y is less than 0";
        onNextBtnClicked();
    }
}

void SlideWidget::handleKeyPressEvent(const QString &sKey)
{
    // qDebug() << "Handling key press event:" << sKey;
    if (sKey == Dr::key_space) {
        // qDebug() << "Handling key press event, key is space";
        bool autoplay = m_slidePlayWidget->getPlayStatus();
        m_slidePlayWidget->setPlayStatus(!autoplay);
    } else if (sKey == Dr::key_left || sKey == Dr::key_up) {
        // qDebug() << "Handling key press event, key is left or up";
        onPreBtnClicked();
    } else if (sKey == Dr::key_right || sKey == Dr::key_down) {
        // qDebug() << "Handling key press event, key is right or down";
        onNextBtnClicked();
    }
    // qDebug() << "Handling key press event end";
}

void SlideWidget::onFetchImage(int index)
{
    qDebug() << "Fetching image for page:" << index;
    const QPixmap &pix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, index);

    if (!pix.isNull() && qMax(pix.width(), pix.height()) == qMin(this->width() - 40, this->height() - 20)) {
        qDebug() << "Fetching image for page:" << index << "is not null";
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
    qDebug() << "Fetching image for page:" << index << "end";
}

void SlideWidget::onUpdatePageImage(int pageIndex)
{
    qDebug() << "Updating page image:" << pageIndex
             << "current:" << m_curPageIndex << "previous:" << m_preIndex;
    if (pageIndex == m_preIndex) {
        qDebug() << "Updating page image, previous:" << pageIndex;
        const QPixmap &lPix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, pageIndex);
        m_lpixmap = drawImage(lPix);
        update();
    }

    if (pageIndex == m_curPageIndex) {
        qDebug() << "Updating page image, current:" << pageIndex;
        const QPixmap &rPix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, pageIndex);
        m_rpixmap = drawImage(rPix);
        update();
    }
    qDebug() << "Updating page image end";
}
