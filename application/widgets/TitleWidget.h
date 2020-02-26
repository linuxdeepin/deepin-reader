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
    ~TitleWidget() Q_DECL_OVERRIDE;

signals:
    void sigSetFindWidget(const int &);
    void sigOpenFileOk(const QString &);
    void sigAppFullScreen();
    void sigMagnifierCancel();
    void sigTabMsg(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

private slots:
    void slotSetFindWidget(const int &);
    void slotUpdateTheme();
    void slotOpenFileOk(const QString &);
    void slotAppFullScreen();
    void slotMagnifierCancel();

    void on_thumbnailBtn_clicked();
    void on_settingBtn_clicked();
    void on_handleShapeBtn_clicked();
    void SlotScaleMenuBtnClicked();

    void SlotSetCurrentTool(const QString &);
    void SlotDealWithShortKey(const QString &);
    void SlotTabMsg(const QString &);

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

    int m_nCurHandleShape = -1;  //  当前的选择工具状态
};

#endif  // TITLEWIDGET_H
