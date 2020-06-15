#ifndef SEARCHTASK_H
#define SEARCHTASK_H

#include <QThread>
#include "PageBase.h"
#include "CommonStruct.h"


class SearchTask : public QThread
{
    Q_OBJECT
public:
    explicit SearchTask(QObject *parent = nullptr);

    bool wasCanceled() const { return loadCancellation() != NotCanceled; }

    const QString &text() const { return m_text; }

    bool matchCase() const { return m_matchCase; }

    bool wholeWords() const { return m_wholeWords; }

    void run();

signals:
    void progressChanged(int progress);

    void signal_resultReady(stSearchRes stres);

    void signal_restest();

public slots:
    void start(const QVector< PageBase * > &pages,
               const QString &text, bool matchCase = false, bool wholeWords = false,
               int beginAtPage = 1);

    void cancel() { setCancellation(); }

private:
    Q_DISABLE_COPY(SearchTask)

    QAtomicInt m_wasCanceled;
    mutable QAtomicInt m_progress;

    enum {
        NotCanceled = 0,
        Canceled = 1
    };

    void setCancellation() { m_wasCanceled.storeRelease(Canceled);}
    void resetCancellation() { m_wasCanceled.storeRelease(NotCanceled);}
    bool testCancellation() {return m_wasCanceled.load() != NotCanceled;}
    int loadCancellation() const { return m_wasCanceled.load();}

//    void releaseProgress(int value);
//    int acquireProgress() const;
//    int loadProgress() const;

    template< typename Future >
    void processResults(Future future);

    QVector< PageBase * > m_pages;

    QString m_text;
    bool m_matchCase;
    bool m_wholeWords;
    int m_beginAtPage;
    bool m_parallelExecution;

};

#endif // SEARCHTASK_H
