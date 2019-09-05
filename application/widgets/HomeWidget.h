#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QPushButton>
#include <DLabel>
#include <DObject>
#include <QFileDialog>
#include <QSettings>
#include <QVBoxLayout>

#include "header/CustomWidget.h"

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
    void fileSelected(const QStringList files) const;

private slots:
    void onChooseBtnClicked();

private:
    QStringList getOpenFileList();

private:
    QVBoxLayout *m_layout = nullptr;
    QSettings *m_settings = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // OPENFILEWIDGET_H
