#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <QVBoxLayout>
#include <QMap>

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
    void sigAddNewNoteItem(QString);
    void sigDltNoteItem(QString);

protected:
    void initWidget() Q_DECL_OVERRIDE;

private slots:
    void slotAddNoteItem(QString);
    void slotDltNoteItem(QString);

private:
    void addNotesItem(const QImage &image, const int &page, const QString &text);
    void initConnection();

private:
    CustomListWidget *m_pNotesList = nullptr;
    void fillContantToList();

private:
    QVBoxLayout *m_pVLayout = nullptr;         // 垂直布局
    int m_nUUid = 1;                           // 注释的uuid， 便于区分注释
    QMap<QString, QMap<QString, QString>> m_mapNotes; // 当前注释列表内容

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // NOTESFORM_H
