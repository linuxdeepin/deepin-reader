#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QGraphicsView>
#include <QFileInfo>

class ModelAnnotation;//
class ModelPage;
class ModelDocument;


class DocumentView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DocumentView(QWidget* parent = 0);
    ~DocumentView();

public slots:
    bool open(const QString& filePath);
private:
    Q_DISABLE_COPY(DocumentView)

    void prepareDocument(ModelDocument* document, const QVector< ModelPage* >& pages);

    bool checkDocument(const QString& filePath, ModelDocument* document, QVector< ModelPage* >& pages);
     void saveLeftAndTop(qreal& left, qreal& top) const;
private:
    ModelDocument* m_document;
    QFileInfo m_fileInfo;
    bool m_wasModified;
    int m_currentPage;
    int m_firstPage;
};

#endif // DOCUMENTVIEW_H
