#ifndef MSGSUBJECT_H
#define MSGSUBJECT_H

#include "ISubject.h"
#include <QList>




//  消息类型
enum MSG_TYPE {
    MSG_UPDATE_THEME = 0,           //  更新主题

    MSG_SLIDER_SHOW_STATE = 10,     //  侧边栏 显隐消息
    MSG_MAGNIFYING,                 //  放大镜消息
    MSG_HANDLESHAPE,                //  手型 消息

    MSG_OPERATION_OPEN_FILE = 50,  //  打开文件
    MSG_OPERATION_SAVE_FILE,        //  保存文件
    MSG_OPERATION_SAVE_AS_FILE,     //  另存为文件
    MSG_OPERATION_OPEN_FOLDER,      //  打开文件所处文件夹
    MSG_OPERATION_PRINT,            //  打印
    MSG_OPERATION_ATTR,             //  属性
    MSG_OPERATION_FIND,             //  搜索
    MSG_OPERATION_FULLSCREEN,       //  全屏
    MSG_OPERATION_SCREENING,        //  放映
    MSG_OPERATION_LARGER,           //  放大
    MSG_OPERATION_SMALLER,          //  缩小
    MSG_OPERATION_SWITCH_THEME,     //  主题切换

    MSG_THUMBNAIL_JUMPTOPAGE = 100, //  跳转到指定页 消息
    MSG_BOOKMARK_DLTITEM,           //  删除指定书签 消息
    MSG_BOOKMARK_ADDITEM,           //  添加指定书签 消息
    MSG_NOTE_COPYCHOICECONTANT,     //  拷贝指定注释内容 消息
    MSG_NOTE_ADDCONTANT,            //  添加注释内容 消息
    MSG_NOTE_ADDITEM,               //  添加注释子节点 消息
    MSG_NOTE_DLTNOTEITEM,           //  删除注释子节点 消息
    MSG_SWITCHLEFTWIDGET,           //  切换左侧窗口(缩略图、书签、注释) 消息
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
    static MsgSubject *getInstance()
    {
        static MsgSubject subject;
        return &subject;
    }

    // ISubject interface
public:
    void addObserver(IObserver *obs) override;
    void removeObserver(IObserver *obs) override;
    void Notify(const int &, const QString &) override;

public:
    void sendMsg(const int &, const QString &msgContent = "");

private:
    QList<IObserver *> m_observerList;
};

#endif // MSGSUBJECT_H
