#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "subjectObserver/CustomWidget.h"
#include "mainShow/FindWidget.h"
#include "mainShow/FileAttrWidget.h"

#include "pdfControl/fileViewNote/FileViewNoteWidget.h"

/**
 * @brief The DocShowShellWidget class
 * @brief   文档　显示外壳
 */

class DocShowShellWidget : public CustomWidget
{
    Q_OBJECT
public:
    DocShowShellWidget(CustomWidget *parent = nullptr);
    ~DocShowShellWidget() Q_DECL_OVERRIDE;

signals:
    void sigShowFileAttr();
    void sigShowFileFind();
    void sigOpenNoteWidget(const QString &);
    void sigShowNoteWidget(const QString &);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotShowFileAttr();
    void slotShowFindWidget();
    void slotOpenNoteWidget(const QString &);
    void slotShowNoteWidget(const QString &);

private:
    void initConnections();
    int dealWithNotifyMsg(const QString &);

private:
    FileAttrWidget          *m_pFileAttrWidget = nullptr;
    FindWidget              *m_pFindWidget = nullptr;
    FileViewNoteWidget      *m_pFileViewNoteWidget = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;
};

#endif // DOCSHOWSHELLWIDGET_H
