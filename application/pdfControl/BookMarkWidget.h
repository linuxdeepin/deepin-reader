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


private slots:
    void slotShowSelectItem(QListWidgetItem *);
    void slotAddBookMark();
    void slotOpenFileOk();

protected:
    void initWidget() override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    void initConnection();
    void dltItem();
    void addBookMarkItem(const QImage &, const int &);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

private:
    DListWidget *m_pBookMarkListWidget = nullptr;
    QVBoxLayout *m_pVBoxLayout = nullptr;
    QListWidgetItem *m_pCurrentItem = nullptr;
    DImageButton *m_pAddBookMarkBtn = nullptr;

    QList<int>      m_pAllPageList;
};

#endif // BOOKMARKFORM_H
