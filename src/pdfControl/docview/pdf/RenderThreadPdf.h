#ifndef RENDERTHREADPDF_H
#define RENDERTHREADPDF_H

#include <QObject>
#include <QThreadPool>
#include <QPixmap>
#include <QRunnable>
#include <QMap>
#include <QMutex>
#include <QStack>

#include "../commonstruct.h"

class PageBase;
struct RenderTaskPdf {
    PageBase *item = nullptr;
    double scaleFactor = 1.0;
    RotateType_EM  rotation = RotateType_EM::RotateType_0;
};

class RenderThreadPdf: public QThread
{
    Q_OBJECT
public:
    explicit RenderThreadPdf(QObject *object = nullptr);
    ~RenderThreadPdf() override;

    static RenderThreadPdf *getIns();

    static void appendTask(PageBase *item, double scaleFactor, RotateType_EM rotation);

    static void destroyRenderPdfThread();

    void run() override;

    void stopCurThread();

    void clearCurTaskList();

private:
    QMutex m_threadMutex;
    QStack<RenderTaskPdf> m_renderTasks;
    bool m_threadQuit{false};

    static RenderThreadPdf *_ins;
};

#endif // RENDERTHREADPDF_H
