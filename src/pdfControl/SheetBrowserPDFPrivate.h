#ifndef FILEVIEWWIDGETPRIVATE_H
#define FILEVIEWWIDGETPRIVATE_H

#include <QObject>
#include <QMouseEvent>
#include <QPointer>

#include "Application.h"
#include "MsgHeader.h"
#include "pdfControl/docview/PageBase.h"
#include "ModuleHeader.h"
#include "widgets/FindWidget.h"

class Annotation;
class DocummentProxy;
class ProxyViewDisplay;
class ProxyMouseMove;
class ProxyFileDataModel;
class TipsWidget;
class NoteShadowViewWidget;
class TextOperationMenu;
class DefaultOperationMenu;
class ProxyData;
class SheetBrowserPDF;
class DocSheetPDF;
class SheetBrowserPDFPrivate: public QObject
{
    Q_OBJECT

public:
    SheetBrowserPDFPrivate(DocSheetPDF *sheet, SheetBrowserPDF *parent);

    ~SheetBrowserPDFPrivate() override;

public:
    void hidetipwidget();

    bool hasOpened();

    void mousePressLocal(bool &highLight, QPoint &point);

    void setMousePressLocal(const bool &highLight, const QPoint &point);

    QColor selectColor() const;

    void setSelectColor(const QColor &color);

    void deleteAnntation(const int &type, const QString &content);

private slots:
    void slotDealWithMenu(const int &, const QString &);

    void SlotNoteViewMsg(const int &, const QString &);

    void onAddHighLightAnnotation(const QString &msgContent);

    void onFileOpenResult(bool);

    void slotCustomContextMenuRequested(const QPoint &point);

    void onPageBookMarkButtonClicked(int, bool);

    void onPageChanged(int index);

    void setFindWidget(FindWidget *findWidget);

private:
    void wheelEvent(QWheelEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void AddHighLightAnnotation(const QString &msgContent);

private:
    void DocFile_ctrl_l();

    void DocFile_ctrl_i();

    void DocFile_ctrl_c();

    double handleResize(const QSize &size);

    void setSmallNoteWidgetSize(const QSize &size);
private:
    void showNoteViewWidget(const QString &, const QString &t_strUUid = "", const QString &sText = "", const int &nType = NOTE_HIGHLIGHT);
    void onOpenNoteWidget(const QString &msgContent);
    void onShowNoteWidget(const QString &contant);
    void __ShowNoteTipWidget(const QString &sText);
    void __CloseFileNoteWidget();
    void __SetCursor(const QCursor &cursor);
    void OpenFilePath(const QString &sPath);
    void clearSelect();
private:
    bool m_hasOpened = false;
    NoteShadowViewWidget    *m_pNoteViewWidget = nullptr;       //  注释内容显示框
    TipsWidget              *m_pTipWidget = nullptr;            //  注释提示框
    TextOperationMenu       *m_operatemenu = nullptr;
    DefaultOperationMenu    *m_pDefaultMenu = nullptr;
    ProxyData               *m_pProxyData = nullptr;            //  唯一 数据区
    ProxyFileDataModel      *m_pProxyFileDataModel = nullptr;
    ProxyMouseMove          *m_pProxyMouseMove = nullptr;       //  移动操作
    ProxyViewDisplay        *m_pDocViewProxy = nullptr;         //  文档操作 旋转\单双页\自适应
    Annotation              *m_pAnnotation = nullptr;           //  高亮 注释操作
    DocummentProxy          *m_pProxy = nullptr;                //  文档代理类
    QPoint                  m_popwidgetshowpoint;
    DocSheetPDF             *m_sheet;
    QPointer<FindWidget>    m_findWidget = nullptr;
    //start         add   by     dxh    2020-6-9
    QColor                  m_selectColor{"#FFA503"};           // 高亮颜色(默认)
    bool                    m_bIsHighLight = false;             // 鼠标左键点击位置有没有高亮
    QPoint                  m_point;                            // 鼠标左键点击位置
    QSize m_smallNoteSize;                                      // 注释小窗体的大小
    //end
    friend class Annotation;
    friend class ProxyViewDisplay;
    friend class ProxyMouseMove;
    friend class ProxyFileDataModel;

private:
    SheetBrowserPDF *const q_ptr = nullptr;
    Q_DECLARE_PUBLIC(SheetBrowserPDF)
};

#endif // FILEVIEWWIDGETPRIVATE_H
