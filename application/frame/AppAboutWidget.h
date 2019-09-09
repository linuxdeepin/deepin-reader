#ifndef APPABOUTWIDGET_H
#define APPABOUTWIDGET_H

/**
 * @brief The AboutWidget class
 * @brief   关于
 */

#include "subjectObserver/CustomWidget.h"
#include <DLabel>
#include <QVBoxLayout>
#include <DPushButton>
#include "application.h"

class AppAboutWidget : public CustomWidget
{
    Q_OBJECT
public:
    AppAboutWidget(CustomWidget *parent = nullptr);

public:
    void showScreenCenter();

protected:
    //拖拽窗口
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void initCloseBtn();
    void initLabels();

private slots:
    void slotBtnCloseClicked();

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;
    DLabel *m_pVersionLabel = nullptr;


    bool m_bClickDown = false;
    QPoint      mouseStartPoint;
    QPoint      windowTopLeftPoint;
    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

};

#endif // ABOUTWIDGET_H
