#ifndef MAINSHOWSPLITTER_H
#define MAINSHOWSPLITTER_H

#include "FileViewWidget.h"
#include "LeftSidebarWidget.h"
#include "controller/MsgSubject.h"
#include <DSplitter>
#include <QResizeEvent>

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainShowDataWidget class
 * @brief   主要用于显示文档 和 操作界面, 分割器
 */

class MainShowSplitter : public DSplitter, public IObserver
{
    Q_OBJECT
public:
    MainShowSplitter(DWidget *parent = nullptr);
    ~MainShowSplitter() override;

private:
    void initWidget();

private:
    void setSidebarVisible() const;

private:
    LeftSidebarWidget *m_pLeftShowWidget = nullptr; //  侧边栏
    FileViewWidget *m_pFileViewWidget = nullptr; //  显示文档区域

    MsgSubject *m_pMsgSubject = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void sendMsg(const int &msgType, const QString &msgContent) override;

    // IObserver interface
private:
    void setObserverName(const QString &name) override;
};

#endif // MAINSHOWDATAWIDGET_H
