#ifndef MSGHEADER_H
#define MSGHEADER_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

/**
 *  消息类型 定义
 */

//  消息类型
enum MSG_TYPE {
    MSG_UPDATE_THEME = 0,               //  更新主题
    MSG_SET_WINDOW_TITLE,               //  设置 应用标题
    MSG_SLIDER_SHOW_STATE,              //  侧边栏 显隐消息
    MSG_MAGNIFYING,                     //  放大镜消息
    MSG_HANDLESHAPE,                    //  手型 消息
};

//  右键菜单消息类型
enum MSG_MENU_TYPE {
    MSG_OPERATION_OPEN_FILE = 50,       //  打开文件
    MSG_OPERATION_SAVE_FILE,            //  保存文件
    MSG_OPERATION_SAVE_AS_FILE,         //  另存为文件
    MSG_OPERATION_OPEN_FOLDER,          //  打开文件所处文件夹
    MSG_OPERATION_PRINT,                //  打印
    MSG_OPERATION_ATTR,                 //  属性
    MSG_OPERATION_FIND,                 //  搜索
    MSG_OPERATION_FULLSCREEN,           //  全屏
    MSG_OPERATION_SCREENING,            //  放映
    MSG_OPERATION_LARGER,               //  放大
    MSG_OPERATION_SMALLER,              //  缩小
    MSG_OPERATION_SWITCH_THEME,         //  主题切换

    MSG_OPERATION_ADD_BOOKMARK,         //  添加书签
    MSG_OPERATION_FIRST_PAGE,           //  第一页
    MSG_OPERATION_PREV_PAGE,            //  上一页
    MSG_OPERATION_NEXT_PAGE,            //  下一页
    MSG_OPERATION_END_PAGE,             //  最后一页

    MSG_OPERATION_TEXT_COPY,            //  复制
    MSG_OPERATION_TEXT_ADD_HIGHLIGHTED, //  添加 高亮显示
    MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED,   //  移除 高亮显示
    MSG_OPERATION_TEXT_ADD_BOOKMARK,    //  添加书签
    MSG_OPERATION_TEXT_ADD_ANNOTATION   //  添加注释
};

//  左侧 侧边栏 消息
enum MSG_LEFT_SLIDER_TYPE {
    MSG_THUMBNAIL_JUMPTOPAGE = 100,     //  跳转到指定页 消息
    MSG_BOOKMARK_DLTITEM,               //  删除指定书签 消息
    MSG_BOOKMARK_ADDITEM,               //  添加指定书签 消息
    MSG_NOTE_COPYCHOICECONTANT,         //  拷贝指定注释内容 消息
    MSG_NOTE_ADDCONTANT,                //  添加注释内容 消息
    MSG_NOTE_ADDITEM,                   //  添加注释子节点 消息
    MSG_NOTE_DLTNOTEITEM,               //  删除注释子节点 消息
    MSG_SWITCHLEFTWIDGET,               //  切换左侧窗口(缩略图、书签、注释) 消息
};

namespace ConstantMsg {
static int g_effective_res = 9999;  //  表明 消息已被处理
static int g_menu_width = 150;      //  菜单的宽度
}

#pragma clang diagnostic pop

#endif // MSGHEADER_H
