#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DListWidget>
#include <DImageButton>
#include <QListWidgetItem>
#include <QVBoxLayout>

#include "NotesItemWidget.h"
#include "subjectObserver/CustomWidget.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   注释窗体
 */

class NotesWidget : public CustomWidget
{
    Q_OBJECT

public:
    NotesWidget(CustomWidget *parent = nullptr);

protected:
    void initWidget() override;

private:
    void addNotesItem(const QImage &image, const int &page, const QString &text);
    void fillContantToList();

private:
    DListWidget *m_pNotesList = nullptr;
    QVBoxLayout *m_pVLayout = nullptr;

    DImageButton *m_pAddNotesBtn = nullptr;
public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
};

#endif // NOTESFORM_H
