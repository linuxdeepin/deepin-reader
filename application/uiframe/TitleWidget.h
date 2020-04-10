#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DPushButton>
#include <DIconButton>
#include <QPointer>
#include "DocSheet.h"

#include "CustomControl/CustomWidget.h"
#include "docview/commonstruct.h"

//class ScaleMenu;
class FontMenu;
class HandleMenu;
class ScaleWidget;
class DocSheet;
class TitleWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TitleWidget)

public:
    explicit TitleWidget(DWidget *parent = nullptr);

    ~TitleWidget() override;

public:
    void setMagnifierState();

public slots:
    int onTitleShortCut(const QString &);

protected:
    void initWidget() override;

public slots:
    void onCurSheetChanged(DocSheet *);

private slots:
    void slotUpdateTheme();

    void slotOpenFileOk(const QString &);

    void SlotSetCurrentTool(const int &);

    void on_thumbnailBtn_clicked();

    void on_settingBtn_clicked();

    void on_handleShapeBtn_clicked();

    void on_searchBtn_clicked();

private:
    void OnFileShowChange(const QString &);

    void initBtns();

    void __InitHandel();

    void __InitSelectTool();

    void setDefaultShape();

    void setHandleShape();

    DPushButton *createBtn(const QString &btnName, bool bCheckable = false);

    void SetBtnDisable(const bool &bAble);

    void OnShortCut_Alt1();

    void OnShortCut_Alt2();

    void OnShortCut_CtrlM();

private:
    QStringList shortKeyList;

    HandleMenu *m_pHandleMenu = nullptr;
    FontMenu *m_pFontMenu = nullptr;

    DPushButton *m_pThumbnailBtn = nullptr;
    DPushButton *m_pSettingBtn = nullptr;
    DPushButton *m_pHandleShapeBtn = nullptr;

    ScaleWidget     *m_pSw = nullptr;
    DIconButton     *m_pSearchBtn = nullptr;

    QPointer<DocSheet> m_curSheet = nullptr;
};

#endif  // TITLEWIDGET_H
