#include <stddef.h>
#include <windows.h>
#include <Psapi.h>

#ifndef __mem_usage_include__
#define __mem_usage_include__

class Memory_Usage
{
    public:
        static size_t get()
        {
            PROCESS_MEMORY_COUNTERS memCounter;
            BOOL result = K32GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
            SIZE_T mem_usage = memCounter.PeakWorkingSetSize;
            return (size_t) mem_usage;
        }

        static double get_in_kb()
        {
            return get()/1024.0;
        }

        static double get_in_mb()
        {
            return get_in_kb()/1024.0;
        }

        static double get_in_gb()
        {
            return get_in_mb()/1024.0;
        }
};

#endif