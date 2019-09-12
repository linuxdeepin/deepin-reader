#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DListWidget>
#include <DImageButton>

#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QString>

#include "NotesItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   注释窗体
 */

enum WIDGET_TYPE {
    NOTE_WIDGET = 0,
    SEARCH_WIDGET,
};

class NotesWidget : public CustomWidget
{
    Q_OBJECT

public:
    NotesWidget(CustomWidget *parent = nullptr, unsigned int type = NOTE_WIDGET);

signals:
    void sigFlushSearchWidget();

private slots:
    void slotFlushSearchList();

protected:
    void initWidget() override;

private:
    void addNotesItem(const QImage &image, const unsigned int &page, const QString &text, const unsigned int resultNum = NOTE_WIDGET);
    void fillContantToList();

private:
    DListWidget *m_pNotesList = nullptr;
    QVBoxLayout *m_pVLayout = nullptr;

    DImageButton *m_pAddNotesBtn = nullptr;
    unsigned int m_widgetType = NOTE_WIDGET; //窗体类型

    QMap<int, stSearchRes> m_resMap;
public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
};

#endif // NOTESFORM_H
