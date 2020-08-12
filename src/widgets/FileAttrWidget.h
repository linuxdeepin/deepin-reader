#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DAbstractDialog>
#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE
class DocSheet;
class QVBoxLayout;
class ImageWidget;
class FileAttrWidget : public DAbstractDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(FileAttrWidget)

public:
    explicit FileAttrWidget(DWidget *parent = nullptr);

    ~FileAttrWidget() override;

    void setFileAttr(DocSheet *sheet);

    void showScreenCenter();

private:
    void initWidget();

    void initCloseBtn();

    void initImageLabel();

    void addTitleFrame(const QString &);

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;
    ImageWidget *frameImage = nullptr;
};

#endif  // FILEATTRWIDGET_H
