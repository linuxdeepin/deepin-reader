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
Q_DECLARE_METATYPE(MagnifierInfo_t)

/**
 * @brief The ReadMagnifierManager class
 * 放大镜任务线程
 */
class ReadMagnifierManager: public QThread
{
public:
    explicit ReadMagnifierManager();
    ~ReadMagnifierManager() override;

public:
    /**
     * @brief addTask
     * 添加放大镜任务
     * @param task
     */
    void addTask(const MagnifierInfo_t &task);

protected:
    /**
     * @brief run
     * 具体任务执行接口
     */
    void run() override;

private:
    QMutex m_mutex;
    QList<MagnifierInfo_t> m_tTasklst;
};

/**
 * @brief The BrowserMagniFier class
 * 放大镜控件
 */
class BrowserMagniFier : public QLabel
{
    Q_OBJECT

public:
    BrowserMagniFier(QWidget *parent);

    /**
     * @brief showMagnigierImage
     * 显示放大区域的图片
     * @param viewPoint
     * @param magnifierPos
     * @param scaleFactor
     */
    void showMagnigierImage(QPoint viewPoint, QPoint magnifierPos, double scaleFactor);

    /**
     * @brief updateImage
     * 更新显示的图片
     */
    void updateImage();

private:
    /**
     * @brief setMagniFierImage
     * 设置要显示的图片
     * @param image
     */
    void setMagniFierImage(const QImage &image);

private slots:
    /**
     * @brief onUpdateMagnifierImage
     * 更新放大镜区域内的图片
     * @param task
     * @param image
     */
    void onUpdateMagnifierImage(const MagnifierInfo_t &task, const QImage &image);

private:
    qreal m_lastScaleFactor;
    QPoint m_lastPoint;
    QPoint m_lastScenePoint;

    SheetBrowser *m_brwoser;
    ReadMagnifierManager m_readManager;
};

#endif // BROWSERMAGNIFIER_H
