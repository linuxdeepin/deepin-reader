#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DMenu>
#include <DImageButton>
#include <QHBoxLayout>
#include "header/CustomWidget.h"

/**
 * @brief The TitleWidget class
 * @brief   标题栏的 按钮操作
 */

class TitleWidget : public CustomWidget
{
    Q_OBJECT
public:
    TitleWidget(CustomWidget *parent = nullptr);

private slots:
    void on_thumbnailBtn_checkedChanged(bool);
    void on_fontBtn_clicked(bool);
    void on_handleShapeBtn_clicked(bool);
    void on_magnifyingBtn_checkedChanged(bool);

    void on_HandleAction_trigger(bool);
    void on_DefaultAction_trigger(bool);

private:
    void initBtns();
    void createBtn(const QString &btnName, const char *member, bool checkable = false, bool checked = false);

    void createAction(const QString &iconName, const char *member);

    void sendMsgToSubject(const int &, const QString &);

private:
    QHBoxLayout *m_layout = nullptr;

    int     m_nCurrentState = 0;
    DMenu   *m_pHandleMenu = nullptr;
//    QAction *m_pHandleAction = nullptr;
//    QAction *m_pDefaultAction = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // TITLEWIDGET_H
