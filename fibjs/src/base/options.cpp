/*
 * options.cpp
 *
 *  Created on: Jul 23, 2015
 *      Author: lion
 */

#include "v8/src/flags.h"

#include "version.h"
#include "console.h"
#include "path.h"
#include "Fiber.h"
#include "options.h"

namespace fibjs {

#ifdef amd64
int32_t stack_size = 512;
#else
int32_t stack_size = 256;
#endif

bool g_prof = false;
int32_t g_prof_interval = 1000;

FILE* g_cov = nullptr;

bool g_tcpdump = false;

#ifdef DEBUG
#define GUARD_SIZE 32
#else
#define GUARD_SIZE 16
#endif

static class _init_v8_opt {
public:
    _init_v8_opt()
    {
        v8::internal::FLAG_lazy = false;
        v8::internal::FLAG_stack_size = stack_size - GUARD_SIZE;

        v8::internal::FLAG_wasm_async_compilation = false;

        // v8::internal::FLAG_parallel_scavenge = false;
        // v8::internal::FLAG_parallel_marking = false;
        // v8::internal::FLAG_concurrent_marking = false;
    }
} s_init_v8_opt;

static void printHelp()
{
    puts("Usage: fibjs [options] [script.js] [arguments] \n"
         "\n"
         "Options:\n"
         "  -h, --help           print fibjs command line options.\n"
         "  -v, --version        print fibjs version.\n"
         "\n"
         "  --use-thread         run fibjs in thread mode.\n"
         "  --tcpdump            print out the contents of the tcp package.\n"
         "\n"
         "  --init               write a package.json file.\n"
         "  --install            install the dependencies in the local node_modules folder.\n"
         "\n"
         "  --prof               log statistical profiling information.\n"
         "  --prof-interval=n    interval for --prof samples (in microseconds, default: 1000).\n"
         "  --prof-process       process log file generated by profiler.start.\n"
         "\n"
         "  --cov[=filename]     collect code coverage information (only work on the main Worker).\n"
         "  --cov-process        generate code coverage analysis report.\n"
         "\n"
         "  --v8-options         print v8 command line options.\n"
         "\n"
         "Documentation can be found at http://fibjs.org\n");
}

#ifdef DEBUG
void asyncLog(int32_t priority, exlib::string msg);
void DcheckHandler(const char* file, int line, const char* message)
{
    char p_msg[256];
    sprintf(p_msg, "Assert(DCheck) in %s, line %d: %s", file, line, message);
    asyncLog(console_base::_DEBUG, p_msg);
}
#endif

void options(int32_t& pos, char* argv[])
{
    int32_t argc = pos;
    int32_t i;

    for (pos = 1; (pos < argc) && (argv[pos][0] == '-'); pos++)
        if (argv[pos][1] == '-') {
            exlib::string tmp("opt_tools/");
            tmp += argv[pos] + 2;

            for (i = 0; opt_tools[i].name && qstrcmp(opt_tools[i].name, tmp.c_str()); i++)
                ;

            if (opt_tools[i].name)
                break;
        }

    argc = pos;
    int32_t df = 0;

    for (int32_t i = 0; i < argc; i++) {
        char* arg = argv[i];

        if (df)
            argv[i - df] = arg;

        if (!qstrcmp(arg, "--help") || !qstrcmp(arg, "-h")) {
            printHelp();
            _exit(0);
        } else if (!qstrcmp(arg, "--version") || !qstrcmp(arg, "-v")) {
            printf("v%s\n", fxjs_version);
            _exit(0);
        } else if (!qstrcmp(arg, "--use-thread")) {
            exlib::Service::use_thread = true;
            df++;
        } else if (!qstrcmp(arg, "--tcpdump")) {
            g_tcpdump = true;
            df++;
        } else if (!qstrcmp(arg, "--prof")) {
            g_prof = true;
            df++;
        } else if (!qstrcmp(arg, "--prof-interval=", 16)) {
            g_prof_interval = atoi(arg + 16);
            if (g_prof_interval < 50)
                g_prof_interval = 50;
            df++;
        } else if (!qstrcmp(arg, "--cov=", 6)) {
            g_cov = fopen(arg + 6, "a");
            if (g_cov == nullptr) {
                printf("Invalid filename: %s\n", arg + 6);
                _exit(0);
            }
            df++;
        } else if (!qstrcmp(arg, "--cov")) {
            char name[22];
            date_t d;
            d.now();
            sprintf(name, "fibjs-%d.lcov", (int32_t)d.date());
            g_cov = fopen(name, "a");
            if (g_cov == nullptr) {
                printf("Can't open file: %s, please try again", name);
                _exit(0);
            }
            df++;
        } else if (!qstrcmp(arg, "--v8-options")) {
            v8::internal::FlagList::PrintHelp();
            _exit(0);
        }
    }

    if (df)
        argc -= df;

    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
#ifdef DEBUG
    v8::V8::SetDcheckErrorHandler(DcheckHandler);
#endif
}
}
