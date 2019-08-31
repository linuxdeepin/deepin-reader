#ifndef MSGSUBJECT_H
#define MSGSUBJECT_H

#include "ISubject.h"
#include <QList>




//  消息类型
enum MSG_TYPE{
    MSG_UPDATE_THEME = 0,           //  更新主题

    MSG_SLIDER_SHOW_STATE = 10,     //  侧边栏 显隐消息
    MSG_MAGNIFYING,                 //  放大镜消息
    MSG_HANDLESHAPE,                //  手型 消息
};







/**
 * @brief The MsgSubject class
 * @brief   消息服务的 发布者
 */

class MsgSubject : public ISubject
{
private:
    MsgSubject();

public:
    static MsgSubject* getInstance()
    {
        static MsgSubject subject;
        return &subject;
    }

    // ISubject interface
public:
    void addObserver(IObserver *obs) override;
    void removeObserver(IObserver *obs) override;
    void Notify(const int&, const QString &) override;

public:
    void sendMsg(const int&, const QString&);

private:
    QList<IObserver*> m_observerList;
};

#endif // MSGSUBJECT_H
