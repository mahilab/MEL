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
    ~Impl();
    void create(const std::string& name);
    void open(const std::string& name);
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

NamedMutex::Impl::~Impl() {
    unlock();
    close();
}

void NamedMutex::Impl::create(const std::string& name) {
    mutex_ = CreateMutexA(NULL, FALSE, name.c_str());
    if (mutex_ == NULL) {
        std::cout << "ERROR: Failed to create mutex <" << name << ">." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
    }
    else {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            // std::cout << "WARNING: Opened an existing mutex when trying to create mutex." << std::endl;
        }
        ReleaseMutex(mutex_);
    }
}

void NamedMutex::Impl::open(const std::string& name) {
    mutex_ = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, name.c_str());
    if (mutex_ == NULL) {
        std::cout << "ERROR: Failed to open mutex <" << name << ">." << std::endl;
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
    }
}

void NamedMutex::Impl::close() {
    CloseHandle(mutex_);
}

void NamedMutex::Impl::lock() {
    if (mutex_ != NULL) {
        DWORD dwWaitStatus;
        dwWaitStatus = WaitForSingleObject(mutex_, INFINITE);
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

void NamedMutex::Impl::unlock() {
    if (!ReleaseMutex(mutex_)) {
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
    name_(name),
    impl_(new NamedMutex::Impl)
{
    switch(mode) {
        case OpenOrCreate:
            impl_->create(name_);
            break;
        case OpenOnly:
            impl_->open(name_);
            break;
    }
}

NamedMutex::~NamedMutex() {
}

void NamedMutex::lock() {
    impl_->lock();
}

void NamedMutex::unlock() {
    impl_->unlock();
}

} // namespace mel
