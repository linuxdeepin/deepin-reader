#ifndef MSGHEADER_H
#define MSGHEADER_H

#include <QString>

/**
 *  消息类型 定义
 */

//  消息类型
enum MSG_TYPE {
    //    MSG_SET_MainWindow_TITLE = 0,       //  设置 应用标题
    MSG_SLIDER_SHOW_STATE = 0,  //  侧边栏 显隐消息
    MSG_HIDE_FIND_WIDGET,       //  侧边栏 显隐消息
    //    MSG_BOOKMARK_SHOW_STATE,            //  书签状态 显隐消息
    MSG_FILE_PAGE_CHANGE,   //  文档 页码变化消息
    MSG_MAGNIFYING,         //  放大镜消息
    MSG_MAGNIFYING_CANCEL,  //  取消放大镜消息
    MSG_HANDLESHAPE,        //  手型 消息
    MSG_OPEN_FILE_PATH,     //  打开的文件消息
    MSG_OPEN_FILE_PATH_S    //  打开的文件消息
};

//  右键菜单消息类型
enum MSG_MENU_TYPE {
    //    MSG_OPERATION_OPEN_FILE = 50,       //  打开文件
    //    MSG_OPERATION_SAVE_FILE,            //  保存文件
    MSG_OPERATION_SAVE_AS_FILE = 50,  //  另存为文件
    MSG_OPERATION_OPEN_FOLDER,        //  打开文件所处文件夹
    //    MSG_OPERATION_PRINT,                //  打印
    MSG_OPERATION_ATTR,               //  属性
    //    MSG_OPERATION_FIND,                 //  搜索
    //    MSG_OPERATION_FULLSCREEN,           //  全屏
    MSG_OPERATION_SLIDE,              //  放映
    //    MSG_OPERATION_LARGER,               //  放大
    //    MSG_OPERATION_SMALLER,              //  缩小
    //    MSG_OPERATION_HELP,                 //  帮助
    MSG_OPERATION_EXIT,               //  退出

    MSG_OPERATION_ADD_BOOKMARK,           //  添加书签
    MSG_OPERATION_DELETE_BOOKMARK,        //  删除书签
    MSG_OPERATION_RIGHT_SELECT_BOOKMARK,  //  右键选择书签
    MSG_OPERATION_FIRST_PAGE,             //  第一页
    MSG_OPERATION_PREV_PAGE,              //  上一页
    MSG_OPERATION_NEXT_PAGE,              //  下一页
    MSG_OPERATION_END_PAGE,               //  最后一页

    MSG_OPERATION_TEXT_COPY,                //  复制
    MSG_OPERATION_TEXT_ADD_HIGHLIGHTED,     //  添加 高亮显示
    MSG_OPERATION_TEXT_UPDATE_HIGHLIGHTED,  //  更新 高亮显示
    MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED,  //  移除 高亮显示
    MSG_OPERATION_TEXT_ADD_BOOKMARK,        //  添加书签
    MSG_OPERATION_TEXT_ADD_ANNOTATION,      //  添加注释
    MSG_OPERATION_TEXT_SHOW_NOTEWIDGET,     //  显示注释窗口
};

//  左侧 侧边栏 消息
enum MSG_LEFT_SLIDER_TYPE {
    MSG_BOOKMARK_DLTITEM = 100,  //  删除指定书签 消息
    //    MSG_BOOKMARK_ADDITEM,               //  添加指定书签 消息
    //    MSG_NOTE_COPYCHOICECONTANT,         //  拷贝指定注释内容 消息
    MSG_NOTE_ADDCONTANT,      //  添加注释内容 消息
    MSG_NOTE_ADDITEM,         //  添加注释子节点 消息
    MSG_NOTE_DLTNOTECONTANT,  //  删除注释内容 消息
    MSG_NOTE_DLTNOTEITEM,     //  删除注释子节点 消息
    MSG_NOTE_SELECTITEM,      //  选择注释子节点 消息
    MSG_SWITCHLEFTWIDGET,     //  切换左侧窗口(缩略图、书签、注释) 消息
    //    MSG_BOOKMARK_STATE,                 //  文件页码书签状态
    MSG_CLOSE_FILE,           //  关闭当前文档　消息
};

//  字体菜单 消息
enum MSG_FONT_MENU_TYPE {
    //    MSG_SCALEUP_MULTIPLE = 150,     //  缩放倍数
    //    MSG_DOUB_PAGE_VIEW,             //  双页视图
    MSG_SELF_ADAPTE_HEIGHT = 150,  //  自适应高度
    MSG_SELF_ADAPTE_WIDTH,         //  自适应宽度
    MSG_SELF_ADAPTE_SCALE,         //  自适应宽\高 缩放比例变化
    MSG_FILE_ROTATE,               //  旋转
};

//  搜索菜单 消息
enum MSG_FIND_TYPE {
    MSG_FIND_CONTENT = 200,     //  查询内容
    MSG_CLEAR_FIND_CONTENT,     //  清除查询内容
    MSG_FIND_PREV,              //  上一个
    MSG_FIND_NEXT,              //  下一个
    MSG_FIND_START,             //  开始查询
    MSG_FIND_STOP,              //  结束查询
    MSG_FIND_NONE,              //  查询无果
    MSG_FIND_EXIT               //  退出查询
};

//  菜单操作结果 消息
enum MSG_OPERATION_RES_TYPE {
    MSG_OPERATION_OPEN_FILE_OK = 1000,  //  打开文件成功, 要告诉所有人
    MSG_OPERATION_OPEN_FILE_START,      //  开始打开文件
    MSG_OPERATION_OPEN_FILE_TITLE,      //  打开文件成功, 文件标题
    MSG_OPERATION_OPEN_FILE_FAIL,       //  打开文件失败
    MSG_OPERATION_UPDATE_THEME,         //  主题变了
    MSG_NOTIFY_KEY_MSG,                 //  按键通知消息
    MSG_NOTIFY_SHOW_TIP,                //  显示提示窗口
};

namespace ConstantMsg {

const QString g_app_name = "deepin-reader";
const QString g_db_name = "deepinreader.db";
const QString g_cfg_name = "config.conf";

const int g_effective_res = 9999;  //  表明 消息已被处理
const int g_menu_width = 150;      //  菜单的宽度
}  // namespace ConstantMsg

#endif  // MSGHEADER_H
