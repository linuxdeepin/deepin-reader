#include "RenderThreadPdf.h"
#include "pagepdf.h"

static int MAXTASKNUM = 20;

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

    if (_ins->m_renderTasks.count() > MAXTASKNUM)
        _ins->m_renderTasks.clear();

    if (!exist) {
        RenderTaskPdf task;
        task.item = item;
        task.scaleFactor = scaleFactor;
        task.rotation = rotation;
        _ins->m_renderTasks.append(task);
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

        _ins->m_threadMutex.lock();
        RenderTaskPdf task = m_renderTasks.pop();
        _ins->m_threadMutex.unlock();

        if (!PagePdf::existIns(task.item)) {
            continue;
        }

        reinterpret_cast<PagePdf *>(task.item)->renderImage(task.scaleFactor, task.rotation);
    }
}

