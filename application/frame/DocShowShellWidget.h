#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "subjectObserver/CustomWidget.h"
#include "mainShow/FindWidget.h"
#include "mainShow/FileAttrWidget.h"

/**
 * @brief The DocShowShellWidget class
 * @brief   文档　显示外壳
 */


class DocShowShellWidget : public CustomWidget
{
    Q_OBJECT
public:
    DocShowShellWidget(CustomWidget *parent = nullptr);
    ~DocShowShellWidget() override;

signals:
    void sigShowFileAttr();
    void sigShowFileFind();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void slotShowFileAttr();
    void slotShowFindWidget();

private:
    void initConnections();

private:
    FileAttrWidget          *m_pFileAttrWidget = nullptr;
    FindWidget              *m_pFindWidget = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;
};

#endif // DOCSHOWSHELLWIDGET_H
