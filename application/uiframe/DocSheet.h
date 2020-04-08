#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include <DSplitter>
#include <QMap>

#include "FileDataModel.h"
#include "docview/commonstruct.h"

DWIDGET_USE_NAMESPACE

class SpinnerWidget;
class SheetBrowserPDF;
class SheetSidebar;
class DocummentProxy;
class SheetBrowserArea;
class DocSheet : public DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheet)

public:
    explicit DocSheet(DocType_EM type, DWidget *parent = nullptr);

    ~DocSheet() override;

public:
    void openFile(const QString &filePath);

signals:
    void sigOpenFileResult(const QString &, const bool &);

    void sigFileChanged(bool hasChanged);     //被修改了 书签 笔记

    void sigTotateChanged();

    void sigOpened(DocSheet *, bool);

public:
    void pageJump(const int &pagenum);

    void pageJumpByMsg(const int &iType, const QString &param);

    void setDoubleShow(bool isShow);

    void setRotateLeft();

    void setRotateRight();

    void setFileChanged(bool hasChanged);

    void setMouseDefault();     //默认工具

    void setMouseHand();        //手型工具

    bool isMouseHand();

    bool isDoubleShow();

    QString qGetPath();

    void qSetPath(const QString &strPath);

    int qGetFileChange();

    void saveData();

    void setData(const int &, const QString &);

    FileDataModel qGetFileData();

    DocummentProxy *getDocProxy();

    void OnOpenSliderShow();

    void OnExitSliderShow();

    void ShowFindWidget();

    DocType_EM type();

    void showTips(const QString &tips);

    void setCurrentState(int state);

    int getCurrentState();

    void openSlide();

    void showControl();

    void quitSlide();

    void openMagnifer();

    void quitMagnifer();

//    bool close();

//    bool save();

//    bool saveAs(QString filePath);

    void handleOpenSuccess();

    void setSidebarVisible(bool isVisible);

private:
    void initPDF();

private slots:
    void SlotFileOpenResult(const QString &, const bool &);

    void SlotNotifyMsg(const int &, const QString &);

    void onShowTips(const QString &tips);

private:
    DocType_EM      m_type;
    void *m_sidebar = nullptr;
    void *m_browser = nullptr;

    QStackedWidget *m_pRightWidget = nullptr;
    SpinnerWidget  *m_pSpinnerWidget = nullptr;
    bool            m_bOldState = false;
    int             m_currentState;
    QString         m_uuid;

public:
    static QUuid     getUuid(DocSheet *);
    static DocSheet *getSheet(QString uuid);
    static QMap<QString, DocSheet *> g_map;
};

#endif // MAINSPLITTER_H
