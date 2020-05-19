#ifndef RenderThreadDJVU_H
#define RenderThreadDJVU_H

#include <QThread>
#include <QMutex>
#include <QStack>
#include <QImage>

#include "global.h"

class SheetBrowserDJVUItem;
struct RenderTaskDJVU {
    SheetBrowserDJVUItem *item = nullptr;
    double scaleFactor = 1.0;
    Dr::Rotation rotation = Dr::RotateBy0;
};

class RenderThreadDJVU : public QThread
{
    Q_OBJECT
public:
    explicit RenderThreadDJVU(QObject *parent = nullptr);

    ~RenderThreadDJVU();

    static void appendTask(SheetBrowserDJVUItem *item, double scaleFactor, Dr::Rotation rotation);

    void run();

private slots:
    void onTaskFinished(SheetBrowserDJVUItem *item, QImage image, double scaleFactor, int rotation);

signals:
    void sigTaskFinished(SheetBrowserDJVUItem *item, QImage image, double scaleFactor, int rotation);

private:
    QStack<RenderTaskDJVU> m_tasks;
    QMutex m_mutex;
    bool m_quit = false;

    static RenderThreadDJVU *instance;
};

#endif // RenderThreadDJVU_H
