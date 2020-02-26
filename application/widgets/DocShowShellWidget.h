#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "CustomControl/CustomWidget.h"

#include <DSpinner>

class PlayControlWidget;
class QStackedLayout;

/**
 * @brief The DocShowShellWidget class
 * @brief   文档　显示外壳
 */

class DocShowShellWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DocShowShellWidget)

public:
    explicit DocShowShellWidget(CustomWidget *parent = nullptr);
    ~DocShowShellWidget() Q_DECL_OVERRIDE;

signals:
    void sigShowCloseBtn(const int &);
    void sigHideCloseBtn();
    void sigShowFileFind();
    void sigChangePlayCtrlShow(bool bshow);
    void sigOpenFileOk(const QString &);

//    void sigDealWithData(const int &, const QString &);
    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void qSetPath(const QString &);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotShowCloseBtn(const int &);
    void slotHideCloseBtn();
    void slotShowFindWidget();
    void slotBtnCloseClicked();
    void slotUpdateTheme();
    void slotChangePlayCtrlShow(bool bshow);
    void slotOpenFileOk(const QString &);

    void slotDealWithData(const int &, const QString &msgContent = "");

private:
    void initConnections();
    int dealWithNotifyMsg(const QString &);

    void onShowFileAttr();
    void onOpenNoteWidget(const QString &);
    void onShowNoteWidget(const QString &);

    /****   begin   wzx 2020.2.18   ********/
    void __ShowPageNoteWidget(const QString &);
    void InitSpinner();
    /****   end     wzx 2020.2.18   ********/

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    PlayControlWidget *m_pctrlwidget = nullptr;
    DSpinner            *m_pSpiner = nullptr;
    DWidget             *m_pSpinerWidget = nullptr;
    QStackedLayout      *m_playout = nullptr;
};

#endif // DOCSHOWSHELLWIDGET_H
