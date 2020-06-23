/**
 * @author Richard
 * @email richardo2016@mail.com
 * @create date 2020-06-22 04:41:41
 * @modify date 2020-06-22 04:41:41
 * 
 * @desc provide some wrapper/utils to help uv work with fibjs
 */
#include "object.h"

#ifndef ASYNCUV_H_
#define ASYNCUV_H_

namespace fibjs {
    class uvAsyncEvent : public exlib::Task_base {
        public:
            virtual ~uvAsyncEvent()
            {
            }

            void post();

            virtual void start()
            {
            }

        public:
            virtual void resume()
            {
                post();
            }
    };
}

#endif // ASYNCUV_H_