#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DPushButton>
#include <DIconButton>

#include "CustomControl/CustomWidget.h"

class ScaleMenu;
class FontMenu;
class HandleMenu;

/**
 * @brief The TitleWidget class
 * @brief   标题栏的 按钮操作
 */

class TitleWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TitleWidget)

public:
    explicit TitleWidget(CustomWidget *parent = nullptr);
    ~TitleWidget() override;

private:
    static TitleWidget *g_onlyTitleWdiget;

public:
    static TitleWidget *Instance();

signals:
    void sigSetFindWidget(const int &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomWidget interface
public:
    int qDealWithShortKey(const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;

private slots:
    void slotSetFindWidget(const int &);
    void slotUpdateTheme();
    void slotOpenFileOk(const QString &);

    void on_thumbnailBtn_clicked();
    void on_settingBtn_clicked();
    void on_handleShapeBtn_clicked();
    void SlotScaleMenuBtnClicked();

    void SlotSetCurrentTool(const QString &);

    void SlotCurrentScale(const int &);

private:
    void initConnections();
    void OnFileShowChange(const QString &);

    void initBtns();
    void __InitHandel();
    void __InitSelectTool();
    void __InitScale();

    void setDefaultShape();
    void setHandleShape();

    DPushButton *createBtn(const QString &btnName, bool bCheckable = false);

    void setMagnifierState();
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

    DIconButton     *m_pPreBtn = nullptr;
    DPushButton     *m_pScaleMenuBtn = nullptr;
    DIconButton     *m_pNextBtn = nullptr;
    ScaleMenu       *m_pScaleMenu = nullptr;
};

#endif  // TITLEWIDGET_H
