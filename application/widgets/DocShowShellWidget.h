#ifndef DOCSHOWSHELLWIDGET_H
#define DOCSHOWSHELLWIDGET_H

#include "CustomControl/CustomWidget.h"

#include <DSpinner>

class QStackedLayout;

class FileViewWidget;
class NoteViewWidget;
class SpinnerWidget;

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

    void setFileChange(bool bchange);
    bool getFileChange();

    // CustomWidget interface
protected:
    void initWidget() override;

private slots:
    void SlotOpenFileOk();

    void SlotFindOperation(const int &, const QString &);

private:
    void onOpenNoteWidget(const QString &);
    void onShowNoteWidget(const QString &);

    void __ShowPageNoteWidget(const QString &);

private:
    FileViewWidget      *m_pFileViewWidget = nullptr;
    NoteViewWidget      *m_pNoteViewWidget = nullptr;

    SpinnerWidget            *m_pSpiner = nullptr;
    QStackedLayout      *m_playout = nullptr;
    QString             m_strBindPath = "";
};

#endif // DOCSHOWSHELLWIDGET_H
