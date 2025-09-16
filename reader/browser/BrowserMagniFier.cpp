// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BrowserMagniFier.h"
#include "SheetBrowser.h"
#include "BrowserPage.h"
#include "Application.h"
#include "ddlog.h"

#include <QPainter>
#include <QDebug>
#include <QIcon>
#include <QMutexLocker>
#include <QMetaObject>
#include <QPainterPath>

ReadMagnifierManager::ReadMagnifierManager(QWidget *parent) : QThread(parent)
{
    // qCDebug(appLog) << "ReadMagnifierManager created";
    m_parent = parent;
}

ReadMagnifierManager::~ReadMagnifierManager()
{
    // qCDebug(appLog) << "ReadMagnifierManager destroyed";
    this->wait();
}

void ReadMagnifierManager::addTask(const MagnifierInfo_t &task)
{
    qCDebug(appLog) << "ReadMagnifierManager::addTask";
    m_tTasklst << task;

    if (!this->isRunning()) {
        qCInfo(appLog) << "Starting ReadMagnifierManager thread";
        this->start();
    }
    qCDebug(appLog) << "ReadMagnifierManager::addTask() - Add task completed";
}

void ReadMagnifierManager::run()
{
    qCDebug(appLog) << "ReadMagnifierManager::run() - Starting thread run loop";
    while (m_tTasklst.size() > 0 && m_parent->isVisible()) {
        // qCDebug(appLog) << "ReadMagnifierManager::run() - Processing task, queue size:" << m_tTasklst.size();
        MagnifierInfo_t task = m_tTasklst.last();
        m_tTasklst.clear();

        if (task.page) {
            // qCDebug(appLog) << "ReadMagnifierManager::run() - Getting image from page";
            const QImage &image = task.page->getImagePoint(task.scaleFactor, task.mousePos);
            QMetaObject::invokeMethod(task.target, task.slotFun.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(const MagnifierInfo_t &, task), Q_ARG(const QImage &, image));
        }

        msleep(100);
    }
    qCDebug(appLog) << "ReadMagnifierManager::run() - Thread run loop completed";
}

BrowserMagniFier::BrowserMagniFier(SheetBrowser *parent)
    : QLabel(parent)
{
    qCDebug(appLog) << "BrowserMagniFier created";
    m_readManager = new ReadMagnifierManager(this);

    m_brwoser = parent;

    setAttribute(Qt::WA_TranslucentBackground);

    setAttribute(Qt::WA_TransparentForMouseEvents);

    setAttribute(Qt::WA_DeleteOnClose);

    setAutoFillBackground(false);

    resize(244, 244);

    show();
    qCDebug(appLog) << "BrowserMagniFier::BrowserMagniFier() - Constructor completed";
}

BrowserMagniFier::~BrowserMagniFier()
{
    // qCDebug(appLog) << "BrowserMagniFier destroyed";
    m_readManager->wait();
}

void BrowserMagniFier::updateImage()
{
    // qCDebug(appLog) << "BrowserMagniFier::updateImage";
    QPointF point = m_lastScenePoint;

    BrowserPage *page = m_brwoser->getBrowserPageForPoint(point);

    if (page) {
        // qCDebug(appLog) << "BrowserMagniFier::updateImage() - Found page, getting current image";
        const QImage &image = page->getCurImagePoint(point);
        setMagniFierImage(image);
    } else {
        // qCDebug(appLog) << "BrowserMagniFier::updateImage() - No page found, setting empty image";
        setMagniFierImage(QImage());
    }

    MagnifierInfo_t task;

    task.page = page;

    task.target = this;

    task.slotFun = "onUpdateMagnifierImage";

    task.mousePos = point.toPoint();

    task.scaleFactor = m_lastScaleFactor;

    m_readManager->addTask(task);

    m_lastPoint = point.toPoint();
    // qCDebug(appLog) << "BrowserMagniFier::updateImage() - Image update completed";
}

void BrowserMagniFier::showMagnigierImage(QPoint mousePos, QPoint magnifierPos, double scaleFactor)
{
    // qCDebug(appLog) << "BrowserMagniFier::showMagnigierImage at" << mousePos << "with scale" << scaleFactor;
    scaleFactor += 2;

    m_lastScenePoint = mousePos;

    QPointF ponitf = mousePos;

    BrowserPage *page = m_brwoser->getBrowserPageForPoint(ponitf);

    if (page) {
        // qCDebug(appLog) << "BrowserMagniFier::showMagnigierImage() - Found page, getting current image";
        const QImage &image = page->getCurImagePoint(ponitf);

        setMagniFierImage(image);
    } else {
        // qCDebug(appLog) << "BrowserMagniFier::showMagnigierImage() - No page found, setting empty image";
        setMagniFierImage(QImage());
    }

    move(QPoint(magnifierPos.x() - 122, magnifierPos.y() - 122));

    MagnifierInfo_t task;

    task.page = page;

    task.target = this;

    task.slotFun = "onUpdateMagnifierImage";

    task.mousePos = ponitf.toPoint();

    task.scaleFactor = scaleFactor;

    m_readManager->addTask(task);

    m_lastPoint = ponitf.toPoint();

    m_lastScaleFactor = scaleFactor;
    // qCDebug(appLog) << "BrowserMagniFier::showMagnigierImage() - Show magnifier image completed";
}

void BrowserMagniFier::onUpdateMagnifierImage(const MagnifierInfo_t &task, const QImage &image)
{
    // qCDebug(appLog) << "BrowserMagniFier::onUpdateMagnifierImage";
    if (task.mousePos == m_lastPoint && qFuzzyCompare(task.scaleFactor, m_lastScaleFactor)) {
        // qCDebug(appLog) << "BrowserMagniFier::onUpdateMagnifierImage() - Task matches current state, setting image";
        setMagniFierImage(image);
    }
    // qCDebug(appLog) << "BrowserMagniFier::onUpdateMagnifierImage() - Update magnifier image completed";
}

void BrowserMagniFier::setMagniFierImage(const QImage &image)
{
    // qCDebug(appLog) << "BrowserMagniFier::setMagniFierImage() - Starting set magnifier image";
    QPixmap pix(static_cast<int>(this->width() * dApp->devicePixelRatio()), static_cast<int>(this->height() * dApp->devicePixelRatio()));

    pix.fill(Qt::transparent);

    QPainter painter(&pix);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter.save();

    QPainterPath clippath;

    clippath.addRoundedRect(17, 17, 210, 210, 105, 105);

    painter.setClipPath(clippath);

    if (!image.isNull()) {
        // qCDebug(appLog) << "BrowserMagniFier::setMagniFierImage() - Drawing valid image";
        QImage im = image;

        pix.setDevicePixelRatio(1);

        im.setDevicePixelRatio(1);

        painter.drawImage(0, 0, im.scaled(static_cast<int>(240 * dApp->devicePixelRatio()), static_cast<int>(240 * dApp->devicePixelRatio()), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // qCDebug(appLog) << "BrowserMagniFier::setMagniFierImage() - Drawing white background for null image";
        painter.fillRect(this->rect(), Qt::white);
    }
    painter.restore();

    painter.drawPixmap(this->rect(), QIcon::fromTheme(QString("dr_") + "maganifier").pixmap(QSize(this->width(), this->height())));

    pix.setDevicePixelRatio(dApp->devicePixelRatio());

    setPixmap(pix);
    // qCDebug(appLog) << "BrowserMagniFier::setMagniFierImage() - Set magnifier image completed";
}
