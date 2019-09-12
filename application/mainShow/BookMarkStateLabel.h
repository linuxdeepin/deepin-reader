#ifndef BOOKMARKSTATELABEL_H
#define BOOKMARKSTATELABEL_H

#include <DLabel>
#include <DWidget>
/**
 * @brief The BookMarkStateWidget class
 * @brief   主面板， 当前页的 书签状态
 *
 *
 */

DWIDGET_USE_NAMESPACE

class BookMarkStateLabel : public DLabel
{
    Q_OBJECT
public:
    BookMarkStateLabel(DWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    void leaveEvent(QEvent *event) override;

private:
    void setPixmapState(const QString &);

private slots:
    void SlotSetMarkState(const bool &);

private:
    bool    m_bChecked = false;
};

#endif // BOOKMARKSTATEWIDGT_H
