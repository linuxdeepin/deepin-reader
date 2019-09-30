#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QSettings>

#include "subjectObserver/CustomWidget.h"

/**
 *  @brief  支持拖拽
 *  @brief  打开pdf 文件
 */

class HomeWidget : public CustomWidget
{
    Q_OBJECT
public:
    HomeWidget(CustomWidget *parent = nullptr);

signals:
    void sigOpenFileDialog();

private slots:
    void slotChooseBtnClicked();

private:
    QStringList getOpenFileList();

private:
    QSettings *m_settings = nullptr;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // OPENFILEWIDGET_H
