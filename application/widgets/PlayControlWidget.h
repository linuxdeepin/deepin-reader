#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include <DWidget>
#include <DFloatingWidget>
#include <DIconButton>
#include "subjectObserver/IObserver.h"

#include "controller/NotifySubject.h"

DWIDGET_USE_NAMESPACE

class PlayControlWidget : public DFloatingWidget, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(PlayControlWidget)
public:
    explicit PlayControlWidget(DWidget *parnet = nullptr);
    ~PlayControlWidget()Q_DECL_OVERRIDE;

    //  主题更新信号
signals:
    void sigUpdateTheme();
    void sigDealWithData(const int &, const QString &);

public:
    void activeshow(int ix = 0, int iy = 0);
    void killshow();
    void setCanShow(bool bshow) {m_bcanshow = bshow;}
    bool canShow() {return m_bcanshow;}

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;

protected:
    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;

private slots:
    void slotUpdateTheme();
    void slotPreClicked();
    void slotPlayClicked();
    void slotNextClicked();
    void slotExitClicked();
    void SlotDealWithData(const int &, const QString &);

private:
    void __PageChangeByKey(const QString &);
    void initWidget();
    void initConnections();
    DIconButton *createBtn(const QString &strname = QString());
    void pagejump(const bool &bpre);
    void changePlayStatus();

private:
    SubjectThread   *m_pNotifySubject = nullptr;

    QTimer *m_ptimer = nullptr;
    DIconButton *m_pbtnpre = nullptr;
    DIconButton *m_pbtnplay = nullptr;
    DIconButton *m_pbtnnext = nullptr;
    DIconButton *m_pbtnexit = nullptr;
    bool m_bcanshow = false;
    bool m_bautoplaying = false;
    bool m_bfirstshow = false;

    QList<int>          m_pMsgList;
};

#endif // PLAYCONTROLWIDGET_H
