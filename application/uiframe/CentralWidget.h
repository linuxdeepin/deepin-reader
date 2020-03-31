/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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

#ifndef FRAME_MAINWIDGET_H
#define FRAME_MAINWIDGET_H

#include "CustomControl/CustomWidget.h"

/**
 * @brief The CentralWidget class
 * @brief   采用　栈式　显示窗口，　当前只显示某一特定窗口
 */

class CentralWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CentralWidget)

public:
    explicit CentralWidget(DWidget *parent = nullptr);
    ~CentralWidget() override;

signals:
    void sigOpenFiles(const QString &);

public:
    int dealWithData(const int &, const QString &) override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

protected:
    void initWidget() override;

public slots:
    void SlotOpenFiles(const QString &);

    void onFilesOpened();

private:
    void initConnections();
    void OnSetCurrentIndex();
    void onShowTip(const QString &);
};

#endif  // MAINSTACKWIDGET_H
