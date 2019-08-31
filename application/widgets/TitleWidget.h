#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DWidget>
#include <QHBoxLayout>
#include <DPushButton>
#include <DMenu>


#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE


/**
 * @brief The TitleWidget class
 * @brief   标题栏的 按钮操作
 */



class TitleWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    TitleWidget(DWidget *parent = nullptr);
    ~TitleWidget() override;

signals:
    void sendThumbnailState(const bool&) const;
    void sendHandShapeState(const bool&) const;
    void sendMagnifyingState(const bool&) const;

private slots:
     void on_thumbnailBtn_clicked(bool);
     void on_fontBtn_clicked();
     void on_handleShapeBtn_clicked();
     void on_magnifyingBtn_clicked(bool);

     void on_HandleAction_trigger();
     void on_DefaultAction_trigger();

private:
    void initBtns();
    void createBtn(const QString& iconName, const char* member, bool checkable = false, bool checked = false);
    void createAction(const QString& iconName, const char* member);

private:
    QHBoxLayout *m_layout = nullptr;

    bool         m_bCurrentState = false;
    QAction     *m_pHandleAction = nullptr;
    QAction     *m_pDefaultAction = nullptr;

    MsgSubject    *m_pMsgSubject = nullptr;

    // IObserver interface
public:
    int update(const int&, const QString &) override;
};

#endif // TITLEWIDGET_H
