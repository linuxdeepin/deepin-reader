#ifndef DEFAULTOPERATIONWIDGET_H
#define DEFAULTOPERATIONWIDGET_H

#include "subjectObserver/CustomWidget.h"

#include <QVBoxLayout>
#include <DPushButton>

/**
 * @brief The DefaultOperationWidget class
 * @brief   右键  默认菜单操作， 搜索、添加书签、第一页、上一页、下一页、最后一页
 */


class DefaultOperationWidget : public CustomWidget
{
    Q_OBJECT
public:
    DefaultOperationWidget(CustomWidget *parent = nullptr);

public:
    void showWidget(const int &, const int &, const bool &bBookState);

private slots:
    void SlotBtnSearchClicked();
    void SlotBtnAddBookMarkClicked();
    void SlotBtnFirstPageClicked();
    void SlotBtnPrevPageClicked();
    void SlotBtnNextPageClicked();
    void SlotBtnEndPageClicked();

private:
    void createBtn(const QString &, const char *member);
    void sendMsgAndHide(const int &);

private:
    QVBoxLayout *layout = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;
};

#endif // DEFAULTOPERATIONWIDGET_H
