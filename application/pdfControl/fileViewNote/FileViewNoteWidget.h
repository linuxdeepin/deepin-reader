#ifndef FILEVIEWNOTEWIDGET_H
#define FILEVIEWNOTEWIDGET_H

#include <DLabel>
#include <DTextEdit>
#include <DMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include "subjectObserver/CustomWidget.h"
#include "../font/fontWidget.h"

/**
 * @brief The CustomTextEdit class
 * 自绘TextEdit
 */
class CustomTextEdit : public DTextEdit
{
    Q_OBJECT
public:
    CustomTextEdit(DWidget * parent = nullptr);

private:
    void initWidget();
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

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

public:
    void setEditText(const QString &note);
    void closeWidget();
    void showWidget(const int &);

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void  paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private slots:
    void slotClosed();

private:
    DTextEdit *m_pTextEdit = nullptr;   // 注释
    MenuLab *m_pCloseLab = nullptr;     // 关闭
    QString m_strNote;                  // 注释内容
};

#endif // FILEVIEWNOTEWIDGET_H
