#ifndef MAGNIFYINGWIDGET_H
#define MAGNIFYINGWIDGET_H

#include <DWidget>
#include <QPaintEvent>

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MagnifyingWidget class
 * @brief   放大镜窗口
 */



class MagnifyingWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT
public:
    MagnifyingWidget(DWidget *parent = nullptr);
    ~MagnifyingWidget() override;

protected:
    void    paintEvent(QPaintEvent *event) override;

private:
    ThemeSubject    *m_pThemeSubject = nullptr;

    // IObserver interface
public:
    int update(const QString &) override;
};

#endif // MAGNIFYINGWIDGET_H
