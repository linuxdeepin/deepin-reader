#ifndef FILEVIEWNOTEWIDGET_H
#define FILEVIEWNOTEWIDGET_H

#include <DLabel>
#include <DTextEdit>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "subjectObserver/CustomWidget.h"
#include "../font/fontWidget.h"

class MenuLab;

class FileViewNoteWidget : public CustomWidget
{
    Q_OBJECT
public:
    FileViewNoteWidget(CustomWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

protected:
    void initWidget() override;

private slots:
    void slotClosed();
    void slotDlted();

private:
    DTextEdit *m_pTextEdit = nullptr;
    MenuLab *m_pCloseLab = nullptr;
    MenuLab *m_pDltLab = nullptr;
};

#endif // FILEVIEWNOTEWIDGET_H
