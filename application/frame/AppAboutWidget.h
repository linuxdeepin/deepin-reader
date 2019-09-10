#ifndef APPABOUTWIDGET_H
#define APPABOUTWIDGET_H

/**
 * @brief The AboutWidget class
 * @brief   关于
 */

#include <DAboutDialog>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class AppAboutWidget : public DAboutDialog
{
    Q_OBJECT
public:
    AppAboutWidget(DWidget *parent = nullptr);

public:
    void showScreenCenter();

private:
    void initLabels();
};

#endif // ABOUTWIDGET_H
