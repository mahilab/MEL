#include "util.h"
#include "Windows.h"
#include <tchar.h>

namespace mel {

    const std::string get_ymdhms() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d-%H.%M.%S", &tstruct);
        return buf;
    }

    std::string get_last_windows_error_message(){
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


    std::vector<double> eigenv2stdv(Eigen::VectorXd& eigen_vec) {
        std::vector<double> std_vec;
        std_vec.resize(eigen_vec.size());
        Eigen::VectorXd::Map(&std_vec[0], eigen_vec.size()) = eigen_vec;
        return std_vec;
    }

    Eigen::VectorXd stdv2eigenv(std::vector<double>& std_vec) {
        Eigen::Map<Eigen::VectorXd> eigen_vec(&std_vec[0], std_vec.size());
        return eigen_vec;
    }

    void enable_soft_realtime() {
        DWORD dwError;
        if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
        {
            dwError = GetLastError();
            _tprintf(TEXT("Failed to elevate process priority (%d)\n"), dwError);
        }
        if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
        {
            dwError = GetLastError();
            _tprintf(TEXT("Failed to elevate thread priority (%d)\n"), dwError);
        }

    }

    

    std::string namify(std::string name) {
        return "<" + name + ">";
    }

    double sin_wave(double amplitude, double frequency, double t) {
        return amplitude * sin(2 * PI * frequency * t);
    }

    double cos_wave(double amplitude, double frequency, double t) {
        return amplitude * cos(2 * PI * frequency * t);
    }

    double square_wave(double amplitude, double frequency, double t) {
        double sin = sin_wave(1, frequency, t);
        if (sin == 0)
            return 0;
        else if (sin > 0)
            return amplitude;
        else if (sin < 0)
            return -amplitude;
    }

    double triangle_wave(double amplitude, double frequency, double t) {
        return 2 * amplitude / PI * asin(sin(2 * PI * frequency * t));
    }

    double sawtooth_wave(double amplitude, double frequency, double t) {
        return -2 * amplitude / PI * atan(cos(PI * frequency * t) / sin(PI * frequency * t));
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


    double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd) {
        return kp * (x_ref - x) + kd * (xd_ref - xd);
    }

    double saturate(double value, double max, double min) {
        if (value > max)
            return max;
        else if (value < min)
            return min;
        else
            return value;
    }

    double saturate(double value, double abs_max) {
        abs_max = abs(abs_max);
        return saturate(value, abs_max, -abs_max);
    }

}

