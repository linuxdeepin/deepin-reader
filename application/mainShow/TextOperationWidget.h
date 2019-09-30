#ifndef TEXTOPERATIONWIDGET_H
#define TEXTOPERATIONWIDGET_H

#include "subjectObserver/CustomWidget.h"

#include "LightedWidget.h"

#include <QVBoxLayout>
#include <DPushButton>
#include <QButtonGroup>
/**
 * @brief The DefaultOperationWidget class
 * @brief   右键  文本菜单操作， 复制、高亮显示、移除高亮显示、添加注释、添加书签
 */


class TextOperationWidget : public CustomWidget
{
    Q_OBJECT
public:
    TextOperationWidget(CustomWidget *parent = nullptr);

public:
    void showWidget(const int &, const int &, const bool &, const QString &);

private slots:
    void SlotBtnCopyClicked();
    void SlotBtnRemoveHighLightedClicked();
    void SlotBtnAddAnnotationClicked();
    void SlotBtnAddBookMarkClicked();

private:
    void sendMsgAndHide(const int &, const QString &msgContent = "");
    DPushButton *createBtn(const QString &, const char *member);

private:
    int     m_nShowY = -1;
    QString     m_strSelectText = "";   //  选中的文字

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;
};

#endif // DEFAULTOPERATIONWIDGET_H
