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
#include "BrowserMagniFier.h"
#include "SheetBrowser.h"
#include "BrowserPage.h"
#include "Application.h"

#include <QPainter>
#include <QIcon>
#include <QMutexLocker>
#include <QMetaObject>
#include <QPainterPath>

ReadMagnifierManager::ReadMagnifierManager(QWidget *parent) : QThread(parent)
{
    m_parent = parent;
}

ReadMagnifierManager::~ReadMagnifierManager()
{
    this->wait();
}

void ReadMagnifierManager::addTask(const MagnifierInfo_t &task)
{
    m_tTasklst << task;
    if (!this->isRunning()) {
        this->start();
    }
}

void ReadMagnifierManager::run()
{
    while (m_tTasklst.size() > 0 && m_parent->isVisible()) {
        MagnifierInfo_t task = m_tTasklst.last();
        m_tTasklst.clear();

        if (task.page) {
            const QImage &image = task.page->getImagePoint(task.scaleFactor, task.mousePos);
            QMetaObject::invokeMethod(task.target, task.slotFun.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(const MagnifierInfo_t &, task), Q_ARG(const QImage &, image));
        }
    }
}

BrowserMagniFier::BrowserMagniFier(QWidget *parent)
    : QLabel(parent)
{
    m_readManager = new ReadMagnifierManager(this);
    m_brwoser = dynamic_cast<SheetBrowser *>(parent);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_DeleteOnClose);
    setAutoFillBackground(false);
    resize(244, 244);
    show();
}

void BrowserMagniFier::updateImage()
{
    QPointF point = m_lastScenePoint;
    BrowserPage *page = m_brwoser->getBrowserPageForPoint(point);
    if (page) {
        const QImage &image = page->getCurImagePoint(point);
        setMagniFierImage(image);
    } else {
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
}

void BrowserMagniFier::showMagnigierImage(QPoint mousePos, QPoint magnifierPos, double scaleFactor)
{
    scaleFactor += 2;
    m_lastScenePoint = mousePos;

    QPointF ponitf = mousePos;
    BrowserPage *page = m_brwoser->getBrowserPageForPoint(ponitf);

    if (page) {
        const QImage &image = page->getCurImagePoint(ponitf);
        setMagniFierImage(image);
    } else {
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
}

void BrowserMagniFier::onUpdateMagnifierImage(const MagnifierInfo_t &task, const QImage &image)
{
    if (task.mousePos == m_lastPoint && qFuzzyCompare(task.scaleFactor, m_lastScaleFactor))
        setMagniFierImage(image);
}

void BrowserMagniFier::setMagniFierImage(const QImage &image)
{
    QPixmap pix(static_cast<int>(this->width() * dApp->devicePixelRatio()), static_cast<int>(this->height() * dApp->devicePixelRatio()));
    pix.setDevicePixelRatio(dApp->devicePixelRatio());
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.save();
    QPainterPath clippath;
    clippath.addRoundedRect(17, 17, 210, 210, 105, 105);
    painter.setClipPath(clippath);
    if (!image.isNull()) {
        painter.drawImage(0, 0, image.scaled(static_cast<int>(240 * dApp->devicePixelRatio()), static_cast<int>(240 * dApp->devicePixelRatio()), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(this->rect(), Qt::white);
    }
    painter.restore();
    painter.drawPixmap(this->rect(), QIcon::fromTheme(QString("dr_") + "maganifier").pixmap(QSize(this->width(), this->height())));
    setPixmap(pix);
}
