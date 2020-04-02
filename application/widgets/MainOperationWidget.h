#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DPushButton>
#include <DToolButton>
#include <QPointer>

#include "CustomControl/CustomWidget.h"

class DocSheet;
class MainOperationWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MainOperationWidget)

public:
    explicit MainOperationWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~MainOperationWidget() override;

signals:
    void sigShowStackWidget(const int &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void SetFindOperation(const int &);
    void handleOpenSuccess();
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
    void slotUpdateTheme();
    void slotButtonClicked(int);

private:
    QString     m_strBindPath = "";
    QPointer<DocSheet> m_sheet;
};

#endif // MAINOPERATIONWIDGET_H
