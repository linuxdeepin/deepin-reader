#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <DLabel>
#include <DImageButton>
#include <DSpinBox>

#include <QKeyEvent>
#include <QVBoxLayout>

#include "translator/Frame.h"
#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   跳转页窗体
 */

using namespace  COMMON;

const int FIRSTPAGES = 1;
//
class PagingWidget : public CustomWidget
{
    Q_OBJECT

public:
    PagingWidget(CustomWidget *parent = nullptr);

public:
    void setTotalPages(int pages);
    void setCurrentPageValue(const int &);
    void setPageValue(const int &);

signals:
    void sigJumpToIndexPage(const int &);

private slots:
    void slotPrePage();
    void slotNextPage();

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
