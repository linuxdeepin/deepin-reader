#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DWidget>
#include <QGridLayout>
#include <DLabel>
#include <QList>

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The FileAttrWidget class
 * @brief   文件属性框
 */



class FileAttrWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    FileAttrWidget(DWidget *parent = nullptr);
    ~FileAttrWidget() override;

public:
    void setFileAttr();

private:
    void initLabels();
    void createLabel(const int&, const QString&);

private:
    QGridLayout     *m_pGridLayout = nullptr;

    QList<DLabel*>  m_labelList;

    MsgSubject      *m_pMsgSubject = nullptr;

    // IObserver interface
public:
    int update(const int&, const QString &) override;
};

#endif // FILEATTRWIDGET_H
