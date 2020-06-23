/*
 * console_key.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: lion
 */

#include "object.h"
#include "ifs/console.h"

#if !defined(_WIN32) && !defined(Darwin)

namespace fibjs {

result_t console_base::keyDown(exlib::string key, exlib::string modifier)
{
    return CALL_E_INVALID_CALL;
}

result_t console_base::keyDown(exlib::string key, v8::Local<v8::Array> modifier)
{
    return CALL_E_INVALID_CALL;
}

result_t console_base::keyUp(exlib::string key, exlib::string modifier)
{
    return CALL_E_INVALID_CALL;
}

result_t console_base::keyUp(exlib::string key, v8::Local<v8::Array> modifier)
{
    return CALL_E_INVALID_CALL;
}

result_t console_base::keyTap(exlib::string key, exlib::string modifier)
{
    return CALL_E_INVALID_CALL;
}

result_t console_base::keyTap(exlib::string key, v8::Local<v8::Array> modifier)
{
    return CALL_E_INVALID_CALL;
}

result_t console_base::typeString(exlib::string text)
{
    return CALL_E_INVALID_CALL;
}
}

#endif
