#pragma once
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <vector>
#include <chrono>
#include <Windows.h>

namespace mel {

    namespace ip = boost::interprocess;
    
    class MelShare {

    public:

        MelShare(char* name, unsigned int bytes = 256);
        MelShare(const MelShare& other) {
            name_ = other.name_;
            bytes_ = other.bytes_;
            shm_ = ip::windows_shared_memory(ip::open_or_create, name_, ip::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);
            mutex_name_ = other.mutex_name_;
            mutex_name_w_ = other.mutex_name_w_;
            mutex_ = other.mutex_;
        }
        MelShare& operator=(const MelShare& other) {
            name_ = other.name_;
            bytes_ = other.bytes_;
            shm_ = ip::windows_shared_memory(ip::open_or_create, name_, ip::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);
            mutex_name_ = other.mutex_name_;
            mutex_name_w_ = other.mutex_name_w_;
            mutex_ = other.mutex_;
        }
        ~MelShare();


        // NON-STATIC READ/WRITE FUNCTIONS (CALLED ONLY FROM AN INSTANCE)

        template<typename T>
        int read(T* buffer, int size) {
            HANDLE mutex;
            mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name_w_.c_str());
            DWORD dwWaitResult;
            volatile T* temp;
            if (mutex != NULL) {
                dwWaitResult = WaitForSingleObject(mutex, INFINITE); // waits 1ms for the mutex to unlock
                switch (dwWaitResult) {
                case WAIT_OBJECT_0:
                    try {
                        temp = static_cast<T*>(region_.get_address());
                        for (int i = 0; i < size; i++) {
                            buffer[i] = temp[i];
                        }
                        if (!ReleaseMutex(mutex)) {
                            std::cout << "ERROR: Failed to release mutex <" << name_ << ">." << std::endl;
                            return -6;
                        }
                        if (!CloseHandle(mutex)) {
                            std::cout << "ERROR: Failed to close mutex <" << name_ << "> handle." << std::endl;
                            return -7;
                        }
                        return 1;
                    }
                    catch (ip::interprocess_exception &ex) {
                        std::cout << "ERROR: Failed to open MelShare <" << name_ << ">." << std::endl;
                        return -1;
                    }
                case WAIT_ABANDONED:
                    std::cout << "ERROR: Wait on mutex <" << name_ << "> abandoned." << std::endl;
                    return -3;
                case WAIT_TIMEOUT:
                    std::cout << "ERROR: Wait on mutex <" << name_ << "> timed out." << std::endl;
                    return -4;
                case WAIT_FAILED:
                    std::cout << "ERROR: Wait on mutex <" << name_ << "> failed." << std::endl;
                    return -5;
                }
            }
            else {
                std::cout << "ERROR: Failed to open mutex <" << name_ << ">." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
                return -2;
            }
        }

        template<typename T>
        int read(std::vector<T>& buffer) { 
            return read(&buffer[0], buffer.size()); }

