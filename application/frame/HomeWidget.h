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

protected:
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

private slots:
    void onChooseBtnClicked();

private:
    QStringList getOpenFileList();

private:
    QSettings *m_settings = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // OPENFILEWIDGET_H
