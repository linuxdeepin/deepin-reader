#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "subjectObserver/CustomWidget.h"
#include "mainShow/FindWidget.h"
#include "mainShow/FileAttrWidget.h"
#include "mainShow/BookMarkStateLabel.h"


/**
 * @brief The DocShowShellWidget class
 * @brief   文档　显示外壳
 */


class DocShowShellWidget : public CustomWidget
{
    Q_OBJECT
public:
    DocShowShellWidget(CustomWidget *parent = nullptr);
    ~DocShowShellWidget();

signals:
    void sigShowFileAttr();
    void sigShowFileFind();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setBookMarkStateWidget();

private slots:
    void slotShowFileAttr();
    void slotShowFindWidget();

private:
    void initConnections();

private:
    FileAttrWidget          *m_pFileAttrWidget = nullptr;
    FindWidget              *m_pFindWidget = nullptr;
    BookMarkStateLabel      *m_pBookMarkStateLabel = nullptr;


    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;
};

#endif // DOCSHOWSHELLWIDGET_H
