#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <QMouseEvent>

#include "CustomControl/CustomWidget.h"
#include "DocummentFileHelper.h"
#include "NoteTipWidget.h"
#include "mainShow/TextOperationMenu.h"

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
    Q_DISABLE_COPY(FileViewWidget)

public:
    explicit FileViewWidget(CustomWidget *parent = nullptr);
    ~FileViewWidget() Q_DECL_OVERRIDE;

signals:
//    void sigDealWithData(const int &, const QString &);
//    void sigDealWithKeyMsg(const QString &);

    void sigShowPlayCtrl(bool bshow);

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;

private slots:
    void slotDealWithData(const int &, const QString &);
    void slotDealWithKeyMsg(const QString &);

    void slotCustomContextMenuRequested(const QPoint &);

    void slotBookMarkStateChange(int, bool);
    void slotDocFilePageChanged(int);
    void SlotDocFileOpenResult(bool);

private:
    void initConnections();
    void onClickPageLink(Page::Link *pLink);
    void onShowNoteTipWidget(const QPoint &docPos);

    void onMagnifying(const QString &);
    void onSetHandShape(const QString &);
    void onSetWidgetAdapt();

    void onFileAddAnnotation(const QString &);
    void onFileUpdateAnnotation(const QString &);
    void onFileRemoveAnnotation(const QString &);

    void onFileAddNote(const QString &);

private:
    void onPrintFile();
    void onFileAddAnnotation();
    void onFileAddNote();

private:
    TextOperationMenu       *m_operatemenu = nullptr;
    DocummentFileHelper     *m_pDocummentFileHelper = nullptr;
    NoteTipWidget           *m_pNoteTipWidget = nullptr;

private:
    int         m_nCurrentHandelState = Default_State;  //  当前鼠标状态
    int         m_nAdapteState = NO_ADAPTE_State;       //  当前自适应状态
    bool        m_bSelectOrMove = false;                //  是否可以选中文字、移动
    bool        m_bIsHandleSelect = false;              //  是否可以选中
    bool        m_bmousepressed = false;                //  鼠标是否被按住
    QString         struidtmp;
    QPoint      m_pStartPoint;
    QPoint      m_pEndSelectPoint;
    QPoint      m_pHandleMoveStartPoint;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};


#endif // FILEVIEWWIDGET_H
