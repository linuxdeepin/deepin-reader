#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "CustomControl/CustomWidget.h"

#include <DSpinner>

class QStackedLayout;

class FindWidget;
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
    void sigFindOperation(const int &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    bool OpenFilePath(const QString &);
    void ShowFindWidget();

    // CustomWidget interface
protected:
    void initWidget() override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void slotUpdateTheme();
    void SlotOpenFileOk();

    void SlotFindOperation(const int &, const QString &);

private:
    void initConnections();
    int dealWithNotifyMsg(const QString &);
    void onOpenNoteWidget(const QString &);
    void onShowNoteWidget(const QString &);

    void __ShowPageNoteWidget(const QString &);
    void InitSpinner();

private:
    FileViewWidget      *m_pFileViewWidget = nullptr;
    FindWidget          *m_pFindWidget = nullptr;
    NoteViewWidget      *m_pNoteViewWidget = nullptr;

    DSpinner            *m_pSpiner = nullptr;
    DWidget             *m_pSpinerWidget = nullptr;
    QStackedLayout      *m_playout = nullptr;
    QString             m_strBindPath = "";
};

#endif // DOCSHOWSHELLWIDGET_H
