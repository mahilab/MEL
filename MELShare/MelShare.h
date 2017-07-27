#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <Windows.h>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

#ifdef MELSHARE_EXPORTS
#	define MELSHARE_API __declspec(dllexport)
#else
#	define MELSHARE_API __declspec(dllimport)
#endif

// error codes
// >0 = successful read/write, return value is current map size
//  0   map size is zero
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

        /// Reads the first n = #buffer_size values from mapped region #region into C-style array #buffer. 
        /// If the size of the buffer is greater than the size of the data in the map, extra buffer indices will be ignored.
        template <typename T>
        int read_map(T* buffer, const int buffer_size,
            const boost::interprocess::mapped_region& region_data,
            const boost::interprocess::mapped_region& region_size,
            const std::string& mutex_name)
        {
            volatile int* size;
            volatile T* data;
            std::wstring w_mutex_name = std::wstring(mutex_name.begin(), mutex_name.end());
            HANDLE mutex;
            mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, w_mutex_name.c_str());
            DWORD dwWaitResult;
            if (mutex != NULL) {
                dwWaitResult = WaitForSingleObject(mutex, INFINITE);
                switch (dwWaitResult) {
                case WAIT_OBJECT_0:
                    size = static_cast<int*>(region_size.get_address());  
                    if (buffer_size > 0) {
                        data = static_cast<T*>(region_data.get_address());
                        for (int i = 0; i < min(buffer_size, *size); i++) {
                            buffer[i] = data[i];
                        }
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
                    return *size;
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

        /// Writes n = #buffer_size values into mapped region #region from #buffer.
        template <typename T>
        int write_map(T* buffer, const int buffer_size,
            const boost::interprocess::mapped_region& region_data,
            const boost::interprocess::mapped_region& region_size,
            const std::string& mutex_name)
        {
            volatile int* size;
            volatile T* data;
            std::wstring w_mutex_name = std::wstring(mutex_name.begin(), mutex_name.end());
            HANDLE mutex;
            mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, w_mutex_name.c_str());
            DWORD dwWaitResult;
            if (mutex != NULL) {
                dwWaitResult = WaitForSingleObject(mutex, INFINITE);
                switch (dwWaitResult) {
                case WAIT_OBJECT_0:
                    size = static_cast<int*>(region_size.get_address());
                    if (buffer_size > 0) {
                        data = static_cast<T*>(region_data.get_address());
                        *size = buffer_size;
                        for (int i = 0; i < buffer_size; i++) {
                            data[i] = buffer[i];
                        }
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
                    return *size;
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



        // base read/write template functions that open a new instance of the shared memory map and read/write from C-style array

        /// Opens shared memory map #name and reads n = #buffer_size values into C-style array #buffer
        template <typename T>
        int read_map(const std::string& name, T* buffer, const int buffer_size) {
            try {
                std::string name_size = name + "_size";
                std::string mutex_name = name + "_mutex";
                boost::interprocess::windows_shared_memory shm_data(boost::interprocess::open_only, name.c_str(), boost::interprocess::read_only);
                boost::interprocess::windows_shared_memory shm_size(boost::interprocess::open_only, name_size.c_str(), boost::interprocess::read_only);
                boost::interprocess::mapped_region region_data(shm_data, boost::interprocess::read_only);
                boost::interprocess::mapped_region region_size(shm_size, boost::interprocess::read_only);

                return read_map(buffer, buffer_size, region_data, region_size, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to open shared memory map <" << name << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
                return -1;
            }
        }

        /// Opens shared memory map #name and writes n = #buffer_size values from C-style array #buffer
        template <typename T>
        int write_map(const std::string& name, T* buffer, const int buffer_size) {
            try {
                std::string name_size = name + "_size";
                std::string mutex_name = name + "_mutex";
                boost::interprocess::windows_shared_memory shm_data(boost::interprocess::open_only, name.c_str(), boost::interprocess::read_write);
                boost::interprocess::windows_shared_memory shm_size(boost::interprocess::open_only, name_size.c_str(), boost::interprocess::read_write);
                boost::interprocess::mapped_region region_data(shm_data, boost::interprocess::read_write);
                boost::interprocess::mapped_region region_size(shm_size, boost::interprocess::read_write);
                return write_map(buffer, buffer_size, region_data, region_data, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to open shared memory map <" << name << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
                return -1;
            }
        }

        // base read/write template functions that open a new instance of the shared memory map and accept a STL vector or array

        /// Opens shared memory map #name and reads n = buffer.size() values into STL vector #buffer
        template <typename T>
        int read_map(const std::string& name, std::vector<T>& buffer) {
            return read_map(name, &buffer[0], buffer.size());
        }

        /// Opens shared memory map #name and reads n = buffer.size() values into STL array #buffer
        template <typename T, std::size_t N>
        int read_map(const std::string& name, std::array<T, N>& buffer) {
            return read_map(name, &buffer[0], buffer.size());
        }

        /// Opens shared memory map #name and writes n = buffer.size() values from STL vector #buffer
        template <typename T>
        int write_map(const std::string& name, std::vector<T>& buffer) {
            return write_map(name, &buffer[0], buffer.size());
        }

        /// Opens shared memory map #name and writes n = buffer.size() values from STL array #buffer
        template <typename T, std::size_t N>
        int write_map(const std::string& name, std::array<T, N>& buffer) {
            return write_map(name, &buffer[0], buffer.size());
        }

        // wraps memory maps, regions, mutexes, and functions into a convienient class for the C++ interface

        class MELSHARE_API MelShare {

        public:

            MelShare(std::string name, unsigned int bytes = 256);
            MelShare(const MelShare& other);
            MelShare& operator=(const MelShare& other);
            ~MelShare();

            template<typename T>
            int read(T* buffer, int buffer_size) {
                return read_map(buffer, buffer_size, region_data_, region_size_, mutex_name_);
            }

            template<typename T>
            int read(std::vector<T>& buffer) {
                return read(&buffer[0], buffer.size());
            }

            template <typename T, std::size_t N>
            int read(std::array<T, N>& buffer) {
                return read(&buffer[0], buffer.size());
            }

            template<typename T>
            int write(T* buffer, int buffer_size) {
                return write_map(buffer, buffer_size, region_data_, region_size_, mutex_name_);
            }

            template<typename T>
            int write(std::vector<T>& buffer) {
                return write(&buffer[0], buffer.size());
            }

            template <typename T, std::size_t N>
            int write(std::array<T, N>& buffer) {
                return write(&buffer[0], buffer.size());
            }

            int get_size() {
                int* empty = nullptr;
                return read_map(empty, 0, region_data_, region_size_, mutex_name_);
            }

            std::string name_;

        private:

            const char* name_data_;
            const char* name_size_;
            unsigned int bytes_data_;
            unsigned int bytes_size_;
            boost::interprocess::windows_shared_memory shm_data_;
            boost::interprocess::windows_shared_memory shm_size_;
            boost::interprocess::mapped_region region_data_;
            boost::interprocess::mapped_region region_size_;
            std::string mutex_name_;
            HANDLE mutex_ = NULL;

        };

        // C Linkages for Python and C#

        extern "C" {
            MELSHARE_API int get_map_size(char* name);
        }

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