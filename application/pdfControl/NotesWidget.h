#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <QVBoxLayout>

#include "NotesItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"
#include "CustomListWidget.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   注释窗体
 */

class NotesWidget : public CustomWidget
{
    Q_OBJECT

public:
    NotesWidget(CustomWidget *parent = nullptr);

signals:
    void sigAddNewNoteItem();

protected:
    void initWidget() override;

private slots:
    void slotAddNoteItem();

private:
    void addNotesItem(const QImage &image, const int &page, const QString &text);

private:
    CustomListWidget *m_pNotesList = nullptr;
    void fillContantToList();

private:
    QVBoxLayout *m_pVLayout = nullptr;

    int m_nUUid = 1;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
};

#endif // NOTESFORM_H
