#ifndef MODULEHEADER_H
#define MODULEHEADER_H

#include <QString>

namespace ImageModule {

const QString g_normal_state = "normal";
const QString g_hover_state = "hover";
const QString g_press_state = "press";
const QString g_checked_state = "checked";
const QString g_focus_state = "focus";
const QString g_disable_state = "disable";

const QString g_dark_theme = "dark";
const QString g_light_theme = "light";
const QString g_system_theme = "system";

}  // namespace ImageModule

namespace Constant {
const QString sAcknowledgementLink = "https://www.deepin.org/acknowledgments/deepin_reader";

const QString sPdf_Filter = "Pdf File (*.pdf)";
const QString sTiff_Filter = "Tiff files (*.tiff)";
const QString sPs_Filter = "Ps files (*.ps)";
const QString sXps_Filter = "Xps files (*.xps)";
const QString sDjvu_Filter = "Djvu files (*.djvu)";
const QString sQStringSep = "@#$!!#$$@@#&!@#$";

const QString sLogoPath = ":/resources/logo/deepin-reader.svg";
}  // namespace Constant

namespace Pri {
const QString g_controller = "controller";
const QString g_docview = "docview";
const QString g_frame = "frame";
const QString g_mainShow = "mainShow";
const QString g_pdfControl = "pdfControl";
const QString g_subjectObserver = "subjectObserver";

// 图标类型
const QString g_icons = "icons";
const QString g_texts = "texts";
const QString g_actions = "actions";
}  // namespace Pri

namespace KeyStr {
const QString g_ctrl_alt_f = "Ctrl+Alt+F";  //  窗口大小切换
const QString g_f11 = "F11";                //  全屏
const QString g_esc = "Esc";                //  退出全屏\退出放映\退出放大镜
const QString g_alt_f4 = "Alt+F4";          //  退出应用程序
const QString g_f1 = "F1";                  //  帮助
const QString g_ctrl_f = "Ctrl+F";          //  搜索

const QString g_pgup = "PgUp";            //  上一页
const QString g_pgdown = "PgDown";        //  下一页
const QString g_ctrl_o = "Ctrl+O";        //  打开
const QString g_ctrl_larger = "Ctrl+=";   //  放大
const QString g_ctrl_smaller = "Ctrl+-";  //  缩小
// const QString g_ctrl_wheel = "Ctrl+wheel"; //ctrl+滚轮放大缩小

const QString g_ctrl_shift_s = "Ctrl+Shift+S";  //  另存为
const QString g_ctrl_e = "Ctrl+E";              //  导出
const QString g_ctrl_p = "Ctrl+P";              //  打印
const QString g_ctrl_s = "Ctrl+S";              //  保存
const QString g_m = "M";                        //  打开目标缩略图

const QString g_ctrl_1 = "Ctrl+1";              //  适合页面状态
const QString g_ctrl_2 = "Ctrl+2";              //  适合高度
const QString g_ctrl_3 = "Ctrl+3";              //  适合宽度
const QString g_ctrl_r = "Ctrl+R";              //  左旋转
const QString g_ctrl_shift_r = "Ctrl+Shift+R";  //  右旋转

const QString g_alt_1 = "Alt+1";  //    选择工具
const QString g_alt_2 = "Alt+2";  //    手型工具

const QString g_ctrl_b = "Ctrl+B";  //    添加书签
const QString g_ctrl_i = "Ctrl+I";  //    添加注释
const QString g_ctrl_l = "Ctrl+L";  //    添加高亮
const QString g_del = "Del";        //  删除书签\注释\高亮

const QString g_z = "Z";  //  放大镜

const QString g_up = "Up";
const QString g_down = "Down";
const QString g_left = "Left";
const QString g_right = "Right";
const QString g_space = "Space";  //空格用于停止启动幻灯片播放

const QString g_ctrl_c = "Ctrl+C";
const QString g_ctrl_x = "Ctrl+X";
const QString g_ctrl_v = "Ctrl+V";
const QString g_ctrl_z = "Ctrl+Z";
const QString g_ctrl_a = "Ctrl+A";
const QString g_ctrl_shift_slash = "Ctrl+Shift+?";
}  // namespace KeyStr

#endif  // IMAGEHEADER_H
