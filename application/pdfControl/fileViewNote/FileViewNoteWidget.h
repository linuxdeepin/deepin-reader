#ifndef FILEVIEWNOTEWIDGET_H
#define FILEVIEWNOTEWIDGET_H

//#include <DWidget>
#include "subjectObserver/CustomWidget.h"

#include <QVBoxLayout>
#include <QTextEdit>

class FileViewNoteWidget : public CustomWidget
{
public:
    FileViewNoteWidget(CustomWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

protected:
    void initWidget() override;

private:
    QTextEdit *m_pTextEdit = nullptr;
    QVBoxLayout *m_pVLayout = nullptr;
};

#endif // FILEVIEWNOTEWIDGET_H
