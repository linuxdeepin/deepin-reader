#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DMenu>
#include <DIconButton>

#include "subjectObserver/CustomWidget.h"
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
    void sigUpdateTheme(const QString &);
    void sigOpenFileOk();
    void sigAppFullScreen();

    void sigMagnifierCancel();

private slots:
    void slotUpdateTheme(const QString &);
    void slotOpenFileOk();
    void slotAppFullScreen();
    void slotMagnifierCancel();

    void on_thumbnailBtn_clicked();
    void on_settingBtn_clicked();
    void on_handleShapeBtn_clicked();
    void on_magnifyingBtn_clicked();

    void slotActionTrigger(QAction *);

private:
    void initConnections();

    void initBtns();
    DIconButton *createBtn(const QString &btnName, bool bCheckable = false);
    QAction *createAction(const QString &iconName);
    void sendMsgToSubject(const int &, const QString &msgCotent = "");

    void setHandleShapeBtn(const QString &);

private:
    DMenu   *m_pHandleMenu = nullptr;
    DMenu   *m_pSettingMenu = nullptr;

    DIconButton *m_pThumbnailBtn = nullptr;
    DIconButton *m_pSettingBtn = nullptr;
    DIconButton *m_pHandleShapeBtn = nullptr;
    DIconButton *m_pMagnifierBtn = nullptr;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // TITLEWIDGET_H
