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
    E_TAB_MSG,
    E_DOCPROXY_MSG_TYPE,
    E_FILE_MSG,
    E_TITLE_MSG_TYPE,
    E_SHORTKEY_MSG_TYPE,
    E_MSG_TYPE_END
};

//  应用类 消息
enum E_APP_MSG_TYPE {
    E_APP_MSG_TYPE_BEGIN = E_MSG_TYPE_END + 50,
    E_APP_EXIT,
    E_APP_EXIT_NOTHING,
    E_APP_MSG_TYPE_END
};

//  central 消息
enum E_CENTRAL_MSG {
    E_CENTRAL_MSG_BEGIN = E_MSG_TYPE_END + 50,
    CENTRAL_INDEX_CHANGE,
    CENTRAL_SHOW_TIP,                //  显示提示窗口
    E_CENTRAL_MSG_END
};

//  消息类型
enum MSG_TYPE {
    MSG_TYPE_BEGIN = E_CENTRAL_MSG_END + 50,
    MSG_MAGNIFYING,             //  放大镜消息
    MSG_MAGNIFYING_CANCEL,          //  取消放大镜消息
    MSG_HANDLESHAPE,                //  手型 消息

    MSG_OPERATION_UPDATE_THEME,         //  主题变了
    MSG_NOTIFY_KEY_MSG,                 //  按键通知消息
    MSG_NOTIFY_KEY_PLAY_MSG,            //  幻灯片模式按键通知消息
    MSG_EXIT_SAVE_FILE,                 //  关闭当前文档　消息
    MSG_EXIT_NOT_SAVE_FILE,             //  关闭当前文档　消息
    MSG_EXIT_NOT_CHANGE_FILE,             //  关闭当前文档　消息
    MSG_CLOSE_FILE,                     //  关闭当前文档　消息
    MSG_JSON_NOTIFY_TYPE,
    MSG_TYPE_END,
};

//  多标签消息
enum E_TAB_TYPE {
    MSG_TAB_MSG_BEGIN = MSG_TYPE_END + 50,
    MSG_TAB_ADD,                    //  添加 tab 消息
    MSG_TAB_REMOVE,                 //  删除 tab 消息
    MSG_TAB_DATA_CHANGE,            //  文档数据有变化消息
    MSG_TAB_SAVE_FILE,              //  关闭当前文档　消息
    MSG_TAB_SHOW_FILE_CHANGE,       //  文档 显示切换消息
    MSG_TAB_NOT_SAVE_FILE,          //  关闭当前文档　消息
    MSG_TAB_NOT_CHANGE_SAVE_FILE,   //  关闭当前文档　消息
    MSG_TAB_MSG_END,
};

//  菜单操作消息类型
enum E_MENU_MSG {
    E_MENU_MSG_BEGIN = MSG_TAB_MSG_END + 50,
    MSG_MENU_NEW_WINDOW,                // 新窗口打开一个文档
    MSG_MENU_NEW_TAB,                   // 新增加一个文档
    MSG_MENU_OPEN_FOLDER,               //  打开文件所处文件夹
    MSG_MENU_MAGNIFER,                  //  放大镜
    MSG_MENU_SCALE_CHANGE,              //  缩放比例变化
    MSG_OPERATION_ATTR,               //  属性
    MSG_OPERATION_SLIDE,              //  放映
    MSG_OPERATION_EXIT,               //  退出
    MSG_OPERATION_TEXT_COPY,          //  复制
};

//  左侧 侧边栏 消息
enum E_LEFT_SLIDER_MSG {
    QQQQQQQQ_NO_USE = MSG_OPERATION_TEXT_COPY + 50, //  侧边栏 显隐消息
    MSG_HIDE_FIND_WIDGET,       //  侧边栏 显隐消息
    MSG_SWITCHLEFTWIDGET,       //  切换左侧窗口(缩略图、书签、注释) 消息
};

//  字体菜单 消息
enum E_FONT_MENU_MSG {
    MSG_SELF_ADAPTE_HEIGHT = MSG_SWITCHLEFTWIDGET + 50,  //  自适应高度

    MSG_FILE_ROTATE,               //  旋转
};

//  搜索 消息
enum E_FIND_MSG {
    MSG_FIND_START = MSG_FILE_ROTATE + 50,           //  开始查询
    MSG_CLEAR_FIND_CONTENT,         //  清除查询内容
    MSG_FIND_NONE,                  //  查询无果
    MSG_FIND_EXIT                   //  退出查询
};

