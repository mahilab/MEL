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