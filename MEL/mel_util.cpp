#include "mel_util.h"
#include "Windows.h"
#include <tchar.h>
#include <iomanip>

namespace mel {

    namespace util {

        UINT timer_resolution;

        const std::string get_ymdhms() {
            time_t     now = time(0);
            struct tm  tstruct;
            char       buf[80];
            tstruct = *localtime(&now);
            strftime(buf, sizeof(buf), "%Y-%m-%d-%H.%M.%S", &tstruct);
            return buf;
        }

        std::string get_last_windows_error_message() {
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
        }

        void enable_realtime() {
            DWORD dwError;
            if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
            {
                dwError = GetLastError();
                _tprintf(TEXT("ERROR: Failed to elevate process priority (%d)\n"), dwError);
            }
            if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
            {
                dwError = GetLastError();
                _tprintf(TEXT("ERROR: Failed to elevate thread priority (%d)\n"), dwError);
            }

            // set Windows timer resolution to smallest possible
            TIMECAPS tc;            

            if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
                util::print("ERROR: Failed to get minimum timer resolution.");
            }

            timer_resolution = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);

            // begin high resolution timer period
            if (timeBeginPeriod(timer_resolution) != TIMERR_NOERROR) {
                util::print("ERROR: Failed to set Windows timer resolution.");
            }
        }

        void disable_realtime() {
            DWORD dwError;
            if (!SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS))
            {
                dwError = GetLastError();
                _tprintf(TEXT("ERROR: Failed to elevate process priority (%d)\n"), dwError);
            }
            if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL))
            {
                dwError = GetLastError();
                _tprintf(TEXT("ERROR: Failed to elevate thread priority (%d)\n"), dwError);
            }

            // end high resolution timer period
            if (timer_resolution != 0) {
                if (timeEndPeriod(timer_resolution) != TIMERR_NOERROR) {
                    util::print("ERROR: Failed to set Windows timer resolution.");
                }
            }
        }

        void usleep(int64 microseconds) {
            // https://stackoverflow.com/questions/13397571/precise-thread-sleep-needed-max-1ms-error
            // https://msdn.microsoft.com/en-us/library/windows/desktop/dd743626(v=vs.85).aspx
            // https://stackoverflow.com/questions/5801813/c-usleep-is-obsolete-workarounds-for-windows-mingw/11470617#11470617

            // create a waitable timer
            HANDLE timer;
            LARGE_INTEGER ft;
            ft.QuadPart = -(10 * microseconds);
            timer = CreateWaitableTimer(NULL, TRUE, NULL);
            SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);

            // wait
            WaitForSingleObject(timer, INFINITE);
            CloseHandle(timer);
        }

        std::string namify(std::string name) {
            return "<" + name + ">";
        }

        /*double_vec mat_vec_multiply(double_mat A, double_vec b) {
            double_vec c(A.size(),0);
            if (A[0].size() == b.size()) {
                for (int i = 0; i < A.size(); ++i) {
                    for (int j = 0; j < A[i].size(); ++j) {
                        c[i] += A[i][j] * b[j];
                    }
                }
            }
            else {
                print("FATAL ERROR: inner dimensions must match");
            }
            return c;
        }*/

    }

}

