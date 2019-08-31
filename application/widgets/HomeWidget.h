#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <DObject>
#include <DWidget>
#include <QVBoxLayout>
#include <DLabel>
#include <QFileDialog>
#include <QSettings>
#include "dlinkbutton.h"

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

/**
 *  @brief  支持拖拽
 *  @brief  打开pdf 文件
 */


DWIDGET_USE_NAMESPACE

class HomeWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    HomeWidget(DWidget *parent = nullptr);
    ~HomeWidget() override;

    void setIconPixmap(bool isLoaded);

signals:
    void fileSelected(const QStringList files) const;

private slots:
    void onChooseBtnClicked();

private:
    QStringList getOpenFileList();

private:
    QVBoxLayout     *m_layout = nullptr;
    QPixmap         m_unloadPixmap;
    QPixmap         m_loadedPixmap;
    DLabel          *m_iconLabel = nullptr;
    DLabel          *m_tipsLabel = nullptr;
    DLabel          *m_splitLine = nullptr;
    DLinkButton     *m_chooseBtn = nullptr;
    QSettings       *m_settings = nullptr;

    MsgSubject      *m_pMsgSubject = nullptr;

    // IObserver interface
public:
    int update(const int&, const QString &) override;
};

#endif // OPENFILEWIDGET_H
