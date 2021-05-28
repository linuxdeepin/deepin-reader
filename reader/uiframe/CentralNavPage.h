/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#ifndef CentralNavPage_H
#define CentralNavPage_H

#include "BaseWidget.h"

/**
 * @brief The CentralNavPage class
 * 嵌入当前窗体中心控件的无文档默认页面
 */
class CentralNavPage : public BaseWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CentralNavPage)

public:
    explicit CentralNavPage(DWidget *parent = nullptr);

signals:
    /**
     * @brief sigNeedOpenFilesExec
     * 请求阻塞式选择并打开文档
     */
    void sigNeedOpenFilesExec();

private slots:
    /**
     * @brief onThemeChanged
     * 主题变化时处理
     */
    void onThemeChanged();

};

#endif // OPENFILEWIDGET_H
