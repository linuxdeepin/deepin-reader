#ifndef NOTETIPWIDGET_H
#define NOTETIPWIDGET_H

#include <DWidget>

DWIDGET_USE_NAMESPACE

/**
 * @brief The NoteTipWidget class
 *  显示注释内容
 */
class QTextEdit;
class NoteTipWidget : public DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NoteTipWidget)

public:
    explicit NoteTipWidget(DWidget *parnet = nullptr);

public:
    void setTipContent(const QString &);

private:
    void initWidget();

private slots:
    void slotUpdateTheme();

private:
    int m_iwidth = -1;

    QTextEdit       *pedit = nullptr;
};


#endif // NOTETIPWIDGET_H
