#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <DWidget>
#include <DLabel>
#include <QVBoxLayout>
#include <DPushButton>
#include <DSpinBox>

DWIDGET_USE_NAMESPACE

const int FIRSTPAGES = 1;
//
class PagingWidget : public DWidget
{
    Q_OBJECT

public:
    PagingWidget(DWidget *parent = nullptr);
    ~PagingWidget();

public:
    void setTotalPages(int pages);

signals:
    void jumpToIndexPage(const int&);

private slots:
    void onPrePage();
    void onNextPage();

public slots:
    void onSetCurrentPage(const int&);

protected:
    //void keyPressEvent(QKeyEvent * event);

private:
    void initWidget();

    void setCurrentPage(const int&);
private:
    DLabel * m_pTotalPagesLab = nullptr;
    DPushButton * m_pPrePageBtn = nullptr;
    DPushButton * m_pNextPageBtn = nullptr;
    DSpinBox * m_pJumpPageSpinBox = nullptr;
    int m_currntPage = 0;
    int m_totalPage = 0;
};

#endif // PAGINGWIDGET_H
