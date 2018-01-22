#include <MEL/Utility/System.hpp>
#include <iomanip>
#include <ctime>

#ifdef __linux__
    #include <sys/stat.h>
    #include <errno.h>
    #include <time.h>
    #include <pthread.h>
    #include <sched.h>
#elif _WIN32
    #include <windows.h>
    #include <tchar.h>
    #include <pdh.h>
    #include <psapi.h>
#endif

namespace mel {

//==============================================================================
// DATE FUNCTIONS
//==============================================================================

const std::string get_ymdhms() {
    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer [80];
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(buffer,80,"%Y-%m-%d-%H.%M.%S",timeinfo);
    return buffer;
}

//==============================================================================
// DIRECTORY FUNCTIONS
//==============================================================================

std::string get_path_slash() {
    #ifdef __linux__
        return "/";
    #elif _WIN32
        return "\\";
    #endif
}

std::vector<std::string> parse_path(std::string path) {
    std::vector<std::string> directories;
    std::size_t found = path.find_first_of("/\\");
    while (found != std::string::npos) {
        std::string new_dir = path.substr(0, found);
        directories.push_back(new_dir);
        path = path.substr(found + 1, path.length());
        found = path.find_first_of("/\\");
    }
    directories.push_back(path);
    return directories;
}

void create_directory(std::string directory) {
    std::vector<std::string> dirs = parse_path(directory);
    for (std::size_t i = 0; i < dirs.size(); ++i) {
        std::string sub_path;
        for (std::size_t j = 0; j <= i; ++j) {
            sub_path += dirs[j];
            sub_path += get_path_slash();
        }
        #ifdef __linux__
            mkdir(sub_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        #elif _WIN32
            CreateDirectory(sub_path.c_str(), NULL);
        #endif
    }
}

//==============================================================================
// SYSTEM FUNCTIONS
//==============================================================================

void sleep(Time duration) {
    if (duration >= Time::Zero) {
        #ifdef __linux__
            uint64 usecs = duration.as_microseconds();
            // Construct the time to wait
            timespec ti;
            ti.tv_nsec = (usecs % 1000000) * 1000;
            ti.tv_sec = usecs / 1000000;
            // If nanosleep returns -1, we check errno. If it is EINTR
            // nanosleep was interrupted and has set ti to the remaining
            // duration. We continue sleeping until the complete duration
            // has passed. We stop sleeping if it was due to an error.
            while ((nanosleep(&ti, &ti) == -1) && (errno == EINTR)) {
            }
        #elif _WIN32
            TIMECAPS tc;
            timeGetDevCaps(&tc, sizeof(TIMECAPS));
            timeBeginPeriod(tc.wPeriodMin);
            // ::Sleep(duration.as_milliseconds()); // low-resolution method
            HANDLE timer;
            LARGE_INTEGER ft;
            ft.QuadPart = -(10 * duration.as_microseconds());
            timer = CreateWaitableTimer(NULL, TRUE, NULL);
            SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
            WaitForSingleObject(timer, INFINITE);
            CloseHandle(timer);
            // timeEndPeriod(tc.wPeriodMin); // to much overhead, not necessary?
        #endif
    }
}

std::string get_last_os_error() {
    #ifdef __linux__
        return std::string("get_last_os_error() not yet implemented on Linux!");
    #elif _WIN32
        //Get the error message, if any.
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0)
            return std::string(); //No error message has been recorded
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        std::string message(messageBuffer, size);
        //Free the buffer.
        LocalFree(messageBuffer);
        return message;
    #endif
}

bool enable_realtime() {
    #ifdef __linux__
        int ret;
        // We'll operate on the currently running thread.
        pthread_t this_thread = pthread_self();
        // struct sched_param is used to store the scheduling priority
        struct sched_param params;
        // We'll set the priority to the maximum.
        params.sched_priority = sched_get_priority_max(SCHED_FIFO);
         // Attempt to set thread real-time priority to the SCHED_FIFO policy
        ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);
        if (ret != 0) {
            // Unsuccessful in setting thread realtime priority
            return false;
        }
        // Now verify the change in thread priority
        int policy = 0;
        ret = pthread_getschedparam(this_thread, &policy, &params);
        if (ret != 0) {
            // Couldn't retrieve real-time scheduling paramers
            return false;
        }
        // Check the correct policy was applied
        if(policy != SCHED_FIFO) {
            // Scheduling is NOT SCHED_FIFO!
            return false;
        }
        else {
            // Success
            return true;
        }
    #elif _WIN32
        DWORD dwError;
        if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
            dwError = GetLastError();
            _tprintf(TEXT("ERROR: Failed to elevate process priority (%d)\n"), static_cast<int>(dwError));
            return false;
        }
        DWORD dwPriClass;
        if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)){
            dwPriClass = GetLastError();
            _tprintf(TEXT("ERROR: Failed to elevate thread priority (%d)\n"), static_cast<int>(dwPriClass));
            return false;
        }
        return true;
    #endif
}

