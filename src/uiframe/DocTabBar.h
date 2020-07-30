#ifndef MAINTABBAR_H
#define MAINTABBAR_H

#include <DTabBar>
#include <DTabWidget>

DWIDGET_USE_NAMESPACE

class CentralDocPage;
class DocSheet;
class DocTabBar : public DTabBar
{
    Q_OBJECT
    Q_DISABLE_COPY(DocTabBar)

public:
    explicit DocTabBar(QWidget *parent);

    int indexOfFilePath(const QString &filePath);

    void insertSheet(DocSheet *sheet, int index = -1);

    void removeSheet(DocSheet *sheet);

    void showSheet(DocSheet *sheet);

    void updateTabWidth();

signals:
    void sigTabMoveIn(DocSheet *);

    void sigTabClosed(DocSheet *);

    void sigTabMoveOut(DocSheet *);

    void sigTabNewWindow(DocSheet *);

    void sigNeedOpenFilesExec();

protected:
    QMimeData *createMimeDataFromTab(int index, const QStyleOptionTab &option) const override;

    void insertFromMimeDataOnDragEnter(int index, const QMimeData *source) override; //只是先生成一个tab,结束后自动删除

    void insertFromMimeData(int index, const QMimeData *source) override;            //完全DROP 需要添加tab并打开对应的文档

    bool canInsertFromMimeData(int index, const QMimeData *source) const override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void resizeEvent(QResizeEvent *e) override;

private slots:
    void onDragActionChanged(Qt::DropAction action);

    void onTabReleased(int index);                                  //方法测试结果为当tab从bar移除释放

    void onTabDroped(int index, Qt::DropAction da, QObject *target);//方法测试结果为当tab添加到其他的bar里释放

    void onSetCurrentIndex();

signals:
    void sigAddTab(const QString &);

    void sigCloseTab(const QString &);

    void sigFileCountChanged(int);

    void sigLastTabMoved();     //最后一个tab移入其他程序

    void sigTabChanged(DocSheet *);

    void sigNeedActivateWindow();

private:
    QString getFileName(const QString &strFilePath);

private slots:
    void onTabChanged(int);

    void onTabAddRequested();

    void onTabCloseRequested(int index);

private:
    int m_delayIndex = -1;
};

#endif // MAINTABWIDGET_H
