#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include <DWidget>
#include <DFloatingWidget>
#include <DIconButton>

DWIDGET_USE_NAMESPACE

class DocSheet;
class PlayControlWidget : public DFloatingWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(PlayControlWidget)
public:
    explicit PlayControlWidget(DocSheet *sheet, DWidget *parnet = nullptr);
    ~PlayControlWidget()override;

signals:
    void sigPlayBtnClicked();

public:
    void activeshow(int ix = 0, int iy = 0);
    void killshow();
    void setCanShow(bool bshow);
    bool canShow();
    void PageChangeByKey(const QString &);

protected:
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;

private slots:
    void slotPreClicked();
    void slotPlayClicked();
    void slotNextClicked();
    void slotExitClicked();

private:
    void initWidget();
    void initConnections();
    DIconButton *createBtn(const QString &strname = QString());
    void pagejump(const bool &bpre);
    void changePlayStatus();

private:
    QTimer *m_ptimer = nullptr;
    DIconButton *m_pbtnpre = nullptr;
    DIconButton *m_pbtnplay = nullptr;
    DIconButton *m_pbtnnext = nullptr;
    DIconButton *m_pbtnexit = nullptr;
    bool m_bcanshow = false;
    bool m_bautoplaying = false;
    bool m_bfirstshow = false;
    DocSheet *m_sheet;
};

#endif // PLAYCONTROLWIDGET_H
