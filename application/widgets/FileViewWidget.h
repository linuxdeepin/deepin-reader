#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <DWidget>
#include <QMouseEvent>
#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

#include "view/MagnifyingWidget.h"

DWIDGET_USE_NAMESPACE


/**
 * @brief The FileViewWidget class
 * @brief   文档显示区域
 */


class FileViewWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT
public:
    FileViewWidget(DWidget *parent = nullptr);
    ~FileViewWidget() override;

protected:
    void	mouseMoveEvent(QMouseEvent *event) override;
    void	leaveEvent(QEvent *event) override;

private slots:
    void setMagnifyingState(const bool&);

private:
    ThemeSubject    *m_pThemeSubject = nullptr;

    MagnifyingWidget    *m_pMagnifyingWidget = nullptr;
    bool                m_bCanVisible = false;

    // IObserver interface
public:
    int update(const QString &) override;
};

#endif // FILEVIEWWIDGET_H
