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
    ~HomeWidget() Q_DECL_OVERRIDE {
        if (m_settings)
        {
            m_settings->deleteLater();
            m_settings = nullptr;
        }
    }

signals:
    void sigUpdateTheme(const QString &);
    void sigOpenFileDialog();

private slots:
    void slotChooseBtnClicked();
    void slotUpdateTheme(const QString &);

private:
    QStringList getOpenFileList();
    void initConnections();

private:
    QSettings *m_settings = nullptr;
    DLabel  *m_pIconLabel = nullptr;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // OPENFILEWIDGET_H
