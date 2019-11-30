#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "CustomControl/CustomWidget.h"
#include "mainShow/FileAttrWidget.h"

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
    void sigShowFileAttr();
    void sigShowFileFind();
    void sigOpenNoteWidget(const QString &);
    void sigShowNoteWidget(const QString &);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotShowCloseBtn(const int &);
    void slotHideCloseBtn();
    void slotShowFileAttr();
    void slotShowFindWidget();
    void slotOpenNoteWidget(const QString &);
    void slotShowNoteWidget(const QString &);
    void slotBtnCloseClicked();
    void slotUpdateTheme();

private:
    void initConnections();
    int dealWithNotifyMsg(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;
};

#endif // DOCSHOWSHELLWIDGET_H
