/*
 * SQLite.cpp
 *
 *  Created on: Oct 27, 2019
 *      Author: lion
 */

#include "object.h"

namespace fibjs {

template <typename T>
result_t db_execute(T* pThis, exlib::string sql, OptArgs args, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        exlib::string str;
        result_t hr = pThis->format(sql, args, str);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = str;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string str = ac->m_ctx[0].string();
    return pThis->execute(str.c_str(), (int32_t)str.length(), retVal);
}

template <typename T>
result_t db_execute(T* pThis, exlib::string method, v8::Local<v8::Object> opts,
    obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        exlib::string str;
        result_t hr = pThis->format(method, opts, str);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = str;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string str = ac->m_ctx[0].string();
    return pThis->execute(str.c_str(), (int32_t)str.length(), retVal);
}

template <typename T>
result_t db_insert(T* pThis, v8::Local<v8::Object> opts, double& retVal, AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, "insert", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    retVal = _retVal->m_values[1].m_val.dblVal();

    return 0;
}

template <typename T>
result_t db_find(T* pThis, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal,
    AsyncEvent* ac)
{
    return db_execute(pThis, "find", opts, retVal, ac);
}

template <typename T>
result_t db_count(T* pThis, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, "count", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    Variant v;
    _retVal->_indexed_getter(0, v);
    retVal = ((NObject*)v.object())->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_update(T* pThis, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, "update", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    retVal = _retVal->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_remove(T* pThis, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, "remove", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    retVal = _retVal->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_begin(T* pThis, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;
    return pThis->execute("BEGIN", 5, retVal);
}

template <typename T>
result_t db_commit(T* pThis, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;
    return pThis->execute("COMMIT", 6, retVal);
}

template <typename T>
result_t db_rollback(T* pThis, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;
    return pThis->execute("ROLLBACK", 8, retVal);
}

template <typename T>
inline result_t db_trans(T* pThis, v8::Local<v8::Function> func, bool& retVal)
{
    result_t hr = 0;
    retVal = false;

    hr = pThis->ac_begin();
    if (hr < 0)
        return hr;

    pThis->leave();
    v8::Local<v8::Value> result = func->Call(pThis->wrap(), 0, NULL);
    pThis->enter();

    if (result.IsEmpty()) {
        pThis->ac_rollback();
        return CALL_E_JAVASCRIPT;
    }

    if (result->IsFalse())
        return pThis->ac_rollback();
    else {
        retVal = true;
        return pThis->ac_commit();
    }
}
}
