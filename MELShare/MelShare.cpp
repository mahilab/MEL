#include "MelShare.h"

namespace ip = boost::interprocess;

// error codes
//  1 = successful read/write
// -1 = failed to open shared memory map
// -2 = failed to open mutex
// -3 = wait on mutex abandoned
// -4 = wait on mutex timed out
// -5 = wait on mutex failed
// -6 = failed to release mutex
// -7 = failed to close mutex handle

template <typename T>
int read_map(char* name, T* buffer, int size) {
    std::string mutex_name = std::string(name) + "_mutex";
    std::wstring mutex_name_w = std::wstring(mutex_name.begin(), mutex_name.end());
    HANDLE mutex;
    mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name_w.c_str());
    DWORD dwWaitResult;
    volatile T* temp;
    if (mutex != NULL) {
        dwWaitResult = WaitForSingleObject(mutex, INFINITE); 
        switch (dwWaitResult) {
        case WAIT_OBJECT_0:
            try {
                ip::windows_shared_memory shm(ip::open_only, name, ip::read_only);
                ip::mapped_region region(shm, ip::read_only);
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
            }
            catch (ip::interprocess_exception &ex) {
                return -1;
            }
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
int write_map(char* name, T* buffer, int size) {
    std::string mutex_name = std::string(name) + "_mutex";
    std::wstring mutex_name_w = std::wstring(mutex_name.begin(), mutex_name.end());
    HANDLE mutex;
    mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name_w.c_str());
    DWORD dwWaitResult;
    volatile T* temp;
    if (mutex != NULL) {
        dwWaitResult = WaitForSingleObject(mutex, INFINITE);
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
                    return -6;
                }
                if (!CloseHandle(mutex)) {
                    return -7;
                }
                return 1;
            }
            catch (ip::interprocess_exception &ex) {
                return -1;
            }
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

int read_char_map(char* name, char* buffer, int size) {
    return read_map(name, buffer, size);
}

int read_int_map(char* name, int* buffer, int size) {
    return read_map(name, buffer, size);
}

int read_double_map(char* name, double* buffer, int size) {
    return read_map(name, buffer, size);
}

int write_char_map(char* name, char* buffer, int size) {
    return write_map(name, buffer, size);
}

int write_int_map(char* name, int* buffer, int size) {
    return write_map(name, buffer, size);
}

int write_double_map(char* name, double* buffer, int size) {
    return write_map(name, buffer, size);
}

namespace mel {

    MelShare::MelShare(char* name, unsigned int bytes) : name_(name), bytes_(bytes) {
        try {
            // create a new windows shared memory 
            // (note this MelShare must remain in scope for as long as the shared memory needs to be accessed)
            shm_ = ip::windows_shared_memory(ip::open_or_create, name_, ip::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);

            //values_ = (int(*)[16])(region_.get_address());
            //values_ = new int[size_];
            //values_ = (int*)(region_.get_address());

            mutex_name_ = std::string(name) + "_mutex";
            mutex_name_w_ = std::wstring(mutex_name_.begin(), mutex_name_.end());
            mutex_ = CreateMutex(NULL, FALSE, mutex_name_w_.c_str());
            if (mutex_ == NULL) {
                std::cout << "ERROR: Failed to create mutex <" << name << ">." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
            }
            else {
                ReleaseMutex(mutex_);
            }
        }
        catch (ip::interprocess_exception &ex) {
            std::cout << "ERROR: Failed to create or open shared memory <" << name_ << ">." << std::endl;
        }
    }

    MelShare::~MelShare() {
        ReleaseMutex(mutex_);
        CloseHandle(mutex_);
    }


}