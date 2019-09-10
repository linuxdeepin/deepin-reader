#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <QMouseEvent>
#include <QAction>
#include <DMenu>
#include "mainShow/MagnifyLabel.h"
#include "subjectObserver/CustomWidget.h"
#include "mainShow/DefaultOperationWidget.h"
#include "mainShow/TextOperationWidget.h"
#include "mainShow/FileAttrWidget.h"
#include "mainShow/FindWidget.h"

#include "docview/docummentproxy.h"

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
    void sigSetMagnifyingImage(const QImage &);
    void sigShowFileAttr();
    void sigShowFindWidget();
    void sigOpenFile(const QString &);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void slotShowFileAttr();
    void slotShowFindWidget();
    void on_slot_openfile(const QString &filePath);
    void SlotCustomContextMenuRequested(const QPoint &);
    void openFilePath(const QString &);

private:

    int magnifying(const QString &);
    int setHandShape(const QString &);
    int screening(const QString &);
    int openFileFolder();

    void initConnections();

private:
    MagnifyLabel            *m_pMagnifyLabel = nullptr;
    bool m_bCanVisible = false; //  放大镜 是否可以显示

    FindWidget              *m_pFindWidget = nullptr;
    FileAttrWidget          *m_pFileAttrWidget = nullptr;
    DefaultOperationWidget  *m_pDefaultOperationWidget = nullptr;
    TextOperationWidget     *m_pTextOperationWidget = nullptr;
    DocummentProxy          *m_pDocummentProxy = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};


#endif // FILEVIEWWIDGET_H
