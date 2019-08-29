#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DWidget>
#include <QGridLayout>
#include <DLabel>
#include <QList>

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The FileAttrWidget class
 * @brief   文件属性框
 */



class FileAttrWidget : public DWidget, public IThemeObserver
{
public:
    FileAttrWidget(DWidget *parent = nullptr);
    ~FileAttrWidget();

public:
    void setFileAttr();

private:
    void initLabels();
    void createLabel(const int&, const QString&);

private:
    QGridLayout     *m_pGridLayout = nullptr;

    QList<DLabel*>  m_labelList;

    ThemeSubject    *m_pThemeSubject  = nullptr;

    // IObserver interface
public:
    int update(const QString &);
};

#endif // FILEATTRWIDGET_H
