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

namespace mel {

    namespace shr {

        // base read/write template functions from which all others are derived

        template <typename T>
        int read_map(char* name, T* buffer, int size,
            boost::interprocess::windows_shared_memory& shm,
            boost::interprocess::mapped_region& region,
            std::wstring mutex_name)
        {
            volatile T* temp;
            HANDLE mutex;
            mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name.c_str());
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
                        return -6;
                    }
                    if (!CloseHandle(mutex)) {
                        return -7;
                    }
                    return 1;
                case WAIT_ABANDONED:
                    return -3;
                case WAIT_TIMEOUT:
                    return -4;
                case WAIT_FAILED:
                    return -5;
                }
            }
            else {
                return -2;
            }
        }

        template <typename T>
        int write_map(char* name, T* buffer, int size,
            boost::interprocess::windows_shared_memory& shm,
            boost::interprocess::mapped_region& region,
            std::wstring mutex_name)
        {
            volatile T* temp;
            HANDLE mutex;
            mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name.c_str());
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
                        return -6;
                    }
                    if (!CloseHandle(mutex)) {
                        return -7;
                    }
                    return 1;
                case WAIT_ABANDONED:
                    return -3;
                case WAIT_TIMEOUT:
                    return -4;
                case WAIT_FAILED:
                    return -5;
                }
            }
            else {
                return -2;
            }
        }

        // base read/write template functions that open a new instance of the shared memory map

        template <typename T>
        int read_map(char* name, T* buffer, int size) {
            try {
                boost::interprocess::windows_shared_memory shm(boost::interprocess::open_only, name, boost::interprocess::read_only);
                boost::interprocess::mapped_region region(shm, boost::interprocess::read_only);
                std::string mutex_name_s = std::string(name) + "_mutex";
                std::wstring mutex_name = std::wstring(mutex_name_s.begin(), mutex_name_s.end());
                return read_map(name, buffer, size, shm, region, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                return -1;
            }
        }

        template <typename T>
        int write_map(char* name, T* buffer, int size) {
            try {
                boost::interprocess::windows_shared_memory shm(boost::interprocess::open_only, name, boost::interprocess::read_write);
                boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
                std::string mutex_name_s = std::string(name) + "_mutex";
                std::wstring mutex_name = std::wstring(mutex_name_s.begin(), mutex_name_s.end());
                return write_map(name, buffer, size, shm, region, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
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
            std::wstring mutex_name_;
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