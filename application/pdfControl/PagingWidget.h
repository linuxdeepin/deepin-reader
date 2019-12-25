#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <DSpinBox>
#include <DIconButton>

#include <QIntValidator>
#include <QKeyEvent>
#include <QVBoxLayout>

#include "CustomControl/CustomWidget.h"
#include "CustomControl/CustomClickLabel.h"

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
    Q_DISABLE_COPY(PagingWidget)

public:
    explicit PagingWidget(CustomWidget *parent = nullptr);

signals:
    void sigDealWithData(const int &, const QString &);

public:
    void setTotalPages(int pages);
    void setCurrentPageValue(const int &);

private slots:
    void slotDealWithData(const int &, const QString &);
    void slotPrePage();
    void slotNextPage();
    void slotUpdateTheme();

protected:
    void initWidget() Q_DECL_OVERRIDE;
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    void initConnections();

    void onJumpToSpecifiedPage(const int &);

private:
    CustomClickLabel *m_pTotalPagesLab = nullptr;        // 当前文档总页数标签
    DIconButton *m_pPrePageBtn = nullptr;      // 按钮 前一页
    DIconButton *m_pNextPageBtn = nullptr;     // 按钮 后一页
    DSpinBox *m_pJumpPageSpinBox = nullptr;    // 输入框 跳转页码
    int m_totalPage = 0;                       // 当前文档页码总数
    int m_preRow = -1;                         // 当前页码所在list中的行

public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // PAGINGWIDGET_H
