#ifndef DOCUMMENTFILEHELPER_H
#define DOCUMMENTFILEHELPER_H

#include <QObject>

#include "docview/docummentproxy.h"
#include "subjectObserver/IObserver.h"
#include "controller/MsgSubject.h"
#include "controller/NotifySubject.h"

class DocummentFileHelper : public QObject, public IObserver
{
    Q_OBJECT
public:
    explicit DocummentFileHelper(QObject *parent = nullptr);

signals:
    void sigOpenFile(const QString &);

private:
    void onSaveFileCtrlS();
    void onSaveFile();
    void onSaveAsFile();
    void onCopySelectContent();
    void onFileSlider();

    void initConnections();

private slots:
    void slotOpenFile(const QString &filePaths);

private:
    DocummentProxy          *m_pDocummentProxy = nullptr;   //  文档操作代理类
    MsgSubject              *m_pMsgSubject = nullptr;
    NotifySubject           *m_pNotifySubject = nullptr;

    QString     m_szFilePath = "";

    // IObserver interface
public:
    int dealWithData(const int &, const QString &);
    void sendMsg(const int &, const QString &msgContent = "");

private:
    void setObserverName(const QString &name);
};

#endif // DOCUMMENTFILEHELPER_H
