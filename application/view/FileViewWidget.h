#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <QMouseEvent>
#include <QAction>
#include <DMenu>
#include "view/MagnifyingWidget.h"
#include "header/CustomWidget.h"
#include "view/DefaultOperationWidget.h"
#include "view/TextOperationWidget.h"

class DocumentView;
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

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;

public slots:
     void on_slot_openfile(const QString& filePath);
private slots:
    void SlotCustomContextMenuRequested(const QPoint &);

private:
    int openFilePath(const QString &);
    int magnifying(const QString &);
    int setHandShape(const QString &);
    int screening(const QString &);

private:
    MagnifyingWidget *m_pMagnifyingWidget = nullptr;
    bool m_bCanVisible = false; //  放大镜 是否可以显示

    DefaultOperationWidget  *m_pDefaultOperationWidget = nullptr;
    TextOperationWidget     *m_pTextOperationWidget = nullptr;
     DocumentView* m_docview;
    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // FILEVIEWWIDGET_H
