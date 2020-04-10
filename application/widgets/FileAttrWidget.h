#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DAbstractDialog>
#include <DWidget>
DWIDGET_USE_NAMESPACE
class DocSheet;
class ImageLabel;
class QVBoxLayout;
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
    ImageLabel  *frameImage = nullptr;
};

#endif  // FILEATTRWIDGET_H
