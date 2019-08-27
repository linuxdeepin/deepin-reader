#ifndef WINDOW_H
#define WINDOW_H

//#include "ddialog.h"
//#include "dmainwindow.h"
//#include "editwrapper.h"
//#include "findbar.h"
//#include "jumplinebar.h"
//#include "replacebar.h"
//#include "settings.h"
//#include "tabbar.h"
//#include "thememodule/themepanel.h"

#include <DMainWindow>
#include <QWidget>
#include <QStackedWidget>
#include <QResizeEvent>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

class Window : public DMainWindow
{
    Q_OBJECT
public:
    Window(DMainWindow *parent = 0);
    ~Window();

    void initTitlebar();

signals:

public slots:
};

#endif // WINDOW_H
