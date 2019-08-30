#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <DWidget>
#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

class BookMarkWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT

public:
    BookMarkWidget(DWidget *parent = nullptr);
    ~BookMarkWidget();

private:
    ThemeSubject    *m_pThemeSubject = nullptr;

    // IObserver interface
public:
    int update(const QString &);
};

#endif // BOOKMARKFORM_H
