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
    int32_t uv_err_no = uv_fs_event_start(&m_fs_handle, fs_event_cb, m_watcher->get_target(), m_watcher->isRecursiveForDir() ? UV_FS_EVENT_RECURSIVE : NULL);
    if (uv_err_no != 0) {
        m_watcher->onError(CALL_E_INVALID_CALL, uv_strerror(uv_err_no));
        m_watcher->close();
    }
}

class UVAsyncThread : public exlib::OSThread {
public:
    UVAsyncThread()
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

void initializeUVAsyncThread()
{
    static UVAsyncThread s_afsIO;
    s_afsIO.start();
}
}