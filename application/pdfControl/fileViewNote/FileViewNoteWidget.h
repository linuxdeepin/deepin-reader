#ifndef FILEVIEWNOTEWIDGET_H
#define FILEVIEWNOTEWIDGET_H

#include <DLabel>
#include <DTextEdit>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "subjectObserver/CustomWidget.h"

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

private:
    DTextEdit *m_pTextEdit = nullptr;
    QVBoxLayout *m_pVLayout = nullptr;
    QHBoxLayout *m_pHLayoutClose = nullptr;
    QHBoxLayout *m_pHLayoutDlt = nullptr;
    DLabel *m_pCloseLab = nullptr;
    DLabel *m_pDltLab = nullptr;
};

#endif // FILEVIEWNOTEWIDGET_H