bool disable_realtime() {
    #ifdef __linux__
        int ret;
        // We'll operate on the currently running thread.
        pthread_t this_thread = pthread_self();
        // struct sched_param is used to store the scheduling priority
        struct sched_param params;
        // We'll set the priority to the maximum.
        params.sched_priority = sched_get_priority_max(SCHED_OTHER);
         // Attempt to set thread normal priority to the SCHED_OTHER policy
        ret = pthread_setschedparam(this_thread, SCHED_OTHER, &params);
        if (ret != 0) {
            // Unsuccessful in setting thread normal priority
            return false;
        }
        // Now verify the change in thread priority
        int policy = 0;
        ret = pthread_getschedparam(this_thread, &policy, &params);
        if (ret != 0) {
            // Couldn't retrieve normal scheduling paramers
            return false;
        }
        // Check the correct policy was applied
        if(policy != SCHED_OTHER) {
            // Scheduling is NOT SCHED_OTHER!
            return false;
        }
        else {
            // Success
            return true;
        }
    #elif _WIN32
        DWORD dwError;
        if (!SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS)) {
            dwError = GetLastError();
            _tprintf(TEXT("ERROR: Failed to elevate process priority (%d)\n"), static_cast<int>(dwError));;
            return false;
        }
        if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL)) {
            dwError = GetLastError();
            _tprintf(TEXT("ERROR: Failed to elevate thread priority (%d)\n"), static_cast<int>(dwError));
            return false;
        }
        return true;
    #endif
}

//==============================================================================
// PEROFRMANCE MONITORING FUNCTIONS (LINUX)
//==============================================================================

#ifdef __linux__

// TODO

//==============================================================================
// PEROFRMANCE MONITORING FUNCTIONS (WINDOWS)
//==============================================================================

#elif _WIN32

// for CPU usage total
static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;

// for CPU usage process
static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

// We need to initialize a few things for Windows functions, so we create a
// simple class with the the init code in its constructor and create an isntance
struct PerformanceInitializer {
    PerformanceInitializer() {
        // for CPU usage total
        PdhOpenQuery(0, 0, &cpuQuery);
        PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuTotal);
        PdhCollectQueryData(cpuQuery);
        // for CPU usage process
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
    ~PerformanceInitializer() { }
};

// create an instance, calls the init code in constructor
PerformanceInitializer global_initializer;

double cpu_usage_total() {
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    return counterVal.doubleValue;
}

double cpu_usage_process() {
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (double)((sys.QuadPart - lastSysCPU.QuadPart) +
        (user.QuadPart - lastUserCPU.QuadPart));
    percent /= (now.QuadPart - lastCPU.QuadPart);
    percent /= numProcessors;
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    return percent * 100;
}

uint64 virt_mem_available() {
    MEMORYSTATUSEX mem_info;
    mem_info.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&mem_info);
    return mem_info.ullTotalPageFile;
}

uint64 virt_mem_used_total() {
    MEMORYSTATUSEX mem_info;
    mem_info.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&mem_info);
    return mem_info.ullTotalPageFile - mem_info.ullAvailPageFile;
}

uint64 virt_mem_used_process() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.PrivateUsage;
}

uint64 ram_available() {
    MEMORYSTATUSEX mem_info;
    mem_info.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&mem_info);
    return mem_info.ullTotalPhys;
}

uint64 ram_used_total() {
    MEMORYSTATUSEX mem_info;
    mem_info.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&mem_info);
    return mem_info.ullTotalPhys - mem_info.ullAvailPhys;
}

uint64 ram_used_process() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.WorkingSetSize;
}

#endif

}
