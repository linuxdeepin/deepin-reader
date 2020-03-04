#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include "CustomControl/CustomWidget.h"

/**
 *  @brief  支持拖拽
 *  @brief  打开pdf 文件
 */

class HomeWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(HomeWidget)

public:
    explicit HomeWidget(CustomWidget *parent = nullptr);
    ~HomeWidget() override;

signals:
    void sigOpenFilePaths(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;

private slots:
    void slotChooseBtnClicked();
    void slotUpdateTheme();

private:
    QStringList getOpenFileList();
    void initConnections();
    void NewWindow();
};

#endif // OPENFILEWIDGET_H
