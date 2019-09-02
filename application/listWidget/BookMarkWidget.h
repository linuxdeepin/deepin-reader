#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <DWidget>
#include <DListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>

#include "BookMarkItemWidget.h"
#include "header/CustomWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The BookMarkWidget class
 * @brief   书签  列表数据
 */


class BookMarkWidget : public CustomWidget
{
    Q_OBJECT

public:
    BookMarkWidget(CustomWidget *parent = nullptr);

private slots:
    void slotShowSelectItem(QListWidgetItem *);

protected:
    void initWidget() override;

private:
    //void initWidget();
    void dltItem();

public:
    // IObserver interface
    int update(const int &, const QString &) override;

private:
    DListWidget *m_pBookMarkListWidget = nullptr;
    QVBoxLayout *m_pVBoxLayout = nullptr;
    int m_iCurrentIndex = 0;
};

#endif // BOOKMARKFORM_H
