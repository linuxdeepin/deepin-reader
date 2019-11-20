#ifndef NOTETIPWIDGET_H
#define NOTETIPWIDGET_H

#include "CustomControl/CustomWidget.h"

/**
 * @brief The NoteTipWidget class
 *  显示注释内容
 */


class NoteTipWidget : public CustomWidget
{
    Q_OBJECT
public:
    NoteTipWidget(CustomWidget *parnet = nullptr);

public:
    void setTipContent(const QString &);

private:
    void initConnection();

private slots:
    void slotUpdateTheme();

    // IObserver interface
public:
    int dealWithData(const int &, const QString &);

    // CustomWidget interface
protected:
    void initWidget();
};

#endif // NOTETIPWIDGET_H
