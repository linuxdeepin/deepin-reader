#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <DLabel>
#include <DSpinBox>
#include <DIconButton>

#include <QIntValidator>
#include <QKeyEvent>
#include <QVBoxLayout>

#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   显示的数字 比 实际的文档 多 1
 * @brief   跳转页窗体
 */

const int FIRSTPAGES = 1;
//
class PagingWidget : public CustomWidget
{
    Q_OBJECT

public:
    PagingWidget(CustomWidget *parent = nullptr);

signals:
    void sigJumpToSpecifiedPage(const int &);

public:
    void setTotalPages(int pages);
    void setCurrentPageValue(const int &);

private slots:
    void slotPrePage();
    void slotNextPage();
    void slotJumpToSpecifiedPage(const int &);

protected:
    void initWidget() override;
    bool eventFilter(QObject *watched, QEvent *event) override;


private:
    DLabel *m_pTotalPagesLab = nullptr;        // 当前文档总页数标签
    DIconButton *m_pPrePageBtn = nullptr;      // 按钮 前一页
    DIconButton *m_pNextPageBtn = nullptr;     // 按钮 后一页
    DSpinBox *m_pJumpPageSpinBox = nullptr;    // 输入框 跳转页码
    int m_currntPage = 0;                      // 当前页码数减一
    int m_totalPage = 0;                       // 当前文档页码总数
    int m_preRow = -1;                         // 当前页码所在list中的行

public:
    int dealWithData(const int &, const QString &) override;
};

#endif // PAGINGWIDGET_H
