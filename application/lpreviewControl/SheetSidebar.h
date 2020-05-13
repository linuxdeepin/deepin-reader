#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include <QResizeEvent>
#include <DToolButton>
#include <QStackedLayout>

#include "CustomControl/CustomWidget.h"
#include "pdfControl/docview/commonstruct.h"

enum PreviewWidgesFlag {
    PREVIEW_THUMBNAIL = 0x0001,
    PREVIEW_CATALOG   = 0x0002,
    PREVIEW_BOOKMARK  = 0x0004,
    PREVIEW_NOTE      = 0x0008,
    PREVIEW_SEARCH    = 0x0010
};
Q_DECLARE_FLAGS(PreviewWidgesFlags, PreviewWidgesFlag);
Q_DECLARE_OPERATORS_FOR_FLAGS(PreviewWidgesFlags)

class DocSheet;
class ThumbnailWidget;
class CatalogWidget;
class BookMarkWidget;
class NotesWidget;
class SearchResWidget;
class QButtonGroup;
class SheetSidebar : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SheetSidebar)

signals:
    void sigDeleteAnntation(const int &, const QString &);
    void sigAdaptWindowSize(const double &scale);

public :
    explicit SheetSidebar(DocSheet *parent = nullptr, PreviewWidgesFlags widgesFlag = PREVIEW_THUMBNAIL | PREVIEW_CATALOG | PREVIEW_BOOKMARK | PREVIEW_NOTE);
    ~SheetSidebar() override;

    void setBookMark(int page, int state);
    void handleLeftPressKey();
    void handleRightPressKey();
    void handleOpenSuccess();
    void handleFindOperation(int);
    void handleFindContentComming(const stSearchRes &);
    int  handleFindFinished();
    void handleRotate(int);
    void handleUpdateThumbnail(const int &page);
    void handleAnntationMsg(const int &, const QString &);

public slots:
    void onPageChanged(int page);

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private slots:
    void onBtnClicked(int index);
    void onUpdateWidgetTheme();

private:
    void initConnections();
    DToolButton *createBtn(const QString &btnName, const QString &objName);
    void dealWithPressKey(const QString &sKey);
    void onJumpToPrevPage();
    void onJumpToNextPage();
    void deleteItemByKey();

private:
    qreal m_scale;
    int  m_searchId;
    bool m_bOldVisible;

    DocSheet *m_sheet;
    QStackedLayout *m_stackLayout;
    QButtonGroup *m_btnGroup;
    PreviewWidgesFlags m_widgetsFlag;

    ThumbnailWidget *m_thumbnailWidget;
    CatalogWidget   *m_catalogWidget;
    BookMarkWidget  *m_bookmarkWidget;
    NotesWidget     *m_notesWidget;
    SearchResWidget *m_searchWidget;
};
#endif  // LEFTSHOWWIDGET_H
