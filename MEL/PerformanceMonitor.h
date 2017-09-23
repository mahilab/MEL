#pragma once
#include "mel_types.h"
#pragma warning(disable : 4006) // disables warning about already defined in winmm.lib(WINMM.dll)


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