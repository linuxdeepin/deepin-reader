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
    DLabel *m_pTotalPagesLab = nullptr;
    DIconButton *m_pPrePageBtn = nullptr;
    DIconButton *m_pNextPageBtn = nullptr;
    DSpinBox *m_pJumpPageSpinBox = nullptr;
    int m_currntPage = 0;
    int m_totalPage = 0;
    int m_preRow = -1;

public:
    int dealWithData(const int &, const QString &) override;
};

#endif // PAGINGWIDGET_H
