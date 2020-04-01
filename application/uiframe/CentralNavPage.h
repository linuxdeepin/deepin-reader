#ifndef CentralNavPage_H
#define CentralNavPage_H

#include "CustomControl/CustomWidget.h"

class CentralNavPage : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CentralNavPage)

public:
    explicit CentralNavPage(DWidget *parent = nullptr);

    ~CentralNavPage() override;

signals:
    void sigOpenFilePaths(const QString &);

    void filesOpened();

public:
    int dealWithData(const int &, const QString &) override;

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
