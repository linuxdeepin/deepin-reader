#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DPushButton>
#include <DWidget>
#include <QHBoxLayout>

#include "header/CustomWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainOperationWidget class
 * @brief   缩略图、书签、注释 操作widget
 */

class MainOperationWidget : public CustomWidget
{
    Q_OBJECT
public:
    MainOperationWidget(CustomWidget *parent = nullptr);

private:
    void initBtns();
    void createBtn(const QString &iconName, const char *member, bool checkable = false, bool checked = false);

private slots:
    void on_thumbnailBtn_clicked(bool);
    void on_bookmarksBtn_clicked(bool);
    void on_annotationBtn_clicked(bool);

private:
    QHBoxLayout *m_hboxLayout = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // MAINOPERATIONWIDGET_H
