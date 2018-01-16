#include <MEL/Utility/Windows/NamedMutex.hpp>
#include <iostream>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace mel {

//==============================================================================
// PIMPL
//==============================================================================

class NamedMutex::Impl : NonCopyable {
public:
    void create(const std::string& name);
    void open(std::string name);
    void close();
    void lock();
    void unlock();
private:
    #ifdef _WIN32
    HANDLE mutex_;
    #endif
};

#ifdef _WIN32

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

void NamedMutex::close(NamedMutexHandle mutex) {
    CloseHandle(mutex);
}

void NamedMutex::lock(NamedMutexHandle mutex) {
    if (mutex != NULL) {
        DWORD dwWaitStatus;
        dwWaitStatus = WaitForSingleObject(mutex, INFINITE);
        switch (dwWaitStatus) {
        case WAIT_OBJECT_0:
            return;
        case WAIT_ABANDONED:
            std::cout << "ERROR: Wait on mutex abandoned." << std::endl;
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return;
        case WAIT_TIMEOUT:
            std::cout << "ERROR: Wait on mutex timed out." << std::endl;
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return;
        case WAIT_FAILED:
            std::cout << "ERROR: Wait on mutex failed." << std::endl;
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return;
        }
    }
    else {
        std::cout << "ERROR: Mutex is invalid." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
    }
}

void NamedMutex::unlock(NamedMutexHandle mutex) {
    if (!ReleaseMutex(mutex)) {
        if (GetLastError() != ERROR_NOT_OWNER) {
            std::cout << "ERROR: Failed to unlock mutex." << std::endl;
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
        }
    }
}

#endif

//==============================================================================
// CLASS DECLARATIONS
//==============================================================================

NamedMutex::NamedMutex(std::string name, NamedMutex::Mode mode) :
    name_(name)
{
    impl_ = new NamedMutex::Impl;
    switch(mode) {
        case Create:
            mutex_ = create(name_);
            break;
        case Open:
            mutex_ = open(name_);
            break;
    }
}

NamedMutex::~NamedMutex() {
    unlock(mutex_);
    close(mutex_);
}

void NamedMutex::lock() {
    lock(mutex_);
}

void NamedMutex::unlock() {
    unlock(mutex_);
}

} // namespace mel
