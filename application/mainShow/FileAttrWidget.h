#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DLabel>
#include <DTextEdit>
#include <DWidget>
#include <DWidgetUtil>
#include <QGridLayout>
#include <QVBoxLayout>

#include <DDialog>
#include "utils/utils.h"
#include "CustomControl/ImageFrame.h"
#include "CustomControl/ImageLabel.h"
#include "subjectObserver/IObserver.h"

#include "controller/NotifySubject.h"

DWIDGET_USE_NAMESPACE
/**
 * @brief The FileAttrWidget class
 * @brief   文件属性框
 */

class FileAttrWidget : public DAbstractDialog, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(FileAttrWidget)

public:
    explicit FileAttrWidget(DWidget *parent = nullptr);
    ~FileAttrWidget() Q_DECL_OVERRIDE;
    void showScreenCenter();

public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;

private:
    void initWidget();
    void setFileAttr();
    void initCloseBtn();
    void initImageLabel();
    void addTitleFrame(const QString &);

private slots:
    void slotBtnCloseClicked();

private:
    QVBoxLayout     *m_pVBoxLayout = nullptr;
    SubjectThread   *m_pNotifySubject = nullptr;

    //    DLabel *labelImage = nullptr;
    /*ImageFrame*/ ImageLabel *frameImage = nullptr;
};

#endif  // FILEATTRWIDGET_H
