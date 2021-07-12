/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SlideWidget.h"
#include "MainWindow.h"
#include "DocSheet.h"
#include "SlidePlayWidget.h"
#include "Application.h"
#include "sidebar/threadmanager/ReaderImageThreadPoolManager.h"

#include <DGuiApplicationHelper>
#include <DApplication>
#include <DTitlebar>

#include <QDebug>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QTimer>

SlideWidget::SlideWidget(DocSheet *docsheet)
    : DWidget(MainWindow::windowContainSheet(docsheet))
    , m_docSheet(docsheet)
{
    initControl();
    initImageControl();
    show();
}

SlideWidget::~SlideWidget()
{
    setWidgetState(false);
}

void SlideWidget::initControl()
{
    m_offset = 0;
    m_curPageIndex = m_docSheet->currentIndex();
    m_preIndex = m_curPageIndex;

    setMouseTracking(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWidgetState(true);
    parentWidget()->stackUnder(this);
    this->setGeometry(0, 0, DApplication::desktop()->screenGeometry().width(), DApplication::desktop()->screenGeometry().height());
    connect(parentWidget(), &QObject::destroyed, this, &SlideWidget::onParentDestroyed);

    m_loadSpinner = new DSpinner(this);
    m_loadSpinner->setFixedSize(40, 40);
    m_loadSpinner->stop();
    m_loadSpinner->hide();

    m_slidePlayWidget = new SlidePlayWidget(this);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalPreBtnClicked,  this, &SlideWidget::onPreBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalPlayBtnClicked, this, &SlideWidget::onPlayBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalNextBtnClicked, this, &SlideWidget::onNextBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalExitBtnClicked, this, &SlideWidget::onExitBtnClicked);
    m_slidePlayWidget->move((DApplication::desktop()->screenGeometry().width() - 270) / 2, DApplication::desktop()->screenGeometry().height() - 100);

    onFetchImage(m_curPageIndex);
    onFetchImage(m_preIndex);
}

void SlideWidget::initImageControl()
{
    m_imageTimer = new QTimer(this);
    m_imageTimer->setInterval(3000);
    connect(m_imageTimer, &QTimer::timeout, this, &SlideWidget::onImageShowTimeOut);
    m_imageTimer->start();

    m_imageAnimation = new QPropertyAnimation(this, "", this);
    m_imageAnimation->setEasingCurve(QEasingCurve::Linear);
    m_imageAnimation->setDuration(500);
    connect(m_imageAnimation, &QPropertyAnimation::valueChanged, this, &SlideWidget::onImagevalueChanged);
    connect(m_imageAnimation, &QPropertyAnimation::finished, this, &SlideWidget::onImageAniFinished);
}

void SlideWidget::onImagevalueChanged(const QVariant &variant)
{
    m_offset = variant.toInt();
    update();
}

void SlideWidget::onImageAniFinished()
{
    bool autoplay = m_slidePlayWidget->getPlayStatus();
    if (autoplay) {
        m_imageTimer->start();
    } else {
        m_imageTimer->stop();
    }
}

void SlideWidget::onParentDestroyed()
{
    m_parentIsDestroyed = true;
}

void SlideWidget::paintEvent(QPaintEvent *event)
{
    DWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.fillRect(this->rect(), Qt::black);

    int roffset = 0;
    if (m_blefttoright) {
        roffset = m_offset - width();
        painter.drawPixmap(m_offset, 0, m_lpixmap);
        painter.drawPixmap(roffset, 0, m_rpixmap);
    } else {
        roffset = m_offset + width();
        painter.drawPixmap(m_offset, 0, m_lpixmap);
        painter.drawPixmap(roffset, 0, m_rpixmap);
    }

    if (m_rpixmap.isNull()) {
        if (m_curPageIndex == m_preIndex) roffset = 0;
        m_loadSpinner->move(roffset + (this->width() - m_loadSpinner->width()) / 2, (this->height() - m_loadSpinner->height()) / 2);
        m_loadSpinner->start();
        m_loadSpinner->show();
    } else {
        m_loadSpinner->hide();
        m_loadSpinner->stop();
    }
}

void SlideWidget::mouseMoveEvent(QMouseEvent *event)
{
    DWidget::mouseMoveEvent(event);
    m_slidePlayWidget->showControl();
}

void SlideWidget::setWidgetState(bool full)
{
    if (m_parentIsDestroyed)
        return;

    if (full) {
        m_nOldState = parentWidget()->windowState();
        parentWidget()->setWindowState(Qt::WindowFullScreen);
    } else {
        if (parentWidget()->windowState() == Qt::WindowFullScreen) {
            if (m_nOldState == Qt::WindowMaximized) {
                parentWidget()->showNormal();
                parentWidget()->showMaximized();
            } else if (m_nOldState == Qt::WindowFullScreen) {
                //notTodo;
            } else {
                parentWidget()->showNormal();
            }
            m_nOldState = Qt::WindowNoState;
        }
    }
}

