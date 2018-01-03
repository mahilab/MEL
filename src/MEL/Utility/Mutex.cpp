#include <MEL/Utility/Mutex.hpp>
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

Mutex::Mutex(std::string name, Mutex::Mode mode) :
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

Mutex::~Mutex() {
    if (has_lock_)
        release(mutex_);
    close(mutex_);
}

Mutex::Status Mutex::try_lock() {
    Mutex::Status status = try_lock(mutex_);
    if (status == LockAquired)
        has_lock_ = true;
    return status;
}

Mutex::Status Mutex::release() {
    Mutex::Status status = release(mutex_);
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


MutexHandle Mutex::create(std::string name) {
    MutexHandle handle;
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

MutexHandle Mutex::open(std::string name) {
    MutexHandle handle;
    handle = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, name.c_str());
    if (handle == NULL) {
        std::cout << "ERROR: Failed to open mutex <" << name << ">." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
    }
    return handle;
}

Mutex::Status Mutex::close(MutexHandle mutex) {
    if (CloseHandle(mutex))
        return Status::CloseSuccess;
    else
        return Status::CloseFailed;
}

Mutex::Status Mutex::try_lock(MutexHandle mutex) {
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

Mutex::Status Mutex::release(MutexHandle mutex) {
    if (!ReleaseMutex(mutex)) {
        std::cout << "ERROR: Failed to release mutex." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
        return Status::ReleaseFailed;
    }
    return Status::ReleaseSuccess;
}

#endif

} // namespace mel
