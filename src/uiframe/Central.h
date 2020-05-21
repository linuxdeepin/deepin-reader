/*
Central(NaviPage ViewPage)

CentralNavPage(openfile)

CentralDocPage(DocTabbar DocSheets)

DocSheet(SheetSidebar SheetBrowserArea document)
 */
#ifndef CENTRAL_H
#define CENTRAL_H

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
    explicit Central(QWidget *parent = nullptr);

    ~Central();

    TitleMenu *titleMenu();

    TitleWidget *titleWidget();

    void openFilesExec();

    void openFiles(QStringList filePathList);

    void addSheet(DocSheet *sheet);

    bool hasSheet(DocSheet *sheet);

    void showSheet(DocSheet *sheet);

    bool saveAll();

    void handleShortcut(QString shortcut);

    void doOpenFile(QString filePath);

signals:
    void sigOpenFiles(const QString &);

    void sigNeedClose();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

public slots:
    void onSheetCountChanged(int count);

    void onMenuTriggered(const QString &action);

    void onOpenFilesExec();

    void onNeedActivateWindow();

    void onShowTips(const QString &text, int iconIndex = 0);

private:
    QStackedLayout      *m_layout = nullptr;
    CentralDocPage      *m_docPage = nullptr;
    CentralNavPage      *m_navPage = nullptr;
    TitleMenu           *m_menu = nullptr;
    TitleWidget         *m_widget = nullptr;
};

#endif  // CENTRAL_H
