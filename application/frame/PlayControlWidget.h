#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include "CustomControl/CustomWidget.h"
#include <DIconButton>

class PlayControlWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(PlayControlWidget)
public:
    explicit PlayControlWidget(CustomWidget *parnet = nullptr);
    ~PlayControlWidget()Q_DECL_OVERRIDE;
    // IObserver interface
public:
    int dealWithData(const int &, const QString &)Q_DECL_OVERRIDE;
    void activeshow();
    void killshow();
    // CustomWidget interface
protected:
    void initWidget()Q_DECL_OVERRIDE;
    void initConnections();
    DIconButton *createBtn(const QString &strobjname = QString());
    void showEvent(QShowEvent *event)Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *hideEvent)Q_DECL_OVERRIDE;

private:
    QTimer *m_ptimer;
    DIconButton *m_pbtnpre;
    DIconButton *m_pbtnplay;
    DIconButton *m_pbtnnext;
    DIconButton *m_pbtnexit;
private slots:
    void slotUpdateTheme();
    void slotPreClicked();
    void slotPlayClicked();
    void slotNextClicked();
    void slotExitClicked();
};

#endif // PLAYCONTROLWIDGET_H
