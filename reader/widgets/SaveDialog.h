/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QObject>

/**
 * @brief The SaveDialog class
 * 保存提示窗口
 */
class SaveDialog : public QObject
{
    Q_DISABLE_COPY(SaveDialog)
    Q_OBJECT
public:
    explicit SaveDialog(QObject *parent = nullptr);

    /**
     * @brief showExitDialog
     * 退出保存提示窗口
     * @return
     */
    static int showExitDialog(QString fileName, QWidget *parent = nullptr);

    /**
     * @brief showTipDialog
     * 提示窗口
     * @param content
     * @return
     */
    static int showTipDialog(const QString &content, QWidget *parent = nullptr);
};

#endif // SAVEDIALOG_H
