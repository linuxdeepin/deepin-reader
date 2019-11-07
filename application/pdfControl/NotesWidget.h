#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DIconButton>

#include <QVBoxLayout>
#include <QMap>
#include <QThread>
#include <QList>

#include "NotesItemWidget.h"
#include "CustomControl/CustomWidget.h"
#include "docview/docummentproxy.h"
#include "CustomListWidget.h"

class ThreadLoadImageOfNote : public QThread
{
    Q_OBJECT
public:
    ThreadLoadImageOfNote(QObject *parent = nullptr);
    ~ThreadLoadImageOfNote() Q_DECL_OVERRIDE{
        stopThreadRun();
    }

public:
    void stopThreadRun();

    inline void setIsLoaded(const bool &load)
    {
        m_isLoaded = load;
    }

    inline bool isLoaded()
    {
        return m_isLoaded;
    }

    inline void setListNoteSt(const QList<stHighlightContent> &list)
    {
        m_stListNote = list;
    }

signals:
    void sigLoadImage(const QImage &);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    bool m_isLoaded = false;// 是都加载完毕
    QList<stHighlightContent> m_stListNote; // 新文件的注释列表
};

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
    void sigAddNewNoteItem(const QString &);
    void sigDltNoteItem(QString);
    void sigDltNoteContant(QString);
    void sigOpenFileOk();
    void sigCloseFile();
    void sigDelNoteItem();

protected:
    void initWidget() Q_DECL_OVERRIDE;

private slots:
    void slotAddNoteItem(const QString &);
    void slotDltNoteItem(QString);
    void slotDltNoteContant(QString);
    void slotOpenFileOk();
    void slotCloseFile();
    void slotLoadImage(const QImage &);
    void slotDelNoteItem();
    void slotSelectItem(QListWidgetItem *);

private:
    void addNotesItem(const QString &text);
    void initConnection();
    void setSelectItemBackColor(QListWidgetItem *);

private:
    void fillContantToList();
    void addNewItem(const stHighlightContent &note);
    void addNewItem(const QImage &image, const int &page, const QString &uuid, const QString &text);
    void flushNoteItemText(const int &page, const QString &uuid, const QString &text);


private:
    CustomListWidget *m_pNotesList = nullptr;
    QMap<QString, int>      m_mapUuidAndPage;       //  uuid 和 页码 对应
    ThreadLoadImageOfNote m_ThreadLoadImage;      // 加载注释缩略图线程
    QListWidgetItem *m_pNoteItem = nullptr;       // 当前鼠标左键点击的item
    int m_nIndex = -1;                            // 当前注释列表数

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // NOTESFORM_H
