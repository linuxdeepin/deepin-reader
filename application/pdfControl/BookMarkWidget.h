#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include "CustomListWidget.h"
#include <DPushButton>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMap>
#include <QStringList>
#include <QThread>

#include "BookMarkItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"
#include "application.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签  列表数据
 */

class BookMarkWidget;

class LoadBookMarkThread : public QThread
{
    Q_OBJECT
public:
    LoadBookMarkThread(QObject *parent = nullptr);

signals:
    void signal_loadImage(int, QImage);

public:
    inline void setBookMark(BookMarkWidget *bookMarkW)
    {
        m_pBookMarkWidget = bookMarkW;
    }

    inline void setBookMarks(const int &count)
    {
        m_bookMarks = count;
    }

    void stopThreadRun();

protected:
    void run() override;

private:
    BookMarkWidget *m_pBookMarkWidget = nullptr;
    int m_bookMarks   = 0;   // 书签总数
    int m_nStartIndex = 0;   // 加载图片起始位置
    int m_nEndIndex   = 19;  // 加载图片结束位置
    bool m_isRunning = true; // 运行状态
};

class BookMarkWidget : public CustomWidget
{
    Q_OBJECT

public:
    BookMarkWidget(CustomWidget *parent = nullptr);
    ~BookMarkWidget() override;

signals:
    void sigOpenFileOk();
    void sigDeleteBookItem();
    void sigAddBookMark();
    void sigCloseFile();

private slots:
    void slotAddBookMark();
    void slotOpenFileOk();
    void slotDocFilePageChanged(int);
    void slotDeleteBookItem();
    void slotCloseFile();
    void slot_loadImage(int, QImage);

protected:
    void initWidget() override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    void initConnection();
    void addBookMarkItem(const int &);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
    int getBookMarkPage(const int &index);


private:
    CustomListWidget *m_pBookMarkListWidget = nullptr;
    DPushButton *m_pAddBookMarkBtn = nullptr;
    QList<int>      m_pAllPageList;
    int m_nCurrentPage = -1;
    LoadBookMarkThread m_loadBookMarkThread;
    BookMarkItemWidget *m_pItemWidget = nullptr; // 当前要填充图片的ItemWidget
};

#endif // BOOKMARKFORM_H
