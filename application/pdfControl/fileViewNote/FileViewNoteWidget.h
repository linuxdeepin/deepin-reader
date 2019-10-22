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

protected:
    void initWidget() Q_DECL_OVERRIDE;

private slots:
    void slotClosed();
//    void slotDlted();

private:
    DTextEdit *m_pTextEdit = nullptr;   // 注释
    MenuLab *m_pCloseLab = nullptr;     // 关闭
//    MenuLab *m_pDltLab = nullptr;       // 删除
    QString m_strNote;                  // 注释内容
};

#endif // FILEVIEWNOTEWIDGET_H
