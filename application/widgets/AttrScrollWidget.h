#ifndef ATTRSCROLLWIDGET_H
#define ATTRSCROLLWIDGET_H

#include <DFrame>
#include <DWidget>
#include <DLabel>
#include <QGridLayout>

DWIDGET_USE_NAMESPACE

class DocSheet;
class AttrScrollWidget : public DFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(AttrScrollWidget)

public:
    explicit AttrScrollWidget(DocSheet *sheet, DWidget *parent  = nullptr);

private:
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QString &sData);
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QDateTime &sData);
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const bool &bData);

    void updateTheme();

private:
    DFrame *addTitleFrame(const QString &sData);
    int m_leftminwidth;
};

#endif // ATTRSCROLLWIDGET_H
