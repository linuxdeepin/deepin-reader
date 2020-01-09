#ifndef FONTMENU_H
#define FONTMENU_H

#include <DMenu>
#include <DLabel>
#include <DSlider>

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QtMath>

#include "CustomControl/CustomClickLabel.h"
#include "docview/commonstruct.h"
#include "subjectObserver/IObserver.h"
#include "controller/NotifySubject.h"
#include "subjectObserver/ModuleHeader.h"
#include "subjectObserver/MsgHeader.h"

DWIDGET_USE_NAMESPACE

class FontMenu : public DMenu, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(FontMenu)

public:
    explicit FontMenu(QWidget *parent = nullptr);
    ~FontMenu() Q_DECL_OVERRIDE;

signals:
    // 准备弃用
//    void sigWidgetHide();
//    void sigDealWithKey(const QString &);
//    void sigKeyLargerOrSmaller(const int &);
//    void sigFiteH(QString);
//    void sigFiteW(QString);
//    void sigRotate(const QString &);

    //新增
    void sigFileOpenOk();
    void sigDealWithShortKey(const QString &);
    void sigSetCurScale(const QString &);

public:
//    void setScaleVal(const int &scale);
//    void setDefaultValOpenFileOk();
//    void dealKeyLargerOrSmaller(const QString &keyType);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &) Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &) Q_DECL_OVERRIDE;

protected:
    SubjectThread   *m_pNotifySubject = nullptr;

private slots:
    void slotTwoPage();
    void slotFiteH();
    void slotFiteW();
    void slotRotateL();
    void slotRotateR();
    void slotScaleValChanged(int);

    //新增
    void slotFileOpenOk();
    void slotDealWithShortKey(const QString &);
    void slotSetCurScale(const QString &);

private:
    void initMenu();
    void initScale();
    void initConnection();
    QAction *createAction(const QString &objName, const char *, bool checked = false);
    void rotateThumbnail(bool);
    void scaleAndRotate();
    void calcRotateType();
    void setScaleRotateViewModeAndShow();
    void setAppSetFiteHAndW();
    void resetFiteHAndW();
    void resetAdaptive();
    void setFileViewScale(bool);
    void setSliderMaxValue();

private:
    QStringList     shortKeyList;                 // 要处理的消息类型
    DLabel *m_pEnlargeLab = nullptr;              // 缩放比例label
    DSlider *m_pEnlargeSlider = nullptr;          // 缩放比例slider
    QAction *m_pTwoPageAction = nullptr;          // 双页
    QAction *m_pFiteHAction = nullptr;            // 自适应高
    QAction *m_pFiteWAction = nullptr;            // 自适应宽
    QAction *m_pRotateLAction = nullptr;          // 左旋转
    QAction *m_pRotateRAction = nullptr;          // 右旋转
    int m_nScale = 100;                           // 缩放比例(缺省100%)
    bool m_bDoubPage = false;                     // 双页
    bool m_bFiteH = false;                        // 自适应高
    bool m_bFiteW = false;                        // 自适应宽
    int m_nRotate = 0;                            // 旋转角度(0~360度)
    RotateType_EM m_rotateType = RotateType_0;    // 旋转类型(后台所需旋转类型)
    bool m_bIsAdaptMove = false;                  // 在自适应宽高时，缩放变化不响应
};

#endif // FONTMENU_H
