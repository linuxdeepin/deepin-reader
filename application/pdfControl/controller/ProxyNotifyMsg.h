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
#ifndef PROXYNOTIFYMSG_H
#define PROXYNOTIFYMSG_H

#include <QObject>
/**
 * @brief The ProxyNotifyMsg class
 *  文档 书签和当前页变化通知
 */

class FileViewWidgetPrivate;

class ProxyNotifyMsg : public QObject
{
    Q_OBJECT
public:
    explicit ProxyNotifyMsg(QObject *parent = nullptr);

private slots:
    void slotBookMarkStateChange(int, bool);
    void slotDocFilePageChanged(int);

private:
    FileViewWidgetPrivate   *_fvwParent = nullptr;
    friend class FileViewWidgetPrivate;
};

#endif // PROXYNOTIFYMSG_H
