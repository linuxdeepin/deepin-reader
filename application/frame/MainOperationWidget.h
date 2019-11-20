#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DIconButton>
#include <DPushButton>
#include <DToolButton>
#include "CustomControl/CustomWidget.h"
#include "controller/DataManager.h"

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
    DToolButton *createBtn(const QString &btnName, const QString &objName);
    QString findBtnName();
    void initConnect();

signals:
    void sigSearchControl();
    void sigSearchClosed();

private slots:
    void slotUpdateTheme();
    void slotButtonClicked(int);
    void slotSearchControl();
    void slotSearchClosed();

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

private:
    int m_nThumbnailIndex = WIDGET_THUMBNAIL;     // 左侧缩略图在当前哪个位置0：全缩图1：书签2：注释
};

#endif // MAINOPERATIONWIDGET_H
