/*
 * console_std.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#include "console.h"
#include "utf8.h"
#include <stdio.h>
#include "ifs/tty.h"

namespace fibjs {

#ifdef _WIN32

void std_logger::out(exlib::string& txt)
{
    class color_out {
    public:
        color_out()
        {
            tty_base::isatty(_fileno(stdout), m_tty);

            if (!m_tty) {
                m_handle = GetStdHandle(STD_OUTPUT_HANDLE);
                m_Now = m_wAttr = 0x7;
                m_wLight = m_wAttr & FOREGROUND_INTENSITY;
            }
        }

        void WriteConsole(exlib::wchar* ptr, size_t sz)
        {
            DWORD dwWrite;

            while (sz) {
                size_t sz1 = sz;
                if (sz1 >= 16384)
                    sz1 = 16384;
                WriteConsoleW(m_handle, ptr, (DWORD)sz1, &dwWrite, NULL);
                ptr += sz1;
                sz -= sz1;
            }
        }

        void out(exlib::string& s)
        {
            static HANDLE s_console;

            if (!m_tty) {
                fwrite(s.c_str(), 1, s.length(), stdout);
                return;
            }

            if (!m_handle) {
                if (!s_console) {
                    AllocConsole();

                    freopen("CONIN$", "r", stdin);
                    freopen("CONOUT$", "w", stdout);
                    freopen("CONOUT$", "w", stderr);

                    s_console = GetStdHandle(STD_OUTPUT_HANDLE);
                }

                m_handle = s_console;
                m_Now = m_wAttr = 0x7;
                m_wLight = m_wAttr & FOREGROUND_INTENSITY;
            }

            exlib::wstring ws = utf8to16String(s);
            exlib::wchar* ptr = &ws[0];
            exlib::wchar* pend = ptr + ws.length();
            exlib::wchar* ptr1;
            exlib::wchar* ptr2;

            while (ptr1 = (exlib::wchar*)qstrstr(ptr, L"\x1b[")) {
                for (ptr2 = ptr1 + 2; *ptr2 == ';' || qisdigit(ptr2[0]); ptr2++)
                    ;

                if (ptr1 > ptr)
                    WriteConsole(ptr, ptr1 - ptr);
                ptr = ptr2;
                if (ptr2[0])
                    ptr++;

                if (ptr2[0] != 'm')
                    continue;

                WORD _Now = m_Now;

                ptr1 += 2;
                if (ptr2 == ptr1 + 1 && *ptr1 == '0') {
                    _Now = m_wAttr;
                } else {
                    while (ptr1 < ptr2) {
                        if (*ptr1 == ';')
                            continue;

                        DWORD n = 0;
                        exlib::wchar ch;

                        while (qisdigit(ch = *ptr1++))
                            n = n * 10 + (ch - '0');

                        WORD val = n % 10;
                        val = (val & 2) | ((val & 1) << 2) | ((val & 4) >> 2);

                        if (n >= 0 && n < 2)
                            _Now = (_Now & ~FOREGROUND_INTENSITY) | (n ? FOREGROUND_INTENSITY : 0);
                        else if (n >= 30 && n < 38)
                            _Now = (_Now & 0xf8) | val;
                        else if (n >= 40 && n < 48)
                            _Now = (_Now & 0x0f) | (val << 4);
                        else if (n >= 90 && n < 98)
                            _Now = (_Now & 0xf0) | val | FOREGROUND_INTENSITY;
                    }
                }

                if (_Now != m_Now) {
                    m_Now = _Now;
                    SetConsoleTextAttribute(m_handle, m_Now);
                }
            }

            WriteConsole(ptr, pend - ptr);
        }

    private:
        HANDLE m_handle;
        WORD m_wAttr, m_Now;
        WORD m_wLight;
        bool m_tty;
    };

    static color_out s_out;
    s_out.out(txt);
}

#else

void std_logger::out(exlib::string& txt)
{
    fwrite(txt.c_str(), 1, txt.length(), stdout);
}

#endif

result_t std_logger::write(AsyncEvent* ac)
{
    item* p1;

    while ((p1 = m_workinglogs.getHead()) != 0) {
        exlib::string txt;
        if (p1->m_priority == console_base::_NOTICE)
            txt = logger::notice() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority == console_base::_WARN)
            txt = logger::warn() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority <= console_base::_ERROR)
            txt = logger::error() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority == console_base::_PRINT)
            txt = p1->m_msg;
        else
            txt = p1->m_msg + "\n";

        out(txt);

        delete p1;
    }
    fflush(stdout);

    return 0;
}
}
