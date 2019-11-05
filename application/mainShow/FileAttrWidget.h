#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <DWidget>
#include <DWidgetUtil>

#include <DIconButton>
#include <QFileInfo>
#include <DFrame>

#include "utils/utils.h"

DWIDGET_USE_NAMESPACE
/**
 * @brief The FileAttrWidget class
 * @brief   文件属性框
 */

class FileAttrWidget : public DAbstractDialog
{
    Q_OBJECT
public:
    FileAttrWidget(DWidget *parent = nullptr);

public:
    void showScreenCenter();

private:
    void initWidget();
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
};

#endif // FILEATTRWIDGET_H
