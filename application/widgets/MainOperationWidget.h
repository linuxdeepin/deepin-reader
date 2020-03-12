#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DPushButton>
#include <DToolButton>

#include "CustomControl/CustomWidget.h"

/**
 * @brief The MainOperationWidget class
 * @brief   缩略图、书签、注释 操作widget
 */

class MainOperationWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MainOperationWidget)

public:
    explicit MainOperationWidget(DWidget *parent = nullptr);
    ~MainOperationWidget() override;

signals:
    void sigShowStackWidget(const int &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void SetFindOperation(const int &);

    // CustomWidget interface
protected:
    void initWidget() override;

private:
    DToolButton *createBtn(const QString &btnName, const QString &objName);
    QString findBtnName();
    void initConnect();
    DPushButton *__CreateHideBtn();
    void __SetBtnCheckById(const int &);

    void onDocProxyMsg(const QString &);

private slots:
    void slotOpenFileOk(const QString &sPath);
    void slotUpdateTheme();
    void slotButtonClicked(int);

private:
    QString     m_strBindPath = "";

};

#endif // MAINOPERATIONWIDGET_H
