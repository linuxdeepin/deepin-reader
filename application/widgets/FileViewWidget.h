#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <DWidget>
#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE


/**
 * @brief The FileViewWidget class
 * @brief   文档显示区域
 */


class FileViewWidget : public DWidget, public IThemeObserver
{
public:
    FileViewWidget(DWidget *parent = nullptr);
    ~FileViewWidget();

private:
    ThemeSubject    *m_pThemeSubject = nullptr;
    // IObserver interface
public:
    int update(const QString &);
};

#endif // FILEVIEWWIDGET_H
