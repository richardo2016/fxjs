/*
 * RedisHash.cpp
 *
 *  Created on: Aug 4, 2014
 *      Author: lion
 */

#include "object.h"
#include "RedisHash.h"

namespace fibjs {

result_t RedisHash::set(Buffer_base* field, Buffer_base* value)
{
    Variant v;
    return m_rdb->doCommand("HSET", m_key, field, value, v);
}

result_t RedisHash::setNX(Buffer_base* field, Buffer_base* value)
{
    Variant v;
    return m_rdb->doCommand("HSETNX", m_key, field, value, v);
}

result_t RedisHash::mset(v8::Local<v8::Object> kvs)
{
    Variant v;
    return m_rdb->doCommand("HMSET", m_key, kvs, v);
}
result_t RedisHash::mset(OptArgs kvs)
{
    Variant v;
    return m_rdb->doCommand("HMSET", m_key, kvs, v);
}

result_t RedisHash::get(Buffer_base* field, obj_ptr<Buffer_base>& retVal)
{
    return m_rdb->doCommand("HGET", m_key, field, retVal);
}

result_t RedisHash::mget(v8::Local<v8::Array> fields, obj_ptr<NArray>& retVal)
{
    return m_rdb->doCommand("HMGET", m_key, fields, retVal);
}

result_t RedisHash::mget(OptArgs fields, obj_ptr<NArray>& retVal)
{
    return m_rdb->doCommand("HMGET", m_key, fields, retVal);
}

result_t RedisHash::incr(Buffer_base* field, int64_t num, int64_t& retVal)
{
    return m_rdb->doCommand("HINCRBY", m_key, field, num, retVal);
}

result_t RedisHash::getAll(obj_ptr<NArray>& retVal)
{
    return m_rdb->doCommand("HGETALL", m_key, retVal);
}

result_t RedisHash::keys(obj_ptr<NArray>& retVal)
{
    return m_rdb->doCommand("HKEYS", m_key, retVal);
}

result_t RedisHash::len(int32_t& retVal)
{
    return m_rdb->doCommand("HLEN", m_key, retVal);
}

result_t RedisHash::del(v8::Local<v8::Array> fields, int32_t& retVal)
{
    return m_rdb->doCommand("HDEL", m_key, fields, retVal);
}

result_t RedisHash::del(OptArgs fields, int32_t& retVal)
{
    return m_rdb->doCommand("HDEL", m_key, fields, retVal);
}

result_t RedisHash::exists(Buffer_base* field, bool& retVal)
{
    return m_rdb->doCommand("HEXISTS", m_key, field, retVal);
}
}
