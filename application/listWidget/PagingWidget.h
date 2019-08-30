#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <DWidget>
#include <DLabel>
#include <QVBoxLayout>
#include <DPushButton>
#include <DSpinBox>

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

const int FIRSTPAGES = 1;
//
class PagingWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT

public:
    PagingWidget(DWidget *parent = nullptr);
    ~PagingWidget();

public:
    void setTotalPages(int pages);
    // IObserver interface
    int update(const QString &);


signals:
    void jumpToIndexPage(const int&);

private slots:
    void onPrePage();
    void onNextPage();

public:
    void setCurrentPageValue(const int&);

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

    ThemeSubject    *m_pThemeSubject = nullptr;
};

#endif // PAGINGWIDGET_H
