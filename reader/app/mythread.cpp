/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chendu <chendu@uniontech.com>
*
* Maintainer: chendu <chendu@uniontech.com>
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

#include "mythread.h"
#include "html2pdfconverter.h"

#include <QDebug>
#include <QThread>
#include <QApplication>

MyThread::MyThread(const QString &inputPath, const QString &outputPath, QObject *parent)
    : QObject(parent)
    , m_inputPath(inputPath)
    , m_outputPath(outputPath)
    , m_threadRunInBack(new QThread(this))
    , m_myHtml2PdfConverterThread(new Html2PdfConverter(inputPath, outputPath))
{
    connect(this, &MyThread::threadRun1, m_myHtml2PdfConverterThread, &Html2PdfConverter::run); //使用threadRun1来启动线程
    connect(m_threadRunInBack, &QThread::finished, m_myHtml2PdfConverterThread, &Html2PdfConverter::deleteLater); //线程结束资源释放

    m_myHtml2PdfConverterThread->moveToThread(QApplication::instance()->thread()); //QWebEnginePage加载必须放到主线程处理
}

MyThread::~MyThread()
{
    // 结束线程
    if (m_threadRunInBack->isRunning()) {
        m_threadRunInBack->quit();
        m_threadRunInBack->wait();
    }
}

void MyThread::doHtml2PdfConverterWork()
{
    if (!m_threadRunInBack->isRunning()) {
        QEventLoop loop;
        connect(m_myHtml2PdfConverterThread, &Html2PdfConverter::sigQuit, &loop, &QEventLoop::quit);
        qInfo() << "doHtml2PdfConverterWork";
        m_threadRunInBack->start();
        Q_EMIT threadRun1();
        loop.exec(); //等待Html2PdfConverter处理完成
    }
}
