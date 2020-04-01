/*
Central(NaviPage ViewPage)

CentralNavPage(openfile)

CentralDocPage(DocTabbar DocSheets)

DocSheet(SheetSidebar SheetBrowserArea document)
 */

#ifndef FRAME_MAINWIDGET_H
#define FRAME_MAINWIDGET_H

#include "CustomControl/CustomWidget.h"

class Central : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Central)

public:
    explicit Central(DWidget *parent = nullptr);

    ~Central() override;

signals:
    void sigOpenFiles(const QString &);

public:
    int dealWithData(const int &, const QString &) override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

protected:
    void initWidget() override;

public slots:
    void SlotOpenFiles(const QString &);

    void onFilesOpened();

private:
    void initConnections();

    void OnSetCurrentIndex();

    void onShowTip(const QString &);

};

#endif  // MAINSTACKWIDGET_H
