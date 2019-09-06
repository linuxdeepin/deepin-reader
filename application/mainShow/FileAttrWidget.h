#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QList>
#include <DPushButton>
#include <QMouseEvent>
#include "subjectObserver/CustomWidget.h"


/**
 * @brief The FileAttrWidget class
 * @brief   文件属性框
 */

class FileAttrWidget : public CustomWidget
{
    Q_OBJECT
public:
    FileAttrWidget(CustomWidget *parent = nullptr);

public:
    void setFileAttr();
    void showScreenCenter();

protected:
    //拖拽窗口
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void initLabels();
    void initCloseBtn();
    void initImageLabel();
    void createLabel(QGridLayout *, const int &, const QString &);

private slots:
    void slotBtnCloseClicked();

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;
    QList<DLabel *> m_labelList;

    bool m_bClickDown = false;
    QPoint      mouseStartPoint;
    QPoint      windowTopLeftPoint;
    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // FILEATTRWIDGET_H
