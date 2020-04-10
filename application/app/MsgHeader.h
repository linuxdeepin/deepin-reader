#ifndef MSGHEADER_H
#define MSGHEADER_H

#include <QString>

/**
 *  消息类型 定义
 */

namespace {

//  搜索框消息
enum E_FIND_WIDGET_MSG {
    E_FIND_PREV,
    E_FIND_NEXT,
    E_FIND_EXIT,
    E_FIND_CONTENT,
};

//  窗口自适应状态
enum E_ADAPTE_MSG {
    NO_ADAPTE_State,
    ADAPTE_WIDGET_State,
    ADAPTE_HEIGHT_State
};

//  高亮&注释消息
enum E_NOTE_MSG {
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
    MSG_NOTE_PAGE_ADD_CONTENT,          //  页面注释 添加内容
    MSG_NOTE_PAGE_ADD_ITEM,             //  页面注释 添加节点
    MSG_NOTE_PAGE_UPDATE_CONTENT,       //  页面注释 更新内容
    MSG_NOTE_PAGE_UPDATE_ITEM,          //  页面注释 更新节点
    MSG_NOTE_PAGE_DELETE_CONTENT,       //  页面注释 删除内容
    MSG_NOTE_PAGE_DELETE_ITEM,          //  页面注释 删除节点
};

}


#endif  // MSGHEADER_H
