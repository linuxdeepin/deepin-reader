#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <DWidget>
#include <DLabel>
#include <QVBoxLayout>
#include <DPushButton>
#include <DSpinBox>

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

const int FIRSTPAGES = 1;
//
class PagingWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT

public:
    PagingWidget(DWidget *parent = nullptr);
    ~PagingWidget() override;

public:
    void setTotalPages(int pages);

signals:
    void sigJumpToIndexPage(const int &);

private slots:
    void slotPrePage();
    void slotNextPage();

public:
    void setCurrentPageValue(const int &);

protected:
    //void keyPressEvent(QKeyEvent * event);

private:
    void initWidget();
    void setCurrentPage(const int &);

private:
    DLabel *m_pTotalPagesLab = nullptr;
    DPushButton *m_pPrePageBtn = nullptr;
    DPushButton *m_pNextPageBtn = nullptr;
    DSpinBox *m_pJumpPageSpinBox = nullptr;
    int m_currntPage = 0;
    int m_totalPage = 0;

    MsgSubject    *m_pMsgSubject = nullptr;
    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // PAGINGWIDGET_H
