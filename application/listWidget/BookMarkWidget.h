#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <DWidget>
#include <DListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>

#include "BookMarkItemWidget.h"
#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"
#include "header/MsgHeader.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The BookMarkWidget class
 * @brief   书签  列表数据
 */


class BookMarkWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT

public:
    BookMarkWidget(DWidget *parent = nullptr);
    ~BookMarkWidget() override;

private slots:
    void slotShowSelectItem(QListWidgetItem *);

private:
    void initWidget();
    void dltItem();

public:
    // IObserver interface
    int update(const int&, const QString &) override;

private:
    MsgSubject    *m_pMsgSubject = nullptr;
    DListWidget * m_pBookMarkListWidget = nullptr;
    QVBoxLayout * m_pVBoxLayout = nullptr;
    int m_iCurrentIndex = 0;
};

#endif // BOOKMARKFORM_H
