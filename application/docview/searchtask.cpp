#include "searchtask.h"
#include <QtConcurrent>


struct Search {
    Search(const QString &text, const bool matchCase, const bool wholeWords) :
        text(text),
        matchCase(matchCase),
        wholeWords(wholeWords)
    {
    }

    const QString &text;
    const bool matchCase;
    const bool wholeWords;



    stSearchRes operator()(PageBase *const page) const
    {
        return page->search(text, matchCase, wholeWords);
    }
};

struct FutureWrapper {
    FutureWrapper(const QVector< PageBase * > &pages, const Search &search) :
        pages(pages),
        search(search)
    {
    }

    const QVector< PageBase * > &pages;
    const Search &search;

    void cancel()
    {
    }

    stSearchRes resultAt(int index)
    {
        return search(pages.at(index));
    }
};

SearchTask::SearchTask(QObject *parent) : QThread(parent),
    m_wasCanceled(NotCanceled),
    m_progress(0),
    m_pages(),
    m_text(),
    m_matchCase(false),
    m_wholeWords(false),
    m_beginAtPage(1),
    m_parallelExecution(false)
{

}

void SearchTask::run()
{
    QVector< PageBase * > pages;
    pages.reserve(m_pages.count());

    for (int index = 0, count = m_pages.count(); index < count; ++index) {
        const int shiftedIndex = (index + m_beginAtPage - 1) % count;        
        pages.append(m_pages.at(shiftedIndex));
    }

    const Search search(m_text, m_matchCase, m_wholeWords);

    if (m_parallelExecution) {
        //processResults(QtConcurrent::mapped(pages, search));
    } else {
        processResults(FutureWrapper(pages, search));
    }
    qDebug() << "+++++++++++" << "search end";

}

void SearchTask::start(const QVector<PageBase *> &pages, const QString &text, bool matchCase, bool wholeWords, int beginAtPage)
{
    m_pages = pages;

    m_text = text;
    m_matchCase = matchCase;
    m_wholeWords = wholeWords;
    m_beginAtPage = beginAtPage;
    resetCancellation();
    QThread::start();
}

template<typename Future>
void SearchTask::processResults(Future future)
{
    for (int index = 0, count = m_pages.count(); index < count; ++index) {
        if (testCancellation()) {
            future.cancel();
            break;
        }

        const int shiftedIndex = (index + m_beginAtPage - 1) % count;//从当前页开始，保证循环后结束于当前页前一页
        const stSearchRes res = future.resultAt(shiftedIndex);

        if (res.listtext.size() > 0) {
            emit signal_restest();
            emit signal_resultReady(res);
        }
    }
}
