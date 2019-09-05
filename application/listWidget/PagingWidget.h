#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <DLabel>
#include <QVBoxLayout>
#include <DImageButton>
#include <DSpinBox>
#include <QKeyEvent>

#include "header/CustomWidget.h"

const int FIRSTPAGES = 1;
//
class PagingWidget : public CustomWidget
{
    Q_OBJECT

public:
    PagingWidget(CustomWidget *parent = nullptr);

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
    void initWidget() override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setCurrentPage(const int &);

    void createBtn(DImageButton *btn, QWidget *parent, const QString &text, const QString &btnName, const QString &normalPic, const QString &hoverPic,
                   const QString &pressPic, const QString &checkedPic,
                   const char *member, bool checkable = false, bool checked = false);

    inline void setPreRowVal(const int &val)
    {
        m_preRow = val;
    }
    inline int getPreRowVal() const
    {
        return m_preRow;
    }

private:
    DLabel *m_pTotalPagesLab = nullptr;
    DImageButton *m_pPrePageBtn = nullptr;
    DImageButton *m_pNextPageBtn = nullptr;
    DSpinBox *m_pJumpPageSpinBox = nullptr;
    int m_currntPage = 0;
    int m_totalPage = 0;
    int m_preRow = -1;

public:
    int dealWithData(const int &, const QString &) override;
};

#endif // PAGINGWIDGET_H
