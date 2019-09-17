#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <DListWidget>
#include <DImageButton>

#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMap>
#include <QStringList>

#include "BookMarkItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"
#include "application.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签  列表数据
 */


class BookMarkWidget : public CustomWidget
{
    Q_OBJECT

public:
    BookMarkWidget(CustomWidget *parent = nullptr);

signals:
    void sigOpenFileOk();
    void sigDeleteBookItem(const int &);
    void sigAddBookMark();

private slots:
    void slotShowSelectItem(QListWidgetItem *);
    void slotAddBookMark();
    void slotOpenFileOk();
    void slotDocFilePageChanged(int);
    void slotDeleteBookItem(const int &);

protected:
    void initWidget() override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    void initConnection();
    void dltItem();
    void addBookMarkItem(const int &);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

private:
    DListWidget *m_pBookMarkListWidget = nullptr;
    QVBoxLayout *m_pVBoxLayout = nullptr;
//    QListWidgetItem *m_pCurrentItem = nullptr;
    DImageButton *m_pAddBookMarkBtn = nullptr;

    QList<int>      m_pAllPageList;
    int m_nCurrentPage = -1;
};

#endif // BOOKMARKFORM_H
