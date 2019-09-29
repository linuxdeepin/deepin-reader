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
    ~DocummentFileHelper() Q_DECL_OVERRIDE;

signals:
    void sigOpenFile(const QString &);
    void sigFileSlider();

public:
    void onClickPageLink(Page::Link *pLink);

private:
    void onSaveFileCtrlS();
    void onSaveFile();
    void onSaveAsFile();
    void onCopySelectContent();
    void onFileSlider();
    void setAppShowTitle(const QString &);
    void initConnections();

private slots:
    void slotOpenFile(const QString &filePaths);
    void slotFileSlider();

private:
    DocummentProxy          *m_pDocummentProxy = nullptr;   //  文档操作代理类
    MsgSubject              *m_pMsgSubject = nullptr;
    NotifySubject           *m_pNotifySubject = nullptr;

    QString         m_szFilePath = "";
    DocType_EM      m_nCurDocType = DocType_NULL;
    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;

private:
    void setObserverName() Q_DECL_OVERRIDE;
};

#endif // DOCUMMENTFILEHELPER_H
