#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DMenu>
#include <DImageButton>
#include <QHBoxLayout>
#include "subjectObserver/CustomImageButton.h"
#include "subjectObserver/CustomWidget.h"

#include "pdfControl/font/fontWidget.h"

#include "PublicFunction.h"
/**
 * @brief The TitleWidget class
 * @brief   标题栏的 按钮操作
 */

class TitleWidget : public CustomWidget
{
    Q_OBJECT
public:
    TitleWidget(CustomWidget *parent = nullptr);
    ~TitleWidget() override;

private slots:
    void on_thumbnailBtn_clicked(bool);
    void on_fontBtn_clicked(bool);
    void on_handleShapeBtn_clicked(bool);
    void on_magnifyingBtn_clicked(bool);

    void on_HandleAction_trigger(bool);
    void on_DefaultAction_trigger(bool);

private:
    void createBtn(const QString &btnName, const int &, const int &, const char *member);
    void createAction(const QString &iconName, const char *member);
    void sendMsgToSubject(const int &, const QString &msgCotent = "");

private:
    QHBoxLayout *m_layout = nullptr;

    int     m_nCurrentState = 0;
    DMenu   *m_pHandleMenu = nullptr;

    // CustomWidget interface

    FontWidget *m_pFontWidget = nullptr;
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // TITLEWIDGET_H
