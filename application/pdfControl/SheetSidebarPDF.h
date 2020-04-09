#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include <QResizeEvent>

#include "CustomControl/CustomWidget.h"

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

signals:
    void sigAnntationMsg(const int &, const QString &);
    void sigBookMarkMsg(const int &, const QString &);
    void sigDeleteAnntation(const int &, const QString &);
    void sigAdaptWindowSize(const double &scale); //缩略图列表自适应窗体大小  add by duanxiaohui 2020-3-19
    void sigUpdateThumbnail(const int &page);

    // IObserver interface
public:
    void handleOpenSuccess();

    int dealWithData(const int &, const QString &) override;

public slots:
    void onSearch(const int &);

    void onRotate(int);

    void onPageChanged(int page);

    // CustomWidget interface
protected:
    void initWidget() override;

    // QWidget interface
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
