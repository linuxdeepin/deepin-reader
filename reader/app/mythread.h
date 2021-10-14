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
#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>

class Html2PdfConverter;

/**
 * @brief The MyThread class
 * 处理独立的线程工作
 */
class MyThread : public QObject
{
    Q_OBJECT
public:
    MyThread(const QString &inputPath, const QString &outputPath, QObject *parent = nullptr);
    ~MyThread() override;
    /**
     * @brief doHtml2PdfConverterWork
     * 开始htmltopdf转换工作
     */
    void doHtml2PdfConverterWork();

signals:
    void threadRun1();
    void sigQuit();

private:
    QString m_inputPath; //输入文件html
    QString m_outputPath; //输出文件pdf
    QThread *m_threadRunInBack = nullptr;
    Html2PdfConverter *m_myHtml2PdfConverterThread = nullptr;
};

#endif // MYTHREAD_H
