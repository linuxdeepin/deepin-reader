#ifndef SUBJECTHEADER_H
#define SUBJECTHEADER_H

#include <QString>

//  消息数据结构体
typedef struct {
    int msgType = -1;
    QString msgContent = "";
} MsgStruct;

#endif // SUBJECTHEADER_H
