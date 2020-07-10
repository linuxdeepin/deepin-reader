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

//  高亮&注释消息
enum E_NOTE_MSG {
    MSG_NOTE_ADD = 0,
    MSG_NOTE_DELETE
};

}


#endif  // MSGHEADER_H
