#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <DWidget>
#include <DWidgetUtil>

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
    void initCloseBtn();
    void initImageLabel();

private slots:
    void slotBtnCloseClicked();

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;

    DLabel *labelImage = nullptr;
    DLabel *labelFileName = nullptr;
};

#endif // FILEATTRWIDGET_H