void SlideWidget::onPreBtnClicked()
{
    m_preIndex = m_curPageIndex;
    m_curPageIndex--;
    if (m_curPageIndex < 0) {
        m_curPageIndex = 0;
        return;
    }
    playImage();
}

void SlideWidget::onPlayBtnClicked()
{
    bool autoplay = m_slidePlayWidget->getPlayStatus();
    if (autoplay) {
        m_canRestart = true;
        m_imageTimer->start();
    } else {
        m_imageTimer->stop();
    }
}

void SlideWidget::onNextBtnClicked()
{
    m_preIndex = m_curPageIndex;
    m_curPageIndex++;
    if (m_curPageIndex >= m_docSheet->pagesNumber()) {
        m_curPageIndex = m_docSheet->pagesNumber() - 1;
        return;
    }
    playImage();
}

void SlideWidget::onExitBtnClicked()
{
    m_docSheet->closeSlide();
}

void SlideWidget::playImage()
{
    m_imageAnimation->stop();
    if (m_preIndex < m_curPageIndex) {
        m_blefttoright = false;
        m_imageAnimation->setStartValue(0);
        m_imageAnimation->setEndValue(0 - width());
    } else {
        m_blefttoright = true;
        m_imageAnimation->setStartValue(0);
        m_imageAnimation->setEndValue(width());
    }

    m_lpixmap = m_rpixmap;
    m_rpixmap = QPixmap();

    onFetchImage(m_curPageIndex);
    m_imageAnimation->start();
    m_imageTimer->stop();
}

void SlideWidget::onImageShowTimeOut()
{
    m_preIndex = m_curPageIndex;
    m_curPageIndex++;
    if (m_curPageIndex >= m_docSheet->pagesNumber()) {
        if (m_canRestart) {
            m_curPageIndex = 0;
            m_canRestart = false;
        } else {
            m_curPageIndex = m_docSheet->pagesNumber() - 1;
            m_slidePlayWidget->setPlayStatus(false);
            return;
        }
    }
    m_canRestart = false;
    playImage();
}

QPixmap SlideWidget::drawImage(const QPixmap &srcImage)
{
    QPixmap pixmap(static_cast<int>(this->width() * dApp->devicePixelRatio()), static_cast<int>(this->height() * dApp->devicePixelRatio()));
    pixmap.setDevicePixelRatio(dApp->devicePixelRatio());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    qreal iwidth = srcImage.width();
    qreal iheight = srcImage.height();
    painter.drawPixmap(QRect(static_cast<int>((pixmap.width() - iwidth) * 0.5 / dApp->devicePixelRatio()),
                             static_cast<int>((pixmap.height() - iheight) * 0.5 / dApp->devicePixelRatio()),
                             static_cast<int>(iwidth / dApp->devicePixelRatio()),
                             static_cast<int>(iheight / dApp->devicePixelRatio())), srcImage);

    return pixmap;
}

void SlideWidget::mousePressEvent(QMouseEvent *event)
{
    DWidget::mousePressEvent(event);
    if (event->button() == Qt::RightButton) {
        onExitBtnClicked();
    }
}

void SlideWidget::handleKeyPressEvent(const QString &sKey)
{
    if (sKey == Dr::key_space) {
        bool autoplay = m_slidePlayWidget->getPlayStatus();
        m_slidePlayWidget->setPlayStatus(!autoplay);
    } else if (sKey == Dr::key_left) {
        onPreBtnClicked();
    } else if (sKey == Dr::key_right) {
        onNextBtnClicked();
    }
}

void SlideWidget::onFetchImage(int index)
{
    const QPixmap &pix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, index);
    if (!pix.isNull() && qMax(pix.width(), pix.height()) == qMin(this->width() - 40, this->height() - 20)) {
        onUpdatePageImage(index);
        return;
    }

    ReaderImageParam_t tParam;
    tParam.pageIndex = index;
    tParam.sheet = m_docSheet;
    tParam.receiver = this;
    tParam.maxPixel = qMin(this->width() - 40, this->height() - 20);
    tParam.slotFun = "onUpdatePageImage";
    ReaderImageThreadPoolManager::getInstance()->addgetDocImageTask(tParam);
}

void SlideWidget::onUpdatePageImage(int pageIndex)
{
    if (pageIndex == m_preIndex) {
        const QPixmap &lPix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, pageIndex);
        m_lpixmap = drawImage(lPix);
        update();
    }

    if (pageIndex == m_curPageIndex) {
        const QPixmap &rPix = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_docSheet, pageIndex);
        m_rpixmap = drawImage(rPix);
        update();
    }
}
