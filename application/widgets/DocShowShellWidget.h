#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "CustomControl/CustomWidget.h"

/**
 * @brief The DocShowShellWidget class
 * @brief   文档　显示外壳
 */
class PlayControlWidget;

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

//    void sigDealWithData(const int &, const QString &);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotShowCloseBtn(const int &);
    void slotHideCloseBtn();
    void slotShowFindWidget();
    void slotBtnCloseClicked();
    void slotUpdateTheme();
    void slotChangePlayCtrlShow(bool bshow);

    void slotDealWithData(const int &, const QString &msgContent = "");

private:
    void initConnections();
    int dealWithNotifyMsg(const QString &);

    void onShowFileAttr();
    void onOpenNoteWidget(const QString &);
    void onShowNoteWidget(const QString &);

    /****   begin   wzx 2020.2.18   ********/
    void __ShowPageNoteWidget(const QString &);
    /****   end     wzx 2020.2.18   ********/
    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    PlayControlWidget *m_pctrlwidget = nullptr;

    // QWidget interface
protected:
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
};

#endif // DOCSHOWSHELLWIDGET_H
