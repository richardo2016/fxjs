#include "object.h"
#include "utils.h"
#include "FSWatcher.h"
#include "AsyncUV.h"
#include <uv/include/uv.h>
#include "Runtime.h"
#include "Buffer.h"

namespace fibjs {

static uv_loop_t* s_uv_loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));

static uv_async_t s_uv_asyncWatcher;
static exlib::LockedList<uvAsyncEvent> s_uvWait;

void uvAsyncEvent::post()
{
    s_uvWait.putTail(this);
    uv_async_send(&s_uv_asyncWatcher);
}

void FSWatcher::AsyncWatchFSProc::start()
{
    uv_fs_event_init(s_uv_loop, &m_fs_handle);
    int32_t hr = uv_fs_event_start(&m_fs_handle, fs_event_cb, m_watcher->get_target(), m_watcher->isRecursiveForDir() ? UV_FS_EVENT_RECURSIVE : NULL);
    if (hr != 0) {
        m_watcher->onError(hr, uv_strerror(hr));
        finished();
    }
}

class FSIOThread : public exlib::OSThread {
public:
    FSIOThread()
    {
        m_lock.lock();
        uv_loop_init(s_uv_loop);
    }

    virtual void Run()
    {
        Runtime rt(NULL);

        uv_async_init(s_uv_loop, &s_uv_asyncWatcher, AsyncEventCallback);

        m_lock.unlock();

        uv_run(s_uv_loop, UV_RUN_DEFAULT);
    }

private:
    static void AsyncEventCallback(uv_async_t* handle)
    {
        assert(handle == &s_uv_asyncWatcher);

        exlib::List<uvAsyncEvent> jobs;
        uvAsyncEvent* p1;

        s_uvWait.getList(jobs);

        while ((p1 = jobs.getHead()) != 0)
            p1->start();
    }

public:
    exlib::spinlock m_lock;
};

void initializeFSIOThread()
{
    static FSIOThread s_afsIO;
    s_afsIO.start();
}
}