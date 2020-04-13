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

public:
    void SetFindOperation(const int &);

    void handleOpenSuccess();

protected:
    void initWidget() override;

private:
    DToolButton *createBtn(const QString &btnName, const QString &objName);

    QString findBtnName();

    void initConnect();

    DPushButton *createHideBtn();

    void setBtnCheckById(const int &);

    void onDocProxyMsg(const QString &);

private slots:
    void slotUpdateTheme();

    void slotButtonClicked(int);

private:
    QPointer<DocSheet> m_sheet;
};

#endif // MAINOPERATIONWIDGET_H
