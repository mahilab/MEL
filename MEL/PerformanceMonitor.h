#pragma once
#include "mel_types.h"

namespace mel{

    namespace util {

        class PerformanceMonitor {

        public:

            PerformanceMonitor();

            uint64 virt_mem_available();
            uint64 virt_mem_used_total();
            uint64 virt_mem_used_process();

            uint64 ram_available();
            uint64 ram_used_total();
            uint64 ram_used_process();

            double cpu_used_total();
            double cpu_used_process();

        };
    }
}

// See Also:
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
// http://www.philosophicalgeek.com/2009/01/03/determine-cpu-usage-of-current-process-c-and-c/