#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DWidget>
#include <DPushButton>
#include <QHBoxLayout>

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainOperationWidget class
 * @brief   缩略图、书签、注释 操作widget
 */


class MainOperationWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT
public:
    MainOperationWidget(DWidget* parent = nullptr);
    ~MainOperationWidget();

signals:
    void showType(const int&) const;

private:
    void initBtns();
    void createBtn(const QString& iconName, const char* member, bool checkable = false, bool checked = false);

private slots:
    void on_thumbnailBtn_clicked(bool);
    void on_bookmarksBtn_clicked(bool);
    void on_annotationBtn_clicked(bool);

private:
     QHBoxLayout    *m_hboxLayout = nullptr;
     ThemeSubject   *m_pThemeSubject = nullptr;

     // IObserver interface
public:
     int update(const QString &);
};

#endif // MAINOPERATIONWIDGET_H
