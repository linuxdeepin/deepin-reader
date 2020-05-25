#include "RenderThreadDJVU.h"
#include "SheetBrowserDJVUItem.h"

RenderThreadDJVU *RenderThreadDJVU::instance = nullptr;
RenderThreadDJVU::RenderThreadDJVU(QObject *parent) : QThread(parent)
{
    connect(this, SIGNAL(sigTaskFinished(SheetBrowserDJVUItem *, QImage, double, int)), this, SLOT(onTaskFinished(SheetBrowserDJVUItem *, QImage, double, int)), Qt::QueuedConnection);
}

RenderThreadDJVU::~RenderThreadDJVU()
{
    instance = nullptr;
    m_quit = true;
    wait();
}

void RenderThreadDJVU::appendTask(SheetBrowserDJVUItem *item, double scaleFactor, Dr::Rotation rotation)
{
    if (nullptr == instance)
        instance = new RenderThreadDJVU;

    instance->m_mutex.lock();

    bool exist = false;
    for (int i = 0; i < instance->m_tasks.count(); ++i) {
        if (instance->m_tasks[i].item == item) {
            instance->m_tasks[i].scaleFactor = scaleFactor;
            instance->m_tasks[i].rotation = rotation;
            exist = true;
            break;
        }
    }

    if (!exist) {
        RenderTaskDJVU task;
        task.item = item;
        task.scaleFactor = scaleFactor;
        task.rotation = rotation;
        instance->m_tasks.append(task);
    }

    instance->m_mutex.unlock();

    if (!instance->isRunning())
        instance->start();
}

void RenderThreadDJVU::run()
{
    m_quit = false;

    while (!m_quit) {
        if (m_tasks.count() <= 0) {
            msleep(10);
            break;
        }

        m_mutex.lock();
        RenderTaskDJVU task = m_tasks.pop();
        m_mutex.unlock();

        if (!SheetBrowserDJVUItem::existInstance(task.item))
            continue;

        QImage image = task.item->getImage(task.scaleFactor, task.rotation);

        if (!image.isNull())
            emit sigTaskFinished(task.item, image, task.scaleFactor, task.rotation);
    }
}

void RenderThreadDJVU::destroy()
{
    if (nullptr != instance)
        delete instance;
}

void RenderThreadDJVU::onTaskFinished(SheetBrowserDJVUItem *item, QImage image, double scaleFactor, int rotation)
{
    if (SheetBrowserDJVUItem::existInstance(item))
        item->handleRenderFinished(scaleFactor, (Dr::Rotation)rotation, image);
}
