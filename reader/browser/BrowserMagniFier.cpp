// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BrowserMagniFier.h"
#include "SheetBrowser.h"
#include "BrowserPage.h"
#include "Application.h"

#include <QPainter>
#include <QDebug>
#include <QIcon>
#include <QMutexLocker>
#include <QMetaObject>
#include <QPainterPath>

ReadMagnifierManager::ReadMagnifierManager(QWidget *parent) : QThread(parent)
{
    // qDebug() << "ReadMagnifierManager created";
    m_parent = parent;
}

ReadMagnifierManager::~ReadMagnifierManager()
{
    // qDebug() << "ReadMagnifierManager destroyed";
    this->wait();
}

void ReadMagnifierManager::addTask(const MagnifierInfo_t &task)
{
    qDebug() << "ReadMagnifierManager::addTask";
    m_tTasklst << task;

    if (!this->isRunning()) {
        qInfo() << "Starting ReadMagnifierManager thread";
        this->start();
    }
    qDebug() << "ReadMagnifierManager::addTask() - Add task completed";
}

void ReadMagnifierManager::run()
{
    qDebug() << "ReadMagnifierManager::run() - Starting thread run loop";
    while (m_tTasklst.size() > 0 && m_parent->isVisible()) {
        // qDebug() << "ReadMagnifierManager::run() - Processing task, queue size:" << m_tTasklst.size();
        MagnifierInfo_t task = m_tTasklst.last();
        m_tTasklst.clear();

        if (task.page) {
            // qDebug() << "ReadMagnifierManager::run() - Getting image from page";
            const QImage &image = task.page->getImagePoint(task.scaleFactor, task.mousePos);
            QMetaObject::invokeMethod(task.target, task.slotFun.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(const MagnifierInfo_t &, task), Q_ARG(const QImage &, image));
        }

        msleep(100);
    }
    qDebug() << "ReadMagnifierManager::run() - Thread run loop completed";
}

BrowserMagniFier::BrowserMagniFier(SheetBrowser *parent)
    : QLabel(parent)
{
    qDebug() << "BrowserMagniFier created";
    m_readManager = new ReadMagnifierManager(this);

    m_brwoser = parent;

    setAttribute(Qt::WA_TranslucentBackground);

    setAttribute(Qt::WA_TransparentForMouseEvents);

    setAttribute(Qt::WA_DeleteOnClose);

    setAutoFillBackground(false);

    resize(244, 244);

    show();
    qDebug() << "BrowserMagniFier::BrowserMagniFier() - Constructor completed";
}

BrowserMagniFier::~BrowserMagniFier()
{
    // qDebug() << "BrowserMagniFier destroyed";
    m_readManager->wait();
}

void BrowserMagniFier::updateImage()
{
    // qDebug() << "BrowserMagniFier::updateImage";
    QPointF point = m_lastScenePoint;

    BrowserPage *page = m_brwoser->getBrowserPageForPoint(point);

    if (page) {
        // qDebug() << "BrowserMagniFier::updateImage() - Found page, getting current image";
        const QImage &image = page->getCurImagePoint(point);
        setMagniFierImage(image);
    } else {
        // qDebug() << "BrowserMagniFier::updateImage() - No page found, setting empty image";
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
    // qDebug() << "BrowserMagniFier::updateImage() - Image update completed";
}

void BrowserMagniFier::showMagnigierImage(QPoint mousePos, QPoint magnifierPos, double scaleFactor)
{
    // qDebug() << "BrowserMagniFier::showMagnigierImage at" << mousePos << "with scale" << scaleFactor;
    scaleFactor += 2;

    m_lastScenePoint = mousePos;

    QPointF ponitf = mousePos;

    BrowserPage *page = m_brwoser->getBrowserPageForPoint(ponitf);

    if (page) {
        // qDebug() << "BrowserMagniFier::showMagnigierImage() - Found page, getting current image";
        const QImage &image = page->getCurImagePoint(ponitf);

        setMagniFierImage(image);
    } else {
        // qDebug() << "BrowserMagniFier::showMagnigierImage() - No page found, setting empty image";
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
    // qDebug() << "BrowserMagniFier::showMagnigierImage() - Show magnifier image completed";
}

void BrowserMagniFier::onUpdateMagnifierImage(const MagnifierInfo_t &task, const QImage &image)
{
    // qDebug() << "BrowserMagniFier::onUpdateMagnifierImage";
    if (task.mousePos == m_lastPoint && qFuzzyCompare(task.scaleFactor, m_lastScaleFactor)) {
        // qDebug() << "BrowserMagniFier::onUpdateMagnifierImage() - Task matches current state, setting image";
        setMagniFierImage(image);
    }
    // qDebug() << "BrowserMagniFier::onUpdateMagnifierImage() - Update magnifier image completed";
}

void BrowserMagniFier::setMagniFierImage(const QImage &image)
{
    // qDebug() << "BrowserMagniFier::setMagniFierImage() - Starting set magnifier image";
    QPixmap pix(static_cast<int>(this->width() * dApp->devicePixelRatio()), static_cast<int>(this->height() * dApp->devicePixelRatio()));

    pix.fill(Qt::transparent);

    QPainter painter(&pix);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter.save();

    QPainterPath clippath;

    clippath.addRoundedRect(17, 17, 210, 210, 105, 105);

    painter.setClipPath(clippath);

    if (!image.isNull()) {
        // qDebug() << "BrowserMagniFier::setMagniFierImage() - Drawing valid image";
        QImage im = image;

        pix.setDevicePixelRatio(1);

        im.setDevicePixelRatio(1);

        painter.drawImage(0, 0, im.scaled(static_cast<int>(240 * dApp->devicePixelRatio()), static_cast<int>(240 * dApp->devicePixelRatio()), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // qDebug() << "BrowserMagniFier::setMagniFierImage() - Drawing white background for null image";
        painter.fillRect(this->rect(), Qt::white);
    }
    painter.restore();

    painter.drawPixmap(this->rect(), QIcon::fromTheme(QString("dr_") + "maganifier").pixmap(QSize(this->width(), this->height())));

    pix.setDevicePixelRatio(dApp->devicePixelRatio());

    setPixmap(pix);
    // qDebug() << "BrowserMagniFier::setMagniFierImage() - Set magnifier image completed";
}
