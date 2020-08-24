/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <DIconButton>
#include <DLineEdit>

#include "widgets/CustomWidget.h"
#include "widgets/CustomClickLabel.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   显示的数字 比 实际的文档 多 1
 * @brief   跳转页窗体
 */

class DocSheet;
class PagingWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(PagingWidget)

public:
    explicit PagingWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~PagingWidget() override;

    void handleOpenSuccess();

    void setIndex(int index);

    QWidget *getLastFocusWidget();

private slots:
    void slotPrePageBtnClicked();
    void slotNextPageBtnClicked();
    void slotUpdateTheme();

    void SlotJumpPageLineEditReturnPressed();
    void onEditFinished();
private:
    void initWidget() override;
    void __SetBtnState(const int &currntPage, const int &totalPage);

    void normalChangePage();
    void pageNumberJump();

private:
    DLabel              *m_pTotalPagesLab = nullptr;        // 当前文档总页数标签
    DLabel              *m_pCurrantPageLab = nullptr;       // 当前文档当前页码
    DIconButton         *m_pPrePageBtn = nullptr;           // 按钮 前一页
    DIconButton         *m_pNextPageBtn = nullptr;          // 按钮 后一页
    DLineEdit           *m_pJumpPageLineEdit = nullptr;     // 输入框 跳转页码
    DocSheet            *m_sheet;
    int                  m_curIndex = 0;
};

#endif // PAGINGWIDGET_H
