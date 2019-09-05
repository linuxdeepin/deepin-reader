#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include "header/CustomWidget.h"
#include <DWidget>
#include <QMouseEvent>


#include "view/MagnifyingWidget.h"

DWIDGET_USE_NAMESPACE
class QMainWindow;
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

public slots:
    void on_slot_openfile(const QString& filePath);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void initUI();
    void initConnections();

private:
    MagnifyingWidget *m_pMagnifyingWidget = nullptr;
    bool m_bCanVisible = false; //  放大镜 是否可以显示
    QMainWindow* m_viewwindow;
    DocumentView* m_docview;
    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // FILEVIEWWIDGET_H
