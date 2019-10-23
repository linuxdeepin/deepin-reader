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

signals:
    void sigRemoveHighLighted();    //  告诉高亮显示框, 当前高亮被移除了

public:
    void showWidget(const int &, const int &, const bool &, const QString &, const QString &);

private slots:
    void SlotBtnCopyClicked();
    void SlotBtnRemoveHighLightedClicked();
    void SlotBtnAddAnnotationClicked();
    void SlotBtnAddBookMarkClicked();

    void SlotSetBtnRemoveHighLightedAble();

private:
    void sendMsgAndHide(const int &, const QString &msgContent = "");
    DPushButton *createBtn(const QString &, const char *member);

private:
    int     m_nShowY = -1;
    QString     m_strSelectText = "";       //  选中的文字
    QString     m_strAnnotationUuid = "";   //  高亮的uuid

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;
};

#endif // DEFAULTOPERATIONWIDGET_H
