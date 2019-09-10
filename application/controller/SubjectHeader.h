#ifndef SUBJECTHEADER_H
#define SUBJECTHEADER_H

#include "subjectObserver/IObserver.h"
#include <QString>

//  消息数据结构体
typedef struct {
    IObserver *obs = nullptr;
    int msgType = -1;
    QString msgContent = "";
} MsgStruct;

#endif // SUBJECTHEADER_H
