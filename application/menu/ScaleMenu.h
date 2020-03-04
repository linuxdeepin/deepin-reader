/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
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
#ifndef SCALEMENU_H
#define SCALEMENU_H

#include "CustomControl/CustomMenu.h"

class ScaleMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(ScaleMenu)

public:
    explicit ScaleMenu(DWidget *parent = nullptr);

signals:
    void sigCurrentScale(const int &);

    // CustomMenu interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomMenu interface
protected:
    void initActions() override;

private slots:
    void slotActionTrigger(QAction *);
    void sloPrevScale();
    void sloNextScale();
    void slotDocProxyMsg(const QString &);

private:
    void __ChangeScale(const int &);
    void InitConnection();
    void OnFileOpenOk(const QString &sPath);
    void onShortKey(const QString &);

private:
    QList<int> dataList;
    int     m_nCurrentIndex = 5;
    QStringList     shortKeyList;                 // 要处理的消息类型


};

#endif // SCALEMENU_H