        template<typename T>
        int write(T* buffer, int size) {
            HANDLE mutex;
            mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name_w_.c_str());
            DWORD dwWaitResult;
            volatile T* temp;
            if (mutex != NULL) {
                dwWaitResult = WaitForSingleObject(mutex, INFINITE); // waits 1ms for the mutex to unlock
                switch (dwWaitResult) {
                case WAIT_OBJECT_0:
                    try {
                        temp = static_cast<T*>(region_.get_address());
                        for (int i = 0; i < size; i++) {
                            temp[i] = buffer[i];
                        }
                        if (!ReleaseMutex(mutex)) {
                            std::cout << "ERROR: Failed to release mutex <" << name_ << ">." << std::endl;
                            return -6;
                        }
                        if (!CloseHandle(mutex)) {
                            std::cout << "ERROR: Failed to close mutex <" << name_ << "> handle." << std::endl;
                            return -7;
                        }
                        return 1;
                    }
                    catch (ip::interprocess_exception &ex) {
                        std::cout << "ERROR: Failed to open MelShare <" << name_ << ">." << std::endl;
                        return -1;
                    }
                case WAIT_ABANDONED:
                    std::cout << "ERROR: Wait on mutex <" << name_ << "> abandoned." << std::endl;
                    return -3;
                case WAIT_TIMEOUT:
                    std::cout << "ERROR: Wait on mutex <" << name_ << "> timed out." << std::endl;
                    return -4;
                case WAIT_FAILED:
                    std::cout << "ERROR: Wait on mutex <" << name_ << "> failed." << std::endl;
                    return -5;
                }
            }
            else {
                std::cout << "ERROR: Failed to open mutex <" << name_ << ">." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
                return -2;
            }
        }

        template<typename T>
        int write(std::vector<T>& buffer) { 
            return write(&buffer[0], buffer.size()); }

    private:

        char* name_;
        unsigned int bytes_;
        ip::windows_shared_memory shm_;
        ip::mapped_region region_;
        //int(*values_)[16];
        int* values_;
        std::string mutex_name_;
        std::wstring mutex_name_w_;
        HANDLE mutex_ = NULL;

    public:

            // STATIC READ/WRITE FUNCTIONS (CAN BE CALLED ANYWHERE)

            template <typename T>
            static int read_map(const char* name, T* buffer, int size) {
                std::string mutex_name = std::string(name) + "_mutex";
                std::wstring mutex_name_w = std::wstring(mutex_name.begin(), mutex_name.end());
                HANDLE mutex;
                mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name_w.c_str());
                DWORD dwWaitResult;
                volatile T* temp;
                if (mutex != NULL) {
                    dwWaitResult = WaitForSingleObject(mutex, INFINITE); // waits 1ms for the mutex to unlock
                    switch (dwWaitResult) {
                    case WAIT_OBJECT_0:
                        try {
                            ip::windows_shared_memory shm(ip::open_only, name, ip::read_write);
                            ip::mapped_region region(shm, ip::read_write);
                            temp = static_cast<T*>(region.get_address());
                            for (int i = 0; i < size; i++) {
                                buffer[i] = temp[i];
                            }
                            if (!ReleaseMutex(mutex)) {
                                std::cout << "ERROR: Failed to release mutex <" << name << ">." << std::endl;
                                return -6;
                            }
                            if (!CloseHandle(mutex)) {
                                std::cout << "ERROR: Failed to close mutex <" << name << "> handle." << std::endl;
                                return -7;
                            }
                            return 1;
                        }
                        catch (ip::interprocess_exception &ex) {
                            std::cout << "ERROR: Failed to open MelShare <" << name << ">." << std::endl;
                            return -1;
                        }
                    case WAIT_ABANDONED:
                        std::cout << "ERROR: Wait on mutex <" << name << "> abandoned." << std::endl;
                        return -3;
                    case WAIT_TIMEOUT:
                        std::cout << "ERROR: Wait on mutex <" << name << "> timed out." << std::endl;
                        return -4;
                    case WAIT_FAILED:
                        std::cout << "ERROR: Wait on mutex <" << name << "> failed." << std::endl;
                        return -5;
                    }
                }
                else {
                    std::cout << "ERROR: Failed to open mutex <" << name << ">." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                    return -2;
                }
            }

            template <typename T>
            static int read_map(const char* name, std::vector<T>& buffer) {
                return read_map(name, &buffer[0], buffer.size());
            }

            template <typename T>
            static int write_map(const char* name, T* buffer, int size) {
                std::string mutex_name = std::string(name) + "_mutex";
                std::wstring mutex_name_w = std::wstring(mutex_name.begin(), mutex_name.end());
                HANDLE mutex;
                mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name_w.c_str());
                DWORD dwWaitResult;
                volatile T* temp;
                if (mutex != NULL) {
                    dwWaitResult = WaitForSingleObject(mutex, INFINITE); // waits 1ms for the mutex to unlock
                    switch (dwWaitResult) {
                    case WAIT_OBJECT_0:
                        try {
                            ip::windows_shared_memory shm(ip::open_only, name, ip::read_write);
                            ip::mapped_region region(shm, ip::read_write);
                            temp = static_cast<T*>(region.get_address());
                            for (int i = 0; i < size; i++) {
                                temp[i] = buffer[i];
                            }
                            if (!ReleaseMutex(mutex)) {
                                std::cout << "ERROR: Failed to release mutex <" << name << ">." << std::endl;
                                return -6;
                            }
                            if (!CloseHandle(mutex)) {
                                std::cout << "ERROR: Failed to close mutex <" << name << "> handle." << std::endl;
                                return -7;
                            }
                            return 1;
                        }
                        catch (ip::interprocess_exception &ex) {
                            std::cout << "ERROR: Failed to open MelShare <" << name << ">." << std::endl;
                            return -1;
                        }
                    case WAIT_ABANDONED:
                        std::cout << "ERROR: Wait on mutex <" << name << "> abandoned." << std::endl;
                        return -3;
                    case WAIT_TIMEOUT:
                        std::cout << "ERROR: Wait on mutex <" << name << "> timed out." << std::endl;
                        return -4;
                    case WAIT_FAILED:
                        std::cout << "ERROR: Wait on mutex <" << name << "> failed." << std::endl;
                        return -5;
                    }
                }
                else {
                    std::cout << "ERROR: Failed to open mutex <" << name << ">." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                    return -2;
                }
            }

            template <typename T>
            static int  write_map(const char* name, std::vector<T>& buffer) {
                return write_map(name, &buffer[0], buffer.size());
            }

    };

}