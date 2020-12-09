#ifndef DOCTHREAD_H
#define DOCTHREAD_H

#include <QThread>
#include <QStack>
#include <QMutex>

#include "Model.h"

class DocSheet;
struct DocOpenThreadTask {
    DocSheet *sheet;
};

class Document;
class DocThread : public QThread
{
    Q_OBJECT
public:
    explicit DocThread(QObject *parent = nullptr);

    ~DocThread();

    static void appendTask(DocOpenThreadTask task);

    static void destroyForever();

    void run();

signals:
    void sigTaskFinished(DocSheet *sheet, bool result, QString error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages);

public slots:
    void onTaskFinished(DocSheet *sheet, bool result, QString error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages);

private:
    QStack<DocOpenThreadTask> m_tasks;

    QMutex m_mutex;

    bool m_quit = false;

    static bool s_quitForever;

    static DocThread *s_instance;

    static DocThread *instance();
};

#endif // DOCTHREAD_H
