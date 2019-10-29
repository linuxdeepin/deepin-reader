#ifndef FILEVIEWNOTEWIDGET_H
#define FILEVIEWNOTEWIDGET_H

#include <DLabel>
#include <DTextEdit>
#include <DMessageBox>
#include <QHideEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include "subjectObserver/CustomWidget.h"
#include "../font/fontWidget.h"

class CustemTextEdit : public DTextEdit
{
    Q_OBJECT
public:
    CustemTextEdit(DWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    void init();

private slots:
    void slotTextEditMaxContantNum();

private:
    int m_nMaxContantLen = 239;    // 允许输入文本最大长度
};

/**
 *@brief The FileViewNoteWidget class
 *@brief 添加注释子界面
 */

class MenuLab;

class FileViewNoteWidget : public CustomWidget
{
    Q_OBJECT
public:
    FileViewNoteWidget(CustomWidget *parent = nullptr);

signals:
    void sigUpdateTheme(const QString &);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

public:
    void setEditText(const QString &note);
    void closeWidget();
    void showWidget(const int &);

protected:
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void initWidget() Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    void initConnections();

private slots:
    void slotUpdateTheme(const QString &);
    void slotClosed();
    void slotTextEditMaxContantNum();

private:
    /*CustemTextEdit DTextEdit*/
    CustemTextEdit *m_pTextEdit = nullptr;   // 注释
    MenuLab *m_pCloseLab = nullptr;          // 关闭
    QString m_strNote = "";                       // 注释内容
};

#endif // FILEVIEWNOTEWIDGET_H
