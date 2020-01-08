#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include "CustomControl/CustomWidget.h"
#include "controller/AppSetting.h"

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
    ~HomeWidget() Q_DECL_OVERRIDE;

signals:
    void sigDealWithKeyMsg(const QString &);

private slots:
    void slotDealWithKeyMsg(const QString &);
    void slotChooseBtnClicked();
    void slotUpdateTheme();

private:
    QStringList getOpenFileList();
    void initConnections();

private:
    AppSetting *m_settings = nullptr;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // OPENFILEWIDGET_H
