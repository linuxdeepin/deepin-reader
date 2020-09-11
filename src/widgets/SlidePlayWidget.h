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
#ifndef SLIDEPLAYWIDGET_H
#define SLIDEPLAYWIDGET_H

#include <DFloatingWidget>
#include <DIconButton>

#include <QTimer>

DWIDGET_USE_NAMESPACE
/**
 * @brief The SlidePlayWidget class
 * 幻灯片播放控件
 */
class SlidePlayWidget : public DFloatingWidget
{
    Q_OBJECT
public:
    explicit SlidePlayWidget(QWidget *parent = nullptr);

    /**
     * @brief getPlayStatus
     * 播放状态
     * @return
     */
    bool getPlayStatus();

    /**
     * @brief showControl
     * 显示播放控件
     */
    void showControl();

    /**
     * @brief setPlayStatus
     * 设置播放状态
     * @param play
     */
    void setPlayStatus(bool play);

signals:
    /**
     * @brief signalPreBtnClicked
     * 上一页按钮点击
     */
    void signalPreBtnClicked();

    /**
     * @brief signalPlayBtnClicked
     * 播放按钮点击
     */
    void signalPlayBtnClicked();

    /**
     * @brief signalNextBtnClicked
     * 下一页按钮点击
     */
    void signalNextBtnClicked();

    /**
     * @brief signalExitBtnClicked
     * 退出按钮点击
     */
    void signalExitBtnClicked();

private:
    /**
     * @brief initControl
     * 初始化控件
     */
    void initControl();

    /**
     * @brief createBtn
     * 创建按钮
     * @param strname
     * @return
     */
    DIconButton *createBtn(const QString &strname);

    /**
     * @brief playStatusChanged
     * 播放状态变换
     */
    void playStatusChanged();

private slots:
    /**
     * @brief onTimerout
     * 播放定时器超时
     */
    void onTimerout();

    /**
     * @brief onPreClicked
     * 上一页按钮点击
     */
    void onPreClicked();

    /**
     * @brief onPlayClicked
     * 播放按钮点击
     */
    void onPlayClicked();

    /**
     * @brief onNextClicked
     * 下一页按钮点击
     */
    void onNextClicked();

    /**
     * @brief onExitClicked
     * 播放按钮点击
     */
    void onExitClicked();

protected:
    /**
     * @brief enterEvent
     * 鼠标进入事件
     * @param event
     */
    void enterEvent(QEvent *event) override;

    /**
     * @brief leaveEvent
     * 鼠标离开事件
     * @param event
     */
    void leaveEvent(QEvent *event) override;

    /**
     * @brief mousePressEvent
     * 鼠标点击事件
     * @param event
     */
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_autoPlay = true;
    QTimer m_timer;
    DIconButton *m_playBtn;
};

#endif // SLIDEPLAYWIDGET_H
