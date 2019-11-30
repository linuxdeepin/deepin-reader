#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DLabel>
#include <DTextEdit>
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
    Q_DISABLE_COPY(FileAttrWidget)

public:
    explicit FileAttrWidget(DWidget *parent = nullptr);

public:
    void showScreenCenter();

private:
    void initWidget();
    void setFileAttr();
    void initCloseBtn();
    void initImageLabel();
    void addTitleFrame(const QString &);

private slots:
    void slotBtnCloseClicked();

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;

    DLabel *labelImage = nullptr;
};

#endif // FILEATTRWIDGET_H
