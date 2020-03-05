#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "CustomControl/CustomWidget.h"

#include <DSpinner>

class QStackedLayout;

class FileViewWidget;
class NoteViewWidget;

/**
 * @brief The DocShowShellWidget class
 * @brief   文档　显示外壳
 */

class DocShowShellWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DocShowShellWidget)

public:
    explicit DocShowShellWidget(CustomWidget *parent = nullptr);
    ~DocShowShellWidget() override;

signals:
    void sigOpenFileOk();

    void sigShowCloseBtn(const int &);
    void sigHideCloseBtn();
    void sigChangePlayCtrlShow(bool bshow);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    bool OpenFilePath(const QString &);

private slots:
//    void slotShowCloseBtn(const int &);
//    void slotBtnCloseClicked();
    void slotUpdateTheme();
    void SlotOpenFileOk();

private:
    void initConnections();
    int dealWithNotifyMsg(const QString &);
    void onOpenNoteWidget(const QString &);
    void onShowNoteWidget(const QString &);

    void __ShowPageNoteWidget(const QString &);
    void InitSpinner();

    // CustomWidget interface
protected:
    void initWidget() override;

private:
    FileViewWidget      *m_pFileViewWidget = nullptr;
    NoteViewWidget      *m_pNoteViewWidget = nullptr;

    DSpinner            *m_pSpiner = nullptr;
    DWidget             *m_pSpinerWidget = nullptr;
    QStackedLayout      *m_playout = nullptr;
};

#endif // DOCSHOWSHELLWIDGET_H
