#include "PerformanceMonitor.h"
#include <Windows.h>
#include "psapi.h"
#include "pdh.h"
#include <stdio.h>
#include "mel_util.h"

namespace mel {

    namespace util {

        static PDH_HQUERY cpuQuery;
        static PDH_HCOUNTER cpuTotal;

        static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        static int numProcessors;
        static HANDLE self;

        PerformanceMonitor::PerformanceMonitor() {
            PdhOpenQuery(NULL, NULL, &cpuQuery);
            PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
            PdhCollectQueryData(cpuQuery);

            SYSTEM_INFO sysInfo;
            FILETIME ftime, fsys, fuser;

            GetSystemInfo(&sysInfo);
            numProcessors = sysInfo.dwNumberOfProcessors;

            GetSystemTimeAsFileTime(&ftime);
            memcpy(&lastCPU, &ftime, sizeof(FILETIME));

            self = GetCurrentProcess();
            GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
            memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
            memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
        }

        uint64 PerformanceMonitor::virt_mem_available() {
            MEMORYSTATUSEX mem_info;
            mem_info.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&mem_info);
            return mem_info.ullTotalPageFile;
        }

        uint64 PerformanceMonitor::virt_mem_used_total() {
            MEMORYSTATUSEX mem_info;
            mem_info.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&mem_info);
            return mem_info.ullTotalPageFile - mem_info.ullAvailPageFile;
        }

        uint64 PerformanceMonitor::virt_mem_used_process() {
            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
            return pmc.PrivateUsage;
        }

        uint64 PerformanceMonitor::ram_available() {
            MEMORYSTATUSEX mem_info;
            mem_info.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&mem_info);
            return mem_info.ullTotalPhys;
        }

        uint64 PerformanceMonitor::ram_used_total() {
            MEMORYSTATUSEX mem_info;
            mem_info.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&mem_info);
            return mem_info.ullTotalPhys - mem_info.ullAvailPhys;
        }

        uint64 PerformanceMonitor::ram_used_process() {
            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
            return pmc.WorkingSetSize;
        }

        double PerformanceMonitor::cpu_used_total() {
            PDH_FMT_COUNTERVALUE counterVal;
            PdhCollectQueryData(cpuQuery);
            PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
            return counterVal.doubleValue;
        }

        double PerformanceMonitor::cpu_used_process() {
            FILETIME ftime, fsys, fuser;
            ULARGE_INTEGER now, sys, user;
            double percent;

            GetSystemTimeAsFileTime(&ftime);
            memcpy(&now, &ftime, sizeof(FILETIME));

            GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
            memcpy(&sys, &fsys, sizeof(FILETIME));
            memcpy(&user, &fuser, sizeof(FILETIME));
            percent = (sys.QuadPart - lastSysCPU.QuadPart) +
                (user.QuadPart - lastUserCPU.QuadPart);
            percent /= (now.QuadPart - lastCPU.QuadPart);
            percent /= numProcessors;
            lastCPU = now;
            lastUserCPU = user;
            lastSysCPU = sys;

            return percent * 100;
        }
    }

}

// Implementation Resources:
// mem/ram/cpu: https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
// cpu: http://www.philosophicalgeek.com/2009/01/03/determine-cpu-usage-of-current-process-c-and-c/
