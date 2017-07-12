#pragma once

#include <iostream>
#include <vector>
#include <Windows.h>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

#ifdef MELSHARE_EXPORTS
#	define MELSHARE_API __declspec(dllexport)
#else
#	define MELSHARE_API __declspec(dllimport)
#endif

// error codes
//  1 = successful read/write
// -1 = failed to open shared memory map
// -2 = failed to open mutex
// -3 = wait on mutex abandoned
// -4 = wait on mutex timed out
// -5 = wait on mutex failed
// -6 = failed to release mutex
// -7 = failed to close mutex handle

// TODO: There seems to be a bug where if a class has a MelShare member, and an instance of that class
// is passed by value, then the mutex for the MelShare ceases to function properly. This issue needs to be
// understood and fixed.

namespace mel {

    namespace share {

        // base read/write template functions from which all others are derived

        template <typename T>
        int read_map(char* name, T* buffer, int size,
            boost::interprocess::windows_shared_memory& shm,
            boost::interprocess::mapped_region& region,
            std::string mutex_name)
        {
            volatile T* temp;
            std::wstring w_mutex_name = std::wstring(mutex_name.begin(), mutex_name.end());
            HANDLE mutex;
            mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, w_mutex_name.c_str());
            DWORD dwWaitResult;
            if (mutex != NULL) {
                dwWaitResult = WaitForSingleObject(mutex, INFINITE);
                switch (dwWaitResult) {
                case WAIT_OBJECT_0:
                    temp = static_cast<T*>(region.get_address());
                    for (int i = 0; i < size; i++) {
                        buffer[i] = temp[i];
                    }
                    if (!ReleaseMutex(mutex)) {
                        std::cout << "ERROR: Failed to release mutex <" << mutex_name << ">." << std::endl;
                        printf("WINDOWS ERROR: %d\n", GetLastError());
                        return -6;
                    }
                    if (!CloseHandle(mutex)) {
                        std::cout << "ERROR: Failed to close mutex <" << mutex_name << "> handle." << std::endl;
                        printf("WINDOWS ERROR: %d\n", GetLastError());
                        return -7;
                    }
                    return 1;
                case WAIT_ABANDONED:
                    std::cout << "ERROR: Wait on mutex <" << mutex_name << "> abandoned." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                    return -3;
                case WAIT_TIMEOUT:
                    std::cout << "ERROR: Wait on mutex <" << mutex_name << "> timed out." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                    return -4;
                case WAIT_FAILED:
                    std::cout << "ERROR: Wait on mutex <" << mutex_name << "> failed." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                    return -5;
                }
            }
            else {
                std::cout << "ERROR: Failed to open mutex <" << mutex_name << ">." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
                return -2;
            }
        }

        template <typename T>
        int write_map(char* name, T* buffer, int size,
            boost::interprocess::windows_shared_memory& shm,
            boost::interprocess::mapped_region& region,
            std::string mutex_name)
        {
            volatile T* temp;
            std::wstring w_mutex_name = std::wstring(mutex_name.begin(), mutex_name.end());
            HANDLE mutex;
            mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, w_mutex_name.c_str());
            DWORD dwWaitResult;
            if (mutex != NULL) {
                dwWaitResult = WaitForSingleObject(mutex, INFINITE);
                switch (dwWaitResult) {
                case WAIT_OBJECT_0:
                    temp = static_cast<T*>(region.get_address());
                    for (int i = 0; i < size; i++) {
                        temp[i] = buffer[i];
                    }
                    if (!ReleaseMutex(mutex)) {
                        std::cout << "ERROR: Failed to release mutex <" << mutex_name << ">." << std::endl;
                        printf("WINDOWS ERROR: %d\n", GetLastError());
                        return -6;
                    }
                    if (!CloseHandle(mutex)) {
                        std::cout << "ERROR: Failed to close mutex <" << mutex_name << "> handle." << std::endl;
                        printf("WINDOWS ERROR: %d\n", GetLastError());
                        return -7;
                    }
                    return 1;
                case WAIT_ABANDONED:
                    std::cout << "ERROR: Wait on mutex <" << mutex_name << "> abandoned." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                    return -3;
                case WAIT_TIMEOUT:
                    std::cout << "ERROR: Wait on mutex <" << mutex_name << "> timed out." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                    return -4;
                case WAIT_FAILED:
                    std::cout << "ERROR: Wait on mutex <" << mutex_name << "> failed." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                    return -5;
                }
            }
            else {
                std::cout << "ERROR: Failed to open mutex <" << mutex_name << ">." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
                return -2;
            }
        }

        // base read/write template functions that open a new instance of the shared memory map

        template <typename T>
        int read_map(char* name, T* buffer, int size) {
            try {
                boost::interprocess::windows_shared_memory shm(boost::interprocess::open_only, name, boost::interprocess::read_only);
                boost::interprocess::mapped_region region(shm, boost::interprocess::read_only);
                std::string mutex_name = std::string(name) + "_mutex";
                return read_map(name, buffer, size, shm, region, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to open shared memory map <" << name << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
                return -1;
            }
        }

        template <typename T>
        int write_map(char* name, T* buffer, int size) {
            try {
                boost::interprocess::windows_shared_memory shm(boost::interprocess::open_only, name, boost::interprocess::read_write);
                boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
                std::string mutex_name = std::string(name) + "_mutex";
                return write_map(name, buffer, size, shm, region, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to open shared memory map <" << name << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
                return -1;
            }
        }

        // base read/write template functions that open a new instance of the shared memory map and accept a stl vector

        template <typename T>
        int read_map(char* name, std::vector<T>& buffer) {
            return read_map(name, &buffer[0], buffer.size());
        }

        template <typename T>
        int write_map(char* name, std::vector<T>& buffer) {
            return write_map(name, &buffer[0], buffer.size());
        }

        // wraps memory maps, regions, mutexes, and functions into a convienient class for the C++ interface

        class MELSHARE_API MelShare {

        public:

            MelShare(char* name, unsigned int bytes = 256);
            MelShare(const MelShare& other);
            MelShare& operator=(const MelShare& other);
            ~MelShare();

            template<typename T>
            int read(T* buffer, int size) {
                return read_map(name_, buffer, size, shm_, region_, mutex_name_);
            }

            template<typename T>
            int read(std::vector<T>& buffer) {
                return read(&buffer[0], buffer.size());
            }

            template<typename T>
            int write(T* buffer, int size) {
                return write_map(name_, buffer, size, shm_, region_, mutex_name_);
            }

            template<typename T>
            int write(std::vector<T>& buffer) {
                return write(&buffer[0], buffer.size());
            }

        private:

            char* name_;
            unsigned int bytes_;
            boost::interprocess::windows_shared_memory shm_;
            boost::interprocess::mapped_region region_;
            std::string mutex_name_;
            HANDLE mutex_ = NULL;

        };

        // C Linkages for Python and C#

        extern "C" {
            MELSHARE_API int read_char_map(char* name, char* buffer, int size);
        }

        extern "C" {
            MELSHARE_API int read_int_map(char* name, int* buffer, int size);
        }

        extern "C" {
            MELSHARE_API int read_double_map(char* name, double* buffer, int size);
        }

        extern "C" {
            MELSHARE_API int write_char_map(char* name, char* buffer, int size);
        }

        extern "C" {
            MELSHARE_API int write_int_map(char* name, int* buffer, int size);
        }

        extern "C" {
            MELSHARE_API int write_double_map(char* name, double* buffer, int size);
        }

    }

}