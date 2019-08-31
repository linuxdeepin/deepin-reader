#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DWidget>
#include <DPushButton>
#include <QHBoxLayout>

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainOperationWidget class
 * @brief   缩略图、书签、注释 操作widget
 */


class MainOperationWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    MainOperationWidget(DWidget* parent = nullptr);
    ~MainOperationWidget() override;

signals:
    void showType(const int&) const;

private:
    void initBtns();
    void createBtn(const QString& iconName, const char* member, bool checkable = false, bool checked = false);

private slots:
    void on_thumbnailBtn_clicked(bool);
    void on_bookmarksBtn_clicked(bool);
    void on_annotationBtn_clicked(bool);

private:
     QHBoxLayout    *m_hboxLayout = nullptr;
     MsgSubject     *m_pMsgSubject = nullptr;

     // IObserver interface
public:
     int update(const int&, const QString &) override;
};

#endif // MAINOPERATIONWIDGET_H
