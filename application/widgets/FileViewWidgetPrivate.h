#ifndef FILEVIEWWIDGETPRIVATE_H
#define FILEVIEWWIDGETPRIVATE_H

#include <QObject>

class FileViewWidget;
class FileViewWidgetPrivate: public QObject
{
    Q_OBJECT
public:
    FileViewWidgetPrivate(FileViewWidget *parent = nullptr);

public:
    void AddHighLight(const QString &msgContent);
    void AddHighLightAnnotation(const QString &msgContent);
    void RemoveHighLight(const QString &msgContent);
    void ChangeAnnotationColor(const QString &msgContent);
    void RemoveAnnotation(const QString &msgContent);
    void UpdateAnnotationText(const QString &msgContent);
    void AddPageIconAnnotation(const QString &msgContent);
    void DeletePageIconAnnotation(const QString &msgContent);
    void UpdatePageIconAnnotation(const QString &msgContent);

public:
    bool m_filechanged;
protected:
    FileViewWidget *q_ptr;
    Q_DECLARE_PUBLIC(FileViewWidget)
};

#endif // FILEVIEWWIDGETPRIVATE_H
