#ifndef RENDERTASK_H
#define RENDERTASK_H

#include <QImage>
#include <QMutex>
#include <QRunnable>
#include <QSet>
#include <QWaitCondition>

#include "renderparam.h"

class ModelPage;
class ViewSettings;

class RenderTaskParent
{
    friend struct RenderTaskFinishedEvent;
    friend struct RenderTaskCanceledEvent;
    friend struct DeleteParentLaterEvent;

public:
    virtual ~RenderTaskParent();

private:
    virtual void on_finished(const RenderParam& renderParam,
                             const QRect& rect, bool prefetch,
                             const QImage& image, const QRectF& cropRect) = 0;
    virtual void on_canceled() = 0;
};

class RenderTaskDispatcher : public QObject
{
    Q_OBJECT

    friend class RenderTask;

private:
    Q_DISABLE_COPY(RenderTaskDispatcher)

    RenderTaskDispatcher(QObject* parent = nullptr);


    void finished(RenderTaskParent* parent,
                  const RenderParam& renderParam,
                  const QRect& rect, bool prefetch,
                  const QImage& image, const QRectF& cropRect);
    void canceled(RenderTaskParent* parent);

    void deleteParentLater(RenderTaskParent* parent);

public:
    bool event(QEvent* event);

private:
    QSet< RenderTaskParent* > m_activeParents;

    void addActiveParent(RenderTaskParent* parent);
    void removeActiveParent(RenderTaskParent* parent);

};

class RenderTask : public QRunnable
{
public:
    explicit RenderTask(ModelPage* page, RenderTaskParent* parent = nullptr);
    ~RenderTask();

    void wait();

    bool isRunning() const;

    bool wasCanceled() const { return loadCancellation() != NotCanceled; }
    bool wasCanceledNormally() const { return loadCancellation() == CanceledNormally; }
    bool wasCanceledForcibly() const { return loadCancellation() == CanceledForcibly; }

    void run();

    void start(const RenderParam& renderParam,
               const QRect& rect, bool prefetch);

    void cancel(bool force = false) { setCancellation(force); }

    void deleteParentLater();

private:
    Q_DISABLE_COPY(RenderTask)

    static ViewSettings* s_settings;

    static RenderTaskDispatcher* s_dispatcher;
    RenderTaskParent* m_parent;

    mutable QMutex m_mutex;
    QWaitCondition m_waitCondition;

    bool m_isRunning;
    QAtomicInt m_wasCanceled;

    enum
    {
        NotCanceled = 0,
        CanceledNormally = 1,
        CanceledForcibly = 2
    };

    inline void setCancellation(bool force){ m_wasCanceled.storeRelease(force ? CanceledForcibly : CanceledNormally);}
    inline void resetCancellation(){ m_wasCanceled.storeRelease(NotCanceled);}
    inline bool testCancellation(){ return m_prefetch ?m_wasCanceled.load() == CanceledForcibly :m_wasCanceled.load() != NotCanceled;}
    inline int loadCancellation() const{ return m_wasCanceled.load();}

    void finish(bool canceled);
    ModelPage* m_page;

    static const RenderParam s_defaultRenderParam;
    RenderParam m_renderParam;

    QRect m_rect;
    bool m_prefetch;

};


#endif // RENDERTASK_H
