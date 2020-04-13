/*
Central(NaviPage ViewPage)

CentralNavPage(openfile)

CentralDocPage(DocTabbar DocSheets)

DocSheet(SheetSidebar SheetBrowserArea document)
 */

#ifndef FRAME_MAINWIDGET_H
#define FRAME_MAINWIDGET_H

#include "CustomControl/CustomWidget.h"

class CentralDocPage;
class CentralNavPage;
class TitleMenu;
class TitleWidget;
class DocSheet;
class QStackedLayout;
class Central : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Central)

public:
    explicit Central(DWidget *parent = nullptr);

    ~Central() override;

    TitleMenu *titleMenu();

    TitleWidget *titleWidget();

    void openFile(QString filePath);

    void openFilesExec();

    void addSheet(DocSheet *sheet);

    bool hasSheet(DocSheet *sheet);

    bool saveAll();

    void handleShortcut(QString shortcut);

public slots:
    void onSheetCountChanged(int count);

signals:
    void sigOpenFiles(const QString &);

    void sigNeedClose();

    void sigNeedShowState(int);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

protected:
    void initWidget() override;

public slots:
    void onFilesOpened();

    void onCurSheetChanged(DocSheet *);

    void onMenuTriggered(const QString &action);

    void onOpenFilesExec();

private slots:
    void OnSetCurrentIndex();

    void onShowTip(const QString &);

private:
    QStackedLayout      *m_layout = nullptr;
    CentralDocPage      *m_docPage = nullptr;
    CentralNavPage      *m_navPage = nullptr;
    TitleMenu           *m_menu = nullptr;
    TitleWidget         *m_widget = nullptr;
};

#endif  // MAINSTACKWIDGET_H
