#include "DocThread.h"
#include "DocSheet.h"
#include "PDFModel.h"
#include "DjVuModel.h"

#include <QTemporaryDir>
#include <QProcess>
#include <QDebug>

bool DocThread::s_quitForever = false;
DocThread *DocThread::s_instance = nullptr;
DocThread::DocThread(QObject *parent) : QThread(parent)
{
    connect(this, &DocThread::sigTaskFinished, this, &DocThread::onTaskFinished, Qt::QueuedConnection);
}

DocThread::~DocThread()
{
    m_quit = true;
    wait();
}

void DocThread::appendTask(DocOpenThreadTask task)
{
    if (s_quitForever)
        return;

    instance()->m_mutex.lock();

    instance()->m_tasks.append(task);

    instance()->m_mutex.unlock();

    if (!instance()->isRunning())
        instance()->start();
}

void DocThread::destroyForever()
{
    if (s_quitForever)
        return;

    s_quitForever = true;

    if (nullptr != s_instance)
        delete s_instance;

}

void DocThread::run()
{
    m_quit = false;

    while (!m_quit) {
        if (m_tasks.count() <= 0) {
            msleep(100);
            continue;
        }

        m_mutex.lock();

        DocOpenThreadTask task = m_tasks.pop();

        m_mutex.unlock();

        if (!DocSheet::existSheet(task.sheet))
            continue;

        int fileType = task.sheet->fileType();
        QString filePath = task.sheet->filePath();
        QString password = task.sheet->password();

        PERF_PRINT_BEGIN("POINT-03", QString("filename=%1,filesize=%2").arg(QFileInfo(filePath).fileName()).arg(QFileInfo(filePath).size()));

        deepin_reader::Document *document = deepin_reader::DocumentFactory::getDocument(fileType, filePath, password);

        if (nullptr == document) {
            emit sigTaskFinished(task.sheet, false, tr("Open failed"), nullptr, QList<deepin_reader::Page *>());
            PERF_PRINT_END("POINT-03", "");
            PERF_PRINT_END("POINT-05", QString("filename=%1,filesize=%2").arg(QFileInfo(filePath).fileName()).arg(QFileInfo(filePath).size()));
            continue;
        }

        int pagesNumber = document->pageCount();

        QList<deepin_reader::Page *> pages;

        for (int i = 0; i < pagesNumber; ++i) {
            deepin_reader::Page *page = document->page(i);
            if (nullptr == page) {
                break;
            }
            pages.append(page);
        }

        if (pages.count() == pagesNumber)
            emit sigTaskFinished(task.sheet, true, "", document, pages);
        else
            emit sigTaskFinished(task.sheet, false, tr("Please check if the file is damaged"), document, pages);

        PERF_PRINT_END("POINT-03", "");
        PERF_PRINT_END("POINT-05", QString("filename=%1,filesize=%2").arg(QFileInfo(filePath).fileName()).arg(QFileInfo(filePath).size()));
    }
}

void DocThread::onTaskFinished(DocSheet *sheet, bool result, QString error, deepin_reader::Document *document, QList<deepin_reader::Page *> pages)
{
    if (!DocSheet::existSheet(sheet))
        return;

    sheet->handleOpened(result, error, document, pages);
}

DocThread *DocThread::instance()
{
    if (s_quitForever)
        return nullptr;

    if (nullptr == s_instance)
        s_instance = new DocThread;

    return s_instance;
}
