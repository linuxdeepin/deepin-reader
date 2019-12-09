#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include <DWidget>
#include <DFloatingWidget>
#include <DIconButton>
#include "subjectObserver/IObserver.h"
#include "controller/MsgSubject.h"
#include "controller/NotifySubject.h"

DWIDGET_USE_NAMESPACE

class PlayControlWidget : public DFloatingWidget, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(PlayControlWidget)
public:
    explicit PlayControlWidget(DWidget *parnet = nullptr);
    ~PlayControlWidget()Q_DECL_OVERRIDE;

public:
    void activeshow(int ix = 0, int iy = 0);
    void killshow();
    void setCanShow(bool bshow) {m_bcanshow = bshow;}
    bool canShow() {return m_bcanshow;}
protected:
    void initWidget();
    void initConnections();
    DIconButton *createBtn(const QString &strname = QString());
    void changePlayStatus();
    void enterEvent(QEvent *)Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *)Q_DECL_OVERRIDE;

private:
    NotifySubject   *m_pNotifySubject;
    MsgSubject      *m_pMsgSubject;
    QTimer *m_ptimer;
    DIconButton *m_pbtnpre;
    DIconButton *m_pbtnplay;
    DIconButton *m_pbtnnext;
    DIconButton *m_pbtnexit;
    bool m_bcanshow;
    bool m_bautoplaying;
private slots:
    void slotUpdateTheme();
    void slotPreClicked();
    void slotPlayClicked();
    void slotNextClicked();
    void slotExitClicked();

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;
};

#endif // PLAYCONTROLWIDGET_H
