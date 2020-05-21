/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     leiyu <leiyu@live.com>
* Maintainer: leiyu <leiyu@deepin.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "SlideWidget.h"
#include "MainWindow.h"
#include "DocSheet.h"
#include "SlidePlayWidget.h"

#include <DGuiApplicationHelper>
#include <DApplication>

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
    m_docSheet->setCurrentState(Default_State);
    setWidgetState(false);
}

void SlideWidget::initControl()
{
    m_curPageIndex = m_docSheet->currentIndex();
    m_preIndex = m_curPageIndex;

    setMouseTracking(true);
    m_docSheet->setCurrentState(SLIDER_SHOW);
    setWidgetState(true);
    parentWidget()->stackUnder(this);
    this->setGeometry(0, 0, DApplication::desktop()->screenGeometry().width(), DApplication::desktop()->screenGeometry().height());
    connect(parentWidget(), &QObject::destroyed, this, &SlideWidget::onParentDestroyed);

    m_slidePlayWidget = new SlidePlayWidget(this);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalPreBtnClicked,  this, &SlideWidget::onPreBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalPlayBtnClicked, this, &SlideWidget::onPlayBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalNextBtnClicked, this, &SlideWidget::onNextBtnClicked);
    connect(m_slidePlayWidget, &SlidePlayWidget::signalExitBtnClicked, this, &SlideWidget::onExitBtnClicked);
    m_slidePlayWidget->move((DApplication::desktop()->screenGeometry().width() - 270) / 2, DApplication::desktop()->screenGeometry().height() - 100);

    QImage limage, rimage;
    int maxPix = qMin(this->width(), this->height() - 20);
    m_docSheet->getImageMax(m_preIndex, limage, maxPix);
    m_docSheet->getImageMax(m_curPageIndex, rimage, maxPix);
    m_lpixmap = drawImage(limage);
    m_rpixmap = drawImage(rimage);
}

void SlideWidget::initImageControl()
{
    m_imageTimer = new QTimer(this);
    m_imageTimer->setInterval(3000);
    connect(m_imageTimer, &QTimer::timeout, this, &SlideWidget::onImageShowTimeOut);
    onPlayBtnClicked();

    m_imageAnimation = new QPropertyAnimation(this, "");
    m_imageAnimation->setEasingCurve(QEasingCurve::Linear);
    m_imageAnimation->setDuration(1000);
    connect(m_imageAnimation, SIGNAL(valueChanged(const QVariant &)), this, SLOT(onImagevalueChanged(const QVariant &)));
}

void SlideWidget::onImagevalueChanged(const QVariant &variant)
{
    m_offset = variant.toInt();
    update();
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

    if (m_blefttoright) {
        painter.drawPixmap(m_offset, 0, m_lpixmap);
        painter.drawPixmap(m_offset - width(), 0, m_rpixmap);
    } else {
        painter.drawPixmap(m_offset, 0, m_lpixmap);
        painter.drawPixmap(m_offset + width(), 0, m_rpixmap);
    }
}

void SlideWidget::mouseMoveEvent(QMouseEvent *event)
{
    DWidget::mouseMoveEvent(event);
    m_slidePlayWidget->showControl();
}
(
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
                parentWidget()->showMaximized();
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
    m_docSheet->closeSlideWidget();
}

void SlideWidget::playImage()
{
    QImage limage, rimage;
    int maxPix = qMin(this->width(), this->height() - 20);

    if (m_preIndex < m_curPageIndex) {
        m_blefttoright = false;
        m_imageAnimation->setStartValue(0);
        m_imageAnimation->setEndValue(0 - width());

        m_docSheet->getImageMax(m_preInde(x, limage, maxPix);
        m_docSheet->getImageMax(m_curPageIndex, rimage, maxPix);

    } else {
        m_blefttoright = true;
        m_imageAnimation->setStartValue(0);
        m_imageAnimation->setEndValue(width());

        m_docSheet->getImageMax(m_preIndex, limage, maxPix);
        m_docSheet->getImageMax(m_curPageIndex, rimage, maxPix);
    }
    m_lpixmap = drawImage(limage);
    m_rpixmap = drawImage(rimage);
    m_imageAnimation->start();
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

QPixmap SlideWidget::drawImage(const QImage &srcImage)
{
    QPixmap pixmap(this->width(), this->height());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.drawImage((pixmap.width() - srcImage.width()) / 2.0, (pixmap.height() - srcImage.height()) / 2.0, srcImage);
    return pixmap;
}
