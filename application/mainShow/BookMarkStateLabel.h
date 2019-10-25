#ifndef BOOKMARKSTATELABEL_H
#define BOOKMARKSTATELABEL_H

#include <DLabel>
#include <DWidget>

#include "subjectObserver/IObserver.h"
#include "controller/NotifySubject.h"
#include "controller/MsgSubject.h"

/**
 * @brief The BookMarkStateWidget class
 * @brief   主面板， 当前页的 书签状态
 *
 *
 */

DWIDGET_USE_NAMESPACE

class BookMarkStateLabel : public DLabel, public IObserver
{
    Q_OBJECT
public:
    BookMarkStateLabel(DWidget *parent = nullptr);

signals:
    void sigSetMarkState(const QString &);
    void sigWidgetVisible(const int &);

public:
    bool bChecked() const;
    int nCurPage() const;

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    void setPixmapState(const QString &);
    void initConnections();

private slots:
    void slotSetMarkState(const QString &);
    void slotSetWidgetVisible(const int &);

private:
    bool            m_bChecked = false;
    MsgSubject      *m_pMsgSubject = nullptr;
    NotifySubject   *m_pNotifySubject = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;

private:
    void setObserverName() Q_DECL_OVERRIDE;
};

#endif // BOOKMARKSTATEWIDGT_H
