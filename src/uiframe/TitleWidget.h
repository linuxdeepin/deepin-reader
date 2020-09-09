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
#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include "DocSheet.h"
#include "widgets/CustomWidget.h"

#include <DPushButton>
#include <DIconButton>

#include <QPointer>
#include <QKeyEvent>

class ScaleWidget;
class DocSheet;

/**
 * @brief The TitleWidget class
 * 窗体自定义标题栏
 */
class TitleWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TitleWidget)

public:
    explicit TitleWidget(DWidget *parent = nullptr);

    ~TitleWidget() override;

public:
    /**
     * @brief setControlEnabled
     * 设置控件知否可用单元
     * @param enable
     */
    void setControlEnabled(const bool &enable);

public slots:
    /**
     * @brief onCurSheetChanged
     * 文档窗口切换单元
     */
    void onCurSheetChanged(DocSheet *);

private slots:
    /**
     * @brief onFindOperation
     * 搜索列表控制单元
     */
    void onFindOperation(const int &);

    /**
     * @brief onThumbnailBtnClicked
     * 缩略图列表显隐按钮
     * @param checked
     */
    void onThumbnailBtnClicked(bool checked);

private:
    /**
     * @brief initBtns
     * 初始化按钮
     */
    void initBtns();

    /**
     * @brief setBtnDisable
     * 设置按钮是否可用
     * @param bAble
     */
    void setBtnDisable(const bool &bAble);

    /**
     * @brief createBtn
     * 创建按钮
     * @param btnName 按钮名字
     * @param bCheckable 是否checked
     * @return 返回当前创建的按钮
     */
    DPushButton *createBtn(const QString &btnName, bool bCheckable = false);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QStringList shortKeyList;

    ScaleWidget *m_pSw = nullptr;
    DPushButton *m_pThumbnailBtn = nullptr;
    QPointer<DocSheet> m_curSheet = nullptr;
};

#endif  // TITLEWIDGET_H
