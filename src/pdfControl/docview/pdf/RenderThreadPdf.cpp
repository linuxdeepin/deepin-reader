#include "RenderThreadPdf.h"
#include "pagepdf.h"

static int MAX_TASK_NUM = 30;

RenderThreadPdf *RenderThreadPdf::_ins{nullptr};
RenderThreadPdf::RenderThreadPdf(QObject *object):
    QThread(object)
{

}

RenderThreadPdf::~RenderThreadPdf()
{
    _ins = nullptr;
    m_threadQuit = true;
    wait();
}

RenderThreadPdf *RenderThreadPdf::getIns()
{
    if (nullptr == _ins) {
        _ins = new RenderThreadPdf();
    }
    return _ins;
}

void RenderThreadPdf::appendTask(PageBase *item, double scaleFactor, RotateType_EM rotation)
{
    if (nullptr == _ins) {
        _ins = new RenderThreadPdf();
    }

    _ins->m_threadMutex.lock();

    bool exist = false;
    for (int i = 0; i < _ins->m_renderTasks.count(); ++i) {
        if (_ins->m_renderTasks[i].item == item) {
            _ins->m_renderTasks[i].scaleFactor = scaleFactor;
            _ins->m_renderTasks[i].rotation = rotation;
            exist = true;
            break;
        }
    }

//    while (_ins->m_renderTasks.count() > MAX_TASK_NUM) {
//        _ins->m_renderTasks.pop();
//    }

    if (!exist) {
        RenderTaskPdf task;
        task.item = item;
        task.scaleFactor = scaleFactor;
        task.rotation = rotation;
//        _ins->m_renderTasks.append(task);
        _ins->m_renderTasks.prepend(task);
    }

    _ins->m_threadMutex.unlock();

    if (!_ins->isRunning())
        _ins->start();
}

void RenderThreadPdf::destroyRenderPdfThread()
{
    if (nullptr != _ins) {
        delete _ins;
        _ins = nullptr;
    }
}

void RenderThreadPdf::run()
{
    m_threadQuit = false;
    while (!m_threadQuit) {
        msleep(10);
        if (m_renderTasks.count() <= 0) {
            msleep(10);
            break;
        }

        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }

        _ins->m_threadMutex.lock();

//        while (_ins->m_renderTasks.count() > MAX_TASK_NUM) {
//            _ins->m_renderTasks.pop();
//        }

        RenderTaskPdf task = m_renderTasks.pop();
        _ins->m_threadMutex.unlock();

        if (!PagePdf::existIns(task.item)) {
            continue;
        }

        reinterpret_cast<PagePdf *>(task.item)->renderImage(task.scaleFactor, task.rotation);
    }
}

void RenderThreadPdf::stopCurThread()
{
    _ins->m_threadMutex.lock();
    if (nullptr != _ins && _ins->isRunning()) {
//        m_threadQuit = true;
//        _ins->quit();
//        _ins->wait();
        _ins->m_renderTasks.clear();
    }
    _ins->m_threadMutex.unlock();
}

void RenderThreadPdf::clearCurTaskList()
{
    if (nullptr != _ins) {
        _ins->m_threadMutex.lock();
        _ins->m_renderTasks.clear();
        _ins->m_threadMutex.unlock();
    }
}

