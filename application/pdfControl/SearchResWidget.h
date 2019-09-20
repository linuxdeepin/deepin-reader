#ifndef SEARCHRESWIDGET_H
#define SEARCHRESWIDGET_H

#include <DListWidget>

#include <QListWidgetItem>
#include <QVBoxLayout>

#include "translator/Search.h"
#include "NotesItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   搜索结果显示窗体
 */

using namespace SEARCH;

class SearchResWidget : public CustomWidget
{
    Q_OBJECT

public:
    SearchResWidget(CustomWidget *parent = nullptr);

signals:
    void sigFlushSearchWidget(QVariant);
    void sigClearWidget();

private slots:
    void slotFlushSearchList(QVariant);
    void slotClearWidget();

protected:
    void initWidget() override;

private:
    void initConnections();

private:
    void addNotesItem(const QImage &image, const int &page, const QString &text, const int &resultNum);

private:
    DListWidget *m_pNotesList = nullptr;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
};

#endif // NOTESFORM_H
