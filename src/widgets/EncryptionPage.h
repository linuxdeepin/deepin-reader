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
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ENCRYPTIONPAGE_H
#define ENCRYPTIONPAGE_H

#include <DWidget>
#include <DPushButton>
#include <DLabel>
#include <DPasswordEdit>

DWIDGET_USE_NAMESPACE

/**
 * @brief The EncryptionPage class
 * 加密界面
 */
class EncryptionPage: public DWidget
{
    Q_OBJECT
public:
    explicit EncryptionPage(QWidget *parent = nullptr);
    ~EncryptionPage();

    /**
     * @brief InitUI
     * 初始化控件
     */
    void InitUI();

    /**
     * @brief InitConnection
     * 初始化connect
     */
    void InitConnection();

signals:
    /**
     * @brief sigExtractPassword
     * 密码输入确认信号
     * @param password
     */
    void sigExtractPassword(const QString &password);

public slots:
    /**
     * @brief nextbuttonClicked
     * 密码输入确认
     */
    void nextbuttonClicked();

    /**
     * @brief wrongPassWordSlot
     * 密码错误响应
     */
    void wrongPassWordSlot();

    /**
     * @brief onPasswordChanged
     * 密码输入框字符改变
     */
    void onPasswordChanged();

    /**
     * @brief onSetPasswdFocus
     * 设置密码输入框的焦点,在切换到当前窗口时
     */
    void onSetPasswdFocus();

private slots:
    /**
     * @brief onUpdateTheme
     * 主题更新
     */
    void onUpdateTheme();

private:
    DPushButton *m_nextbutton = nullptr;
    DPushButton *m_echoBtn = nullptr;
    DPasswordEdit *m_password = nullptr;
};

#endif // ENCRYPTIONPAGE_H
