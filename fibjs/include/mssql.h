/*
 * mssql.h
 *
 *  Created on: Oct 29, 2016
 *      Author: lion
 */

#ifndef _MSSQL_H_
#define _MSSQL_H_

#include "ifs/MSSQL.h"
#include "utf8.h"

// #import "msado25.tlb" raw_interfaces_only, rename("EOF", "adoEOF")
#include "msado25.tlh"

namespace fibjs {

class mssql : public MSSQL_base {
public:
    mssql()
        : m_conn(NULL)
    {
    }

    virtual ~mssql();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t begin(AsyncEvent* ac);
    virtual result_t commit(AsyncEvent* ac);
    virtual result_t rollback(AsyncEvent* ac);
    virtual result_t trans(v8::Local<v8::Function> func, bool& retVal);
    virtual result_t execute(exlib::string sql, OptArgs args, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t insert(v8::Local<v8::Object> opts, double& retVal, AsyncEvent* ac);
    virtual result_t find(v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t count(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t update(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t remove(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac);
    virtual result_t format(exlib::string method, v8::Local<v8::Object> opts, exlib::string& retVal);
    virtual result_t format(exlib::string sql, OptArgs args, exlib::string& retVal);

public:
    // MySQL_base
    virtual result_t use(exlib::string dbName, AsyncEvent* ac);

public:
    result_t connect(const char* server, const char* username, const char* password, const char* dbName);
    result_t execute(const char* sql, int32_t sLen, obj_ptr<NArray>& retVal);

private:
    inline result_t error(HRESULT hr)
    {
        ADODB::Errors* errs = NULL;

        m_conn->get_Errors(&errs);
        if (errs) {
            ADODB::Error* err = NULL;
            _variant_t i(0);

            errs->get_Item(i, &err);
            if (err) {
                BSTR msg = NULL;
                err->get_Description(&msg);
                err->Release();

                if (msg) {
                    exlib::string msga = utf16to8String(msg);
                    SysFreeString(msg);
                    return Runtime::setError(msga);
                }
            }

            errs->Release();
        }

        return hr;
    }

public:
    ADODB::_Connection* m_conn;
};

} /* namespace fibjs */
#endif /* _MSSQL_H_ */
