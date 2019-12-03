#ifndef DOCUMMENTPDF_H
#define DOCUMMENTPDF_H

#include "../docummentbase.h"
#include <poppler-qt5.h>
#include <QFileInfo>

class DocummentPDFPrivate;
class DocummentPDF: public DocummentBase
{
    Q_OBJECT
public:
    DocummentPDF(DWidget *parent = nullptr);
    ~DocummentPDF() override;
    bool bDocummentExist() override;
//    bool getImage(int pagenum, QImage &image, double width, double height) override;
//    void docBasicInfo(stFileInfo &info) override;
    bool save(const QString &filePath, bool withChanges) override;
    bool saveas(const QString &filePath, bool withChanges) override;
    void removeAllAnnotation();
    QString removeAnnotation(const QPoint &startpos) override;
    void removeAnnotation(const QString &struuid, int ipage = -1) override;
    QString addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color = Qt::yellow) override;
    void getAllAnnotation(QList<stHighlightContent> &listres) override;
    void search(const QString &strtext, QColor color = Qt::yellow) override;
    void clearSearch() override;
    bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid) override;
    void title(QString &title) override;
    void setAnnotationText(int ipage, const QString &struuid, const QString &strtext) override;
    void getAnnotationText(const QString &struuid, QString &strtext, int ipage)override;
    bool loadDocumment(QString filepath) override;
    void jumpToHighLight(const QString &uuid, int ipage) override;
    void changeAnnotationColor(int ipage, const QString uuid, const QColor &color) override;
    bool freshFile() override;

private:
    bool pdfsave(const QString &filePath, bool withChanges);
    void refreshOnePage(int ipage);
private:
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentPDF)
};
#endif // DOCUMMENTPDF_H
