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
#ifndef SLIDEPLAYWIDGET_H
#define SLIDEPLAYWIDGET_H

#include <DFloatingWidget>
#include <DIconButton>

#include <QTimer>

DWIDGET_USE_NAMESPACE
class SlidePlayWidget : public DFloatingWidget
{
    Q_OBJECT
public:
    explicit SlidePlayWidget(QWidget *parent = nullptr);

    bool getPlayStatus();
    void showControl();
    void setPlayStatus(bool play);

signals:
    void signalPreBtnClicked();
    void signalPlayBtnClicked();
    void signalNextBtnClicked();
    void signalExitBtnClicked();

private:
    void initControl();
    DIconButton *createBtn(const QString &strname);
    void playStatusChanged();

private slots:
    void onTimerout();
    void onPreClicked();
    void onPlayClicked();
    void onNextClicked();
    void onExitClicked();

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    bool m_autoPlay = true;;
    QTimer m_timer;
    DIconButton *m_playBtn;
};

#endif // SLIDEPLAYWIDGET_H
