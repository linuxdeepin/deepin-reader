#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include <QResizeEvent>

#include "CustomControl/CustomWidget.h"
#include "docview/commonstruct.h"

/**
 * @brief The LeftShowWidget class
 * @brief   侧边栏显示
 */

class MainOperationWidget;
class DataStackedWidget;
class DocSheet;
class SheetSidebarPDF : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SheetSidebarPDF)

public:
    explicit SheetSidebarPDF(DocSheet *parent = nullptr);

    ~SheetSidebarPDF() override;

    void setBookMark(int page, int state);

    void handleOpenSuccess();

    void handleFindOperation(int);

    void handleFindContentComming(const stSearchRes &);

    int  handleFindFinished();      //返回搜索结果条数

    void handleRotate(int);

    void handleUpdateThumbnail(const int &page);

    void handleAnntationMsg(const int &, const QString &);

signals:
    void sigAnntationMsg(const int &, const QString &);

    void sigDeleteAnntation(const int &, const QString &);

    void sigAdaptWindowSize(const double &scale); //缩略图列表自适应窗体大小  add by duanxiaohui 2020-3-19

public slots:
    void onPageChanged(int page);

protected:
    void initWidget() override;

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotUpdateTheme();

private:
    void initConnections();
    void onSetWidgetVisible(const int &);

private:
    MainOperationWidget     *m_pMainOperationWidget = nullptr;
    DataStackedWidget       *m_pStackedWidget = nullptr;

    int         m_nSearch = -1;
    bool        m_bOldVisible = false;
    QString     m_strBindPath = "";
    DocSheet   *m_sheet;
};

#endif  // LEFTSHOWWIDGET_H
