#ifndef DEFAULTOPERATIONMENU_H
#define DEFAULTOPERATIONMENU_H

#include "widgets/CustomMenu.h"
#include <QPointer>

class DocSheetPDF;
class DefaultOperationMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(DefaultOperationMenu)

public:
    explicit DefaultOperationMenu(DWidget *parent = nullptr);
    ~DefaultOperationMenu() override;

public:
    void execMenu(DocSheetPDF *sheet, const QPoint &, const int &);
    void setClickpoint(const QPoint &pt);

protected:
    void initActions() override;

private:
    QAction *createAction(const QString &, const char *member);

private slots:
    void slotSearchClicked();

    void slotAddIconNote();
    void slotBookMarkClicked();

    void slotFirstPageClicked();
    void slotPrevPageClicked();
    void slotNextPageClicked();
    void slotEndPageClicked();

    void slotFullScreenClicked();
    void slotSlideShowClicked();

    void slotRotateLeftClicked();
    void slotRotateRightClicked();

    void slotPrintClicked();
    void slotDocInfoClicked();

private:
    int      m_nRightPageNumber = -1;

    QAction *m_pSearch      = nullptr;
    QAction *m_pBookMark    = nullptr;
    QAction *m_pAddIconNote = nullptr;
    QAction *m_pFirstPage   = nullptr;
    QAction *m_pPrevPage    = nullptr;
    QAction *m_pNextPage    = nullptr;
    QAction *m_pEndPage     = nullptr;

    QPoint m_pointclicked;
    QPoint m_showPoint;

    QPointer<DocSheetPDF> m_sheet;
};

#endif // DEFAULTOPERATIONMENU_H
