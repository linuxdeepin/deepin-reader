#ifndef WINDOW_H
#define WINDOW_H

//#include "ddialog.h"
//#include "dmainwindow.h"
//#include "editwrapper.h"
//#include "findbar.h"
//#include "jumplinebar.h"
//#include "replacebar.h"
//#include "settings.h"

//#include "thememodule/themepanel.h"
#include "tabbar.h"

#include <DMainWindow>
#include <QWidget>
#include <QStackedWidget>
#include <QResizeEvent>
#include <QVBoxLayout>
#include "widgets/TitleWidget.h"
#include "widgets/MainWidget.h"

DWIDGET_USE_NAMESPACE

class Window : public DMainWindow
{
    Q_OBJECT
public:
    Window(DMainWindow *parent = 0);
    ~Window();

private:
    void initUI();
    void initConnections();
    void initTitlebar();

private slots:

private:
    MainWidget  *m_centralWidget = nullptr;
    TitleWidget *m_titleWidget = nullptr;

    QStackedWidget *m_editorWidget;
    QVBoxLayout *m_centralLayout;
    Tabbar *m_tabbar;
    QMenu *m_menu;
};

#endif // WINDOW_H
