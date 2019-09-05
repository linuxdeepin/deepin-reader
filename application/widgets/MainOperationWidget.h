#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DImageButton>
#include <QHBoxLayout>

#include "header/CustomWidget.h"

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
    void createBtn(const QString &btnName, const char *member);

private slots:
    void on_thumbnailBtn_checkedChanged();
    void on_bookmarksBtn_checkedChanged();
    void on_annotationBtn_checkedChanged();

private:
    QHBoxLayout *m_hboxLayout = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // MAINOPERATIONWIDGET_H
