#ifndef FRAME_H
#define FRAME_H

#include <QObject>

//  用于　翻译

namespace Frame {

const QString sAppName = QObject::tr("deepin_reader");
const QString sDragFile = QObject::tr("drag Pdf file here");
const QString sDescription = QObject::tr("Document viewer is a document viewer that comes with the deep operating system. \r\n"
                                         "In addition to opening and reading PDF files, you can also do \r\n on documents"
                                         "Bookmark, comment, and highlight selected text．");

const QString sSelectFile = QObject::tr("Select File");     //  选择文件

const QString sOpenFile = QObject::tr("Open File");         //  打开文件
const QString sSaveFile = QObject::tr("Save File");         //  保存文件
const QString sSaveAsFile = QObject::tr("Save As File");    //  另存为文件
const QString sOpenFolder = QObject::tr("Open Folder");     //  打开所在文件夹

const QString sPrint = QObject::tr("Print");                //  打印
const QString sPrintErrorNoDevice = QObject::tr("No Print Device");                //  没有打印设备

const QString sFileAttr = QObject::tr("File Attr");         //  属性
const QString sSearch = QObject::tr("Search");              //  搜索
const QString sFullScreen = QObject::tr("Full Screen");     //  全屏
const QString sScreening = QObject::tr("Screening");        //  放映
const QString sLarger = QObject::tr("Larger");              //  放大
const QString sSmaller = QObject::tr("Smaller");            //  缩小

const QString sThumbnail = QObject::tr("thumbnail");        //  缩略图
const QString sBookmark = QObject::tr("bookmark");          //　书签
const QString sAnnotation = QObject::tr("annotation");      //　注释

const QString sSetting = QObject::tr("setting");            //  设置
const QString sDefaultShape = QObject::tr("defaultShape");  //  默认手型
const QString sHandleShape = QObject::tr("handleShape");    //  手型

const QString sMagnifier = QObject::tr("magnifier");        //  放大镜
}


#endif // FRAME_H
