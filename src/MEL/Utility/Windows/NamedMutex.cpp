#include <MEL/Utility/Windows/NamedMutex.hpp>
#include <iostream>
#include <string>

#ifdef __linux__

#elif _WIN32
    #include <windows.h>
#endif

namespace mel {

//==============================================================================
// COMMON IMPLEMENTATION
//==============================================================================

NamedMutex::NamedMutex(std::string name, NamedMutex::Mode mode) :
    name_(name),
    mode_(mode),
    has_lock_(false)
{
    switch(mode_) {
        case Create:
            mutex_ = create(name_);
            break;
        case Open:
            mutex_ = open(name_);
            break;
    }
}

NamedMutex::~NamedMutex() {
    if (has_lock_)
        unlock(mutex_);
    close(mutex_);
}

NamedMutex::Status NamedMutex::lock() {
    NamedMutex::Status status = lock(mutex_);
    if (status == LockAquired)
        has_lock_ = true;
    return status;
}

NamedMutex::Status NamedMutex::unlock() {
    NamedMutex::Status status = unlock(mutex_);
    if (status == ReleaseSuccess)
        has_lock_ = false;
    return status;
}

#ifdef __linux__

//==============================================================================
// LINUX IMPLEMENTATION
//==============================================================================



#elif _WIN32

//==============================================================================
// WINDOWS IMPLEMENTATION
//==============================================================================

NamedMutexHandle NamedMutex::create(std::string name) {
    NamedMutexHandle handle;
    handle = CreateMutexA(NULL, FALSE, name.c_str());
    if (handle == NULL) {
        std::cout << "ERROR: Failed to create mutex <" << name << ">." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
    }
    else {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            // std::cout << "WARNING: Opened an existing mutex when trying to create mutex." << std::endl;
        }
        ReleaseMutex(handle);
    }
    return handle;
}

NamedMutexHandle NamedMutex::open(std::string name) {
    NamedMutexHandle handle;
    handle = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, name.c_str());
    if (handle == NULL) {
        std::cout << "ERROR: Failed to open mutex <" << name << ">." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
    }
    return handle;
}

NamedMutex::Status NamedMutex::close(NamedMutexHandle mutex) {
    if (CloseHandle(mutex))
        return Status::CloseSuccess;
    else
        return Status::CloseFailed;
}

NamedMutex::Status NamedMutex::lock(NamedMutexHandle mutex) {
    if (mutex != NULL) {
        DWORD dwWaitStatus;
        dwWaitStatus = WaitForSingleObject(mutex, INFINITE);
        switch (dwWaitStatus) {
        case WAIT_OBJECT_0:
            return Status::LockAquired;
        case WAIT_ABANDONED:
            std::cout << "ERROR: Wait on mutex abandoned." << std::endl;
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return Status::LockAbandoned;
        case WAIT_TIMEOUT:
            std::cout << "ERROR: Wait on mutex timed out." << std::endl;
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return Status::LockTimeout;
        case WAIT_FAILED:
            std::cout << "ERROR: Wait on mutex failed." << std::endl;
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return Status::LockFailed;
        }
        return Status::LockFailed;
    }
    else {
        std::cout << "ERROR: Mutex is invalid." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
        return Status::NotOpen;
    }
}

NamedMutex::Status NamedMutex::unlock(NamedMutexHandle mutex) {
    if (!ReleaseMutex(mutex)) {
        std::cout << "ERROR: Failed to unlock mutex." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
        return Status::ReleaseFailed;
    }
    return Status::ReleaseSuccess;
}

#endif

} // namespace mel
