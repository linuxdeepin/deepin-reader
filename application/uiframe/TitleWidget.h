#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DPushButton>
#include <DIconButton>

#include "CustomControl/CustomWidget.h"
#include "docview/commonstruct.h"

//class ScaleMenu;
class FontMenu;
class HandleMenu;

class ScaleWidget;

/**
 * @brief The TitleWidget class
 * @brief   标题栏的 按钮操作
 */

class TitleWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TitleWidget)

public:
    explicit TitleWidget(DWidget *parent = nullptr);
    ~TitleWidget() override;

private:
    static TitleWidget *g_onlyTitleWdiget;

public:
    static TitleWidget *Instance();

    void suitDocType(DocType_EM type);     //根据不同文档展示不同的菜单项

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    void setMagnifierState();

    // CustomWidget interface
public:
    int qDealWithShortKey(const QString &) override;

protected:
    void initWidget() override;

private slots:
    void slotUpdateTheme();
    void slotOpenFileOk(const QString &);

    void on_thumbnailBtn_clicked();
    void on_settingBtn_clicked();
    void on_handleShapeBtn_clicked();
    void on_searchBtn_clicked();

    void SlotSetCurrentTool(const int &);

    void SlotScaleChanged();

private:
    void initConnections();
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

    void SetFindWidget(const int &, const QString &);

private:
    QStringList shortKeyList;

    HandleMenu *m_pHandleMenu = nullptr;
    FontMenu *m_pFontMenu = nullptr;

    DPushButton *m_pThumbnailBtn = nullptr;
    DPushButton *m_pSettingBtn = nullptr;
    DPushButton *m_pHandleShapeBtn = nullptr;

    ScaleWidget     *m_pSw = nullptr;

    DIconButton     *m_pSearchBtn = nullptr;
};

#endif  // TITLEWIDGET_H
