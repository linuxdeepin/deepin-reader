#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DIconButton>
#include <DPushButton>
#include "subjectObserver/CustomWidget.h"

/**
 * @brief The MainOperationWidget class
 * @brief   缩略图、书签、注释 操作widget
 */

class MainOperationWidget : public CustomWidget
{
    Q_OBJECT
public:
    MainOperationWidget(CustomWidget *parent = nullptr);

private:
    DIconButton *createBtn(const QString &btnName);
    QString findBtnName();
    void initConnect();

signals:
    void sigSearchControl();
    void sigSearchClosed();

private slots:
    void slotButtonClicked(int);
    void slotSearchControl();
    void slotSearchClosed();

private:
    DPushButton *m_pHideBtn = nullptr;      //  隐藏按钮

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

private:
    int m_nThumbnailIndex = -1;     // 左侧缩略图在当前哪个位置0：全缩图1：书签2：注释
};

#endif // MAINOPERATIONWIDGET_H
