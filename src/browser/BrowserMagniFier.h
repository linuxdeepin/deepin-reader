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
#ifndef BROWSERMAGNIFIER_H
#define BROWSERMAGNIFIER_H

#include <QLabel>
#include <QThread>
#include <QMutex>

class BrowserPage;
class SheetBrowser;
typedef struct MagnifierInfo_t {
    QObject *target = nullptr;
    QString slotFun = "";

    QPoint mousePos;
    double scaleFactor;
    BrowserPage *page;
} MagnifierInfo_t;
Q_DECLARE_METATYPE(MagnifierInfo_t);

class ReadMagnifierManager: public QThread
{
public:
    explicit ReadMagnifierManager();
    ~ReadMagnifierManager() override;

public:
    void addTask(const MagnifierInfo_t &task);

protected:
    void run() override;

private:
    QMutex m_mutex;
    QList<MagnifierInfo_t> m_tTasklst;
};

class BrowserMagniFier : public QLabel
{
    Q_OBJECT

public:
    BrowserMagniFier(QWidget *parent);

    void showMagnigierImage(QPoint viewPoint, QPoint magnifierPos, double scaleFactor);

    void updateImage();

private:
    void setMagniFierImage(const QImage &image);

private slots:
    void onUpdateMagnifierImage(const MagnifierInfo_t &task, const QImage &image);

private:
    qreal m_lastScaleFactor;
    QPoint m_lastPoint;
    QPoint m_lastScenePoint;

    SheetBrowser *m_brwoser;
    ReadMagnifierManager m_readManager;
};

#endif // BROWSERMAGNIFIER_H