//  文档操作消息
enum E_DOC_TYPE_MSG {
    MSG_DOC_JUMP_PAGE = MSG_FIND_EXIT + 50,             //  请求跳转页面
    MSG_OPEN_FILE_PATH,             //  打开的文件消息
//    MSG_OPEN_FILE_PATH_S,           //  打开的文件消息
    MSG_OPERATION_OPEN_FILE_OK,     //  打开文件成功, 要告诉所有人
    MSG_DOC_OPEN_FILE_START,      //  开始打开文件
    MSG_OPERATION_OPEN_FILE_FAIL,       //  打开文件失败
    MSG_OPERATION_FIRST_PAGE,                           //  第一页
    MSG_OPERATION_PREV_PAGE,                            //  上一页
    MSG_OPERATION_NEXT_PAGE,                            //  下一页
    MSG_OPERATION_END_PAGE,                             //  最后一页
    MSG_FILE_PAGE_CHANGE,                               //  文档 页码变化消息
};

//  书签消息
enum E_BOOKMARK_MSG {
    MSG_OPERATION_ADD_BOOKMARK = MSG_FILE_PAGE_CHANGE + 50,           //  添加书签
    MSG_OPERATION_TEXT_ADD_BOOKMARK,        //  添加书签
    MSG_OPERATION_DELETE_BOOKMARK,          //  删除书签
    MSG_BOOKMARK_DLTITEM,                   //  删除指定书签 消息
    MSG_OPERATION_RIGHT_SELECT_BOOKMARK,    //  右键选择书签
};


//  高亮&注释消息
enum E_NOTE_MSG {
    E_NOTE_MSG_BEGIN = MSG_OPERATION_RIGHT_SELECT_BOOKMARK + 50,
    MSG_NOTE_ADD_CONTENT,        //  添加注释内容 消息
    MSG_NOTE_ADD_HIGHLIGHT_COLOR,       //  添加高亮
    MSG_NOTE_UPDATE_HIGHLIGHT_COLOR,    //  更新 高亮显示
    MSG_NOTE_REMOVE_HIGHLIGHT_COLOR,    //  移除 高亮显示
    MSG_NOTE_ADD_HIGHLIGHT_NOTE,        //  添加高亮注释
    MSG_OPERATION_TEXT_ADD_ANNOTATION,      //  菜单点击, 添加注释
    MSG_OPERATION_TEXT_SHOW_NOTEWIDGET,     //  菜单点击, 已经有注释内容, 显示注释窗口
    MSG_NOTE_ADD_ITEM,                  //  添加注释子节点
    MSG_NOTE_UPDATE_CONTENT,            //  更新高亮注释内容
    MSG_NOTE_UPDATE_ITEM,               //  更新高亮注释节点
    MSG_NOTE_DELETE_CONTENT,            //  删除注释内容 消息
    MSG_NOTE_DELETE_ITEM,               //  删除注释子节点
    MSG_NOTE_SELECT_ITEM,               //  选择注释子节点 消息
    MSG_NOTE_PAGE_ADD,                  //  页面注释显示 可以点击添加
    MSG_NOTE_PAGE_SHOW_NOTEWIDGET,      //  页面注释显示 注释内容界面
    MSG_NOTE_PAGE_ADD_CONTENT,          //  页面注释 添加内容
    MSG_NOTE_PAGE_ADD_ITEM,             //  页面注释 添加节点
    MSG_NOTE_PAGE_UPDATE_CONTENT,       //  页面注释 更新内容
    MSG_NOTE_PAGE_UPDATE_ITEM,          //  页面注释 更新节点
    MSG_NOTE_PAGE_DELETE_CONTENT,       //  页面注释 删除内容
    MSG_NOTE_PAGE_DELETE_ITEM,          //  页面注释 删除节点
    E_NOTE_MSG_END,
};

//  文档显示变化消息
enum E_FILE_VALUE_MSG {
    E_FILE_VALUE_MSG_BEGIN = E_NOTE_MSG_END + 50,
    MSG_TAB_FILE_CHANGE,                //  文档显示切换
    MSG_WIDGET_THUMBNAILS_VIEW,         //  文档 左侧缩略图展开状态
    MSG_VIEWCHANGE_DOUBLE_SHOW,         //  双页显示状态
    MSG_VIEWCHANGE_FIT,                 //  自适应
    MSG_VIEWCHANGE_ROTATE,              //  旋转
    MSG_HANDLE_SELECT,                  //  选择工具
    MSG_FILE_SCALE,                     //  当前比例
    MSG_FILE_IS_CHANGE,                 //  文档是否有变化
    MSG_LEFTBAR_STATE,                  //  文档是否有变化
    E_FILE_VALUE_MSG_END
};

}

namespace ConstantMsg {

const QString g_app_name = "deepin-reader";
const QString g_db_name = "deepinreader.db";
const QString g_cfg_name = "config.conf";

//const QString g_warningtip_suffix = "##**warning";
//const QString g_errortip_suffix = "##**error";

const int g_effective_res = 9999;  //  表明 消息已被处理
//const int g_menu_width = 150;      //  菜单的宽度
}  // namespace ConstantMsg

#endif  // MSGHEADER_H
