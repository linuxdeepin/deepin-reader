#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <QMouseEvent>
#include <QAction>
#include <DMenu>

#include "subjectObserver/CustomWidget.h"
#include "mainShow/DefaultOperationWidget.h"
#include "mainShow/TextOperationWidget.h"
#include "mainShow/BookMarkStateLabel.h"
#include "pdfControl/fileViewNote/FileViewNoteWidget.h"
#include "DocummentFileHelper.h"

//  当前鼠标状态
enum Handel_Enum {
    Default_State,
    Handel_State,
    Magnifier_State
};

//  窗口自适应状态
enum ADAPTE_Enum {
    NO_ADAPTE_State,
    WIDGET_State,
    HEIGHT_State
};

/**
 * @brief The FileViewWidget class
 * @brief   文档显示区域
 */

class FileViewWidget : public CustomWidget
{
    Q_OBJECT
public:
    FileViewWidget(CustomWidget *parent = nullptr);
    ~FileViewWidget() Q_DECL_OVERRIDE;

signals:
    void sigPrintFile();
    void sigOpenNoteWidget();
    void sigWidgetAdapt();

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotCustomContextMenuRequested(const QPoint &);
    void slotPrintFile();
    void slotOpenNoteWidget();
    void slotSetWidgetAdapt();

private:
    int magnifying(const QString &);
    int setHandShape(const QString &);
    void setBookMarkStateWidget();

    void onFileAddAnnotation(const QString &);
    void onFileRemoveAnnotation();

    void initConnections();

    int dealWithTitleRequest(const int &msgType, const QString &msgContent);
    int dealWithFileMenuRequest(const int &msgType, const QString &msgContent);

private:
    BookMarkStateLabel      *m_pBookMarkStateLabel = nullptr;
    DefaultOperationWidget  *m_pDefaultOperationWidget = nullptr;
    TextOperationWidget     *m_pTextOperationWidget = nullptr;
    FileViewNoteWidget      *m_pFileViewNoteWidget = nullptr;

    DocummentFileHelper     *m_pDocummentFileHelper = nullptr;

private:
    int         m_nCurrentHandelState = Default_State;  //  当前鼠标状态
    int         m_nAdapteState = Default_State;         //  当前自适应状态
    bool        m_bSelectOrMove = false;      //  是否可以选中文字、移动
    QPoint      m_pStartPoint;
    QPoint      m_pRightClickPoint;            //   右键菜单点
    QPoint      m_pHandleMoveStartPoint;
    QPoint      m_pMoveEndPoint;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};


#endif // FILEVIEWWIDGET_H
