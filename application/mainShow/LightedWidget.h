#ifndef LIGHTEDWIDGET_H
#define LIGHTEDWIDGET_H

#include "subjectObserver/CustomWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DLabel>
#include <DFloatingButton>
#include <QPalette>

/**
 * @brief The LightedWidget class
 * @brief   添加高亮, 颜色选择
 */


class LightedWidget : public CustomWidget
{
    Q_OBJECT
public:
    LightedWidget(CustomWidget *parent = nullptr);

signals:
    void sigAddHighLighted();

private slots:
    void SlotOnBtnGroupClicked(int);
    void SlotRemoveHighLighted();

private:
    int     m_nOldId = -1;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &);

    // CustomWidget interface
protected:
    void initWidget();
};

#endif // LIGHTEDWIDGET_H
