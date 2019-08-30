#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <DWidget>
#include <DListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>

#include "BookMarkItemWidget.h"
#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The BookMarkWidget class
 * @brief   书签  列表数据
 */


class BookMarkWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT

public:
    BookMarkWidget(DWidget *parent = nullptr);
    ~BookMarkWidget();

private:
    void initWidget();

public:
    // IObserver interface
    int update(const QString &);

private:
    ThemeSubject    *m_pThemeSubject = nullptr;
    DListWidget * m_pBookMarkListWidget = nullptr;
    QVBoxLayout * m_pVBoxLayout = nullptr;

};

#endif // BOOKMARKFORM_H
