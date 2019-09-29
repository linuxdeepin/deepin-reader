#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QList>
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
    void showScreenCenter();

protected:
    //拖拽窗口
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void setFileAttr();
    void initLabels();
    void initCloseBtn();
    void initImageLabel();
    DLabel *createLabel(QGridLayout *, const int &, const QString &);

    void setFileInfoTime(const QDateTime &, const QDateTime &);

private slots:
    void slotBtnCloseClicked();

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;

    bool        m_bClickDown = false;
    QPoint      mouseStartPoint;
    QPoint      windowTopLeftPoint;

    DLabel *labelImage = nullptr;
    DLabel *labelFileName = nullptr;

    DLabel *labelFileTitle = nullptr;
    DLabel *labelFilePath = nullptr;
    DLabel *labelTheme = nullptr;
    DLabel *labelAuthor = nullptr;
    DLabel *labelKeyWord = nullptr;
    DLabel *labelCreator = nullptr;
    DLabel *labelProducer = nullptr;
    DLabel *labelCreateTime = nullptr;
    DLabel *labelUpdateTime = nullptr;
    DLabel *labelFormat = nullptr;
    DLabel *labelPageNumber = nullptr;
    DLabel *labelBetter = nullptr;
    DLabel *labelSafe = nullptr;
    DLabel *labelPaperSize = nullptr;
    DLabel *labelSize = nullptr;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // FILEATTRWIDGET_H
