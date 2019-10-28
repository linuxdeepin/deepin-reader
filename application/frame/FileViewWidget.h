#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <QMouseEvent>
#include <QAction>
#include <DMenu>

#include "subjectObserver/CustomWidget.h"
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
    void sigSetHandShape(const QString &);
    void sigMagnifying(const QString &);

    void sigPrintFile();
    void sigWidgetAdapt();

    void sigFileAddAnnotation(const QString &);
    void sigFileRemoveAnnotation();

    void sigFileAddNote(const QString &);

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotCustomContextMenuRequested(const QPoint &);

    void slotSetHandShape(const QString &);
    void slotMagnifying(const QString &);
    void slotPrintFile();
    void slotSetWidgetAdapt();

    void slotFileAddAnnotation(const QString &);
    void slotFileRemoveAnnotation();

    void slotFileAddNote(const QString &);

private:
    void initConnections();

    int dealWithTitleRequest(const int &msgType, const QString &msgContent);
    int dealWithFileMenuRequest(const int &msgType, const QString &msgContent);

private:
    DocummentFileHelper     *m_pDocummentFileHelper = nullptr;

    DocummentProxy          *m_pDocummentProxy = nullptr;

private:
    int         m_nCurrentHandelState = Default_State;  //  当前鼠标状态
    int         m_nAdapteState = Default_State;         //  当前自适应状态
    bool        m_bSelectOrMove = false;      //  是否可以选中文字、移动
    QPoint      m_pStartPoint;
    QPoint      m_pRightClickPoint;            //   右键菜单点
    QPoint      m_pHandleMoveStartPoint;
    QPoint      m_pMoveEndPoint;

    QString m_strUUid;                         // 当前添加注释的uuid
    bool m_bIsHighLight = false;               // 判断鼠标点击位置是否有高亮
    bool m_bIsHighLightReleasePoint = false;   // 判断左键鼠标释放的位置是否有高亮
    int m_nPage = -1;                          // 鼠标当前点击所在页码数

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};


#endif // FILEVIEWWIDGET_H
