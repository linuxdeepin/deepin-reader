#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DMenu>
#include <DToolButton>

#include "CustomControl/CustomWidget.h"
#include "pdfControl/font/fontWidget.h"
#include "utils/PublicFunction.h"
/**
 * @brief The TitleWidget class
 * @brief   标题栏的 按钮操作
 */

class TitleWidget : public CustomWidget
{
    Q_OBJECT
public:
    TitleWidget(CustomWidget *parent = nullptr);
    ~TitleWidget() Q_DECL_OVERRIDE;

signals:
    void sigShowFindContent();
    void sigOpenFileOk();
    void sigAppFullScreen();
    void sigMagnifierCancel();
    void sigDealWithShortKey(const QString &);

private slots:
    void slotShowFindContent();
    void slotUpdateTheme();
    void slotOpenFileOk();
    void slotAppFullScreen();
    void slotMagnifierCancel();

    void on_thumbnailBtn_clicked();
    void on_settingBtn_clicked();
    void on_handleShapeBtn_clicked();
    void on_magnifyingBtn_clicked();

    void slotActionTrigger(QAction *);
    void slotDealWithShortKey(const QString &);

private:
    void initConnections();

    void initBtns();
    void initMenus();

    void setDefaultShape();
    void setHandleShape();

    DToolButton *createBtn(const QString &btnName, bool bCheckable = false);
    void sendMsgToSubject(const int &, const QString &msgCotent = "");
    void notifyMsgToSubject(const int &, const QString &msgCotent = "");

private:
    DMenu   *m_pHandleMenu = nullptr;
    DMenu   *m_pSettingMenu = nullptr;

    DToolButton *m_pThumbnailBtn = nullptr;
    DToolButton *m_pSettingBtn = nullptr;
    DToolButton *m_pHandleShapeBtn = nullptr;
    DToolButton *m_pMagnifierBtn = nullptr;

    int     m_nCurHandleShape = -1;     //  当前的选择工具状态
    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // TITLEWIDGET_H
