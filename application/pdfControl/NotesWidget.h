#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DIconButton>

#include <QVBoxLayout>
#include <QMap>

#include "NotesItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"
#include "CustomListWidget.h"
#include "translator/PdfControl.h"

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
    void sigDltNoteContant(QString);

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

private slots:
    void slotAddNoteItem(QString);
    void slotDltNoteItem(QString);
    void slotDltNoteContant(QString);

private:
    void addNotesItem(const QImage &image, const int &page, const QString &text);
    void initConnection();

private:
    CustomListWidget *m_pNotesList = nullptr;
    void fillContantToList();
    bool hasNoteInList(const int &, const QString&);
    void addNewItem(const QImage &image, const int &page, const QString &uuid, const QString &text);
    void flushNoteItemText(const int &page, const QString &uuid, const QString &text);

private:
    QVBoxLayout *m_pVLayout = nullptr;            // 垂直布局
//    QString m_strUUid;                          // 注释的uuid， 便于区分注释
    //  page       uuid     text
    QMap<int, QMap<QString, QString>> m_mapNotes; // 当前注释列表内容

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // NOTESFORM_H
