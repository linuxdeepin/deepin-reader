#ifndef FILEVIEWNOTEWIDGET_H
#define FILEVIEWNOTEWIDGET_H

#include <DTextEdit>
#include <QHideEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "CustomControl/CustomWidget.h"
#include "../font/fontWidget.h"

class CustemTextEdit : public DTextEdit
{
    Q_OBJECT
public:
    CustemTextEdit(DWidget *parent = nullptr);

protected:
    //void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    void init();
    int calcTextSize(const QString &);
    void showTipsWidget();

private slots:
    void slotTextEditMaxContantNum();

private:
    int m_nMaxContantLen = 1500;    // 允许输入文本最大长度
};

/**
 *@brief The FileViewNoteWidget class
 *@brief 添加注释子界面
 */
class FileViewNoteWidget : public CustomWidget
{
    Q_OBJECT
public:
    FileViewNoteWidget(CustomWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

public:
    void setEditText(const QString &note);
    void closeWidget();
    void showWidget(const int &);
    void showWidget(const QPoint &);

    void setPointAndPage(const QString &);

    void setNoteUuid(const QString &pNoteUuid);
    void setNotePage(const QString &pNotePage);

protected:
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void initWidget() Q_DECL_OVERRIDE;

private:
    void initConnections();

private slots:
    void slotUpdateTheme();
    void slotClosed();
    void slotTextEditMaxContantNum();

private:
    /*CustemTextEdit DTextEdit*/
    QString     m_pHighLightPointAndPage = "";
    QString     m_pNoteUuid = "";
    QString     m_pNotePage = "";

    CustemTextEdit *m_pTextEdit = nullptr;   // 注释
    CustomClickLabel *m_pCloseLab = nullptr;          // 关闭
    QString m_strNote = "";                       // 注释内容
};

#endif // FILEVIEWNOTEWIDGET_H
