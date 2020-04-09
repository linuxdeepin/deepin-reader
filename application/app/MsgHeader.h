#ifndef MSGHEADER_H
#define MSGHEADER_H

#include <QString>

/**
 *  消息类型 定义
 */

namespace {

//  消息类型
enum E_MSG_TYPE {
    E_MSG_TYPE_BEGIN = 0,
    E_APP_MSG_TYPE,
    E_MSG_TYPE_END
};

//  搜索框消息
enum E_FIND_WIDGET_MSG {
    E_FIND_WIDGET_BEGIN = E_MSG_TYPE_END + 50,
    E_FIND_PREV,
    E_FIND_NEXT,
    E_FIND_EXIT,
    E_FIND_CONTENT,
    MSG_FIND_NONE_CONTENT,       //  未搜索到内容消息
    E_FIND_WIDGET_END,
};

//  消息类型
enum MSG_TYPE {
    MSG_TYPE_BEGIN = E_FIND_WIDGET_END + 50,
    MSG_NOTIFY_KEY_MSG,                 //  按键通知消息
    MSG_TYPE_END,
};

//  窗口自适应状态
enum E_ADAPTE_MSG {
    NO_ADAPTE_State,
    ADAPTE_WIDGET_State,
    ADAPTE_HEIGHT_State
};

//  文档操作消息
enum E_DOC_TYPE_MSG {
    MSG_SAVE_FILE = ADAPTE_HEIGHT_State + 50,           //  关闭当前文档　消息
    MSG_NOT_SAVE_FILE,                                  //  关闭当前文档　消息
    MSG_NOT_CHANGE_SAVE_FILE,                           //  关闭当前文档　消息
};

//  高亮&注释消息
enum E_NOTE_MSG {
    E_NOTE_MSG_BEGIN = MSG_NOT_CHANGE_SAVE_FILE + 50,
    MSG_NOTE_ADD_CONTENT,               //  添加注释内容 消息
    MSG_NOTE_ADD_HIGHLIGHT_COLOR,       //  添加高亮
    MSG_NOTE_UPDATE_HIGHLIGHT_COLOR,    //  更新 高亮显示
    MSG_NOTE_REMOVE_HIGHLIGHT,          //  移除 高亮显示
    MSG_OPERATION_TEXT_ADD_ANNOTATION,  //  菜单点击, 添加注释
    MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, //  菜单点击, 已经有注释内容, 显示注释窗口
    MSG_NOTE_ADD_ITEM,                  //  添加注释子节点
    MSG_NOTE_UPDATE_CONTENT,            //  更新高亮注释内容
    MSG_NOTE_UPDATE_ITEM,               //  更新高亮注释节点
    MSG_NOTE_DELETE_CONTENT,            //  删除注释内容 消息
    MSG_NOTE_DELETE_ITEM,               //  删除注释子节点
    MSG_NOTE_PAGE_SHOW_NOTEWIDGET,      //  页面注释显示 注释内容界面
    MSG_NOTE_PAGE_ADD_CONTENT,          //  页面注释 添加内容
    MSG_NOTE_PAGE_ADD_ITEM,             //  页面注释 添加节点
    MSG_NOTE_PAGE_UPDATE_CONTENT,       //  页面注释 更新内容
    MSG_NOTE_PAGE_UPDATE_ITEM,          //  页面注释 更新节点
    MSG_NOTE_PAGE_DELETE_CONTENT,       //  页面注释 删除内容
    MSG_NOTE_PAGE_DELETE_ITEM,          //  页面注释 删除节点
    E_NOTE_MSG_END,
};

}


#endif  // MSGHEADER_H
