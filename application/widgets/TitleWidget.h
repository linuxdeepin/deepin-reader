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
    void on_fontBtn_clicked();
    void on_handleShapeBtn_clicked();
    void on_magnifyingBtn_checkedChanged(bool);

    void on_HandleAction_trigger();
    void on_DefaultAction_trigger();

private:
    void initBtns();
    void createBtn(const QString &btnName, const QString &normalPic, const QString &hoverPic,
                   const QString &pressPic, const QString &checkedPic,
                   const char *member, bool checkable = false, bool checked = false);

    void createAction(const QString &iconName, const char *member);

    void sendMsgToSubject(const int &, const QString &);

private:
    QHBoxLayout *m_layout = nullptr;

    bool m_bCurrentState = false;
    QAction *m_pHandleAction = nullptr;
    QAction *m_pDefaultAction = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // TITLEWIDGET_H
