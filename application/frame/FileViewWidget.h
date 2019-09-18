#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <QMouseEvent>
#include <QAction>
#include <DMenu>

#include "subjectObserver/CustomWidget.h"
#include "mainShow/DefaultOperationWidget.h"
#include "mainShow/TextOperationWidget.h"
#include "mainShow/FileAttrWidget.h"
#include "mainShow/FindWidget.h"
#include "mainShow/BookMarkStateLabel.h"

#include "DocummentFileHelper.h"

//  当前鼠标状态
enum Handel_Enum {
    Default_State,
    Handel_State,
    Magnifier_State
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
    ~FileViewWidget() override;

signals:
//    void sigCopyContent();
    void sigSetMarkState(const bool &);
    void sigShowFileAttr();
    void sigShowFileFind();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void slotCustomContextMenuRequested(const QPoint &);
    void slotShowFileAttr();
    void slotShowFindWidget();

private:
    int magnifying(const QString &);
    int setHandShape(const QString &);

//    void onSaveFile();
//    void onCopySelectContent();

    void initConnections();

    int dealWithTitleMenuRequest(const int &msgType, const QString &msgContent);
    int dealWithFileMenuRequest(const int &msgType, const QString &msgContent);

    void setBookMarkStateWidget();


private:
    BookMarkStateLabel      *m_pBookMarkStateLabel = nullptr;
    FindWidget              *m_pFindWidget = nullptr;
    FileAttrWidget          *m_pFileAttrWidget = nullptr;
    DefaultOperationWidget  *m_pDefaultOperationWidget = nullptr;
    TextOperationWidget     *m_pTextOperationWidget = nullptr;

    DocummentFileHelper     *m_pDocummentFileHelper = nullptr;

private:
    int         m_nCurrentHandelState = Default_State; //  当前鼠标状态
    bool        m_bSelectOrMove = false;      //  是否可以选中文字、移动
    QPoint      m_pStartPoint;
    QPoint      m_pMoveStartPoint;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};


#endif // FILEVIEWWIDGET_H
