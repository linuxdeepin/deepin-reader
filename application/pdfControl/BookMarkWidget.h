#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <DListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>

#include "BookMarkItemWidget.h"
#include "subjectObserver/CustomWidget.h"

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
    void dltItem();

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

private:
    DListWidget *m_pBookMarkListWidget = nullptr;
    QVBoxLayout *m_pVBoxLayout = nullptr;
    int m_iCurrentIndex = 0;
};

#endif // BOOKMARKFORM_H
