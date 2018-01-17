#include <MEL/Utility/Windows/NamedMutex.hpp>
#include <MEL/Utility/Console.hpp>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace mel {

//==============================================================================
// PIMPL
//==============================================================================

class NamedMutex::Impl : NonCopyable {
public:
    Impl(const std::string& name, NamedMutex::Mode mode);
    ~Impl();
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

NamedMutex::Impl::Impl(const std::string& name, NamedMutex::Mode mode) {
    switch(mode) {
        case OpenOrCreate:
            mutex_ = CreateMutexA(NULL, FALSE, name.c_str());
            if (mutex_ == NULL) {
                print("ERROR: Failed to create mutex <" + name + ">.");
                printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            }
            else {
                if (GetLastError() == ERROR_ALREADY_EXISTS)
                    print("WARNING: Opened an existing mutex when trying to create mutex <" + name + ">.");
                // ReleaseMutex(mutex_);
            }
            break;
        case OpenOnly:
            mutex_ = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, name.c_str());
            if (mutex_ == NULL) {
                print("ERROR: Failed to open mutex <" + name + ">.");
                printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            }
            break;
    }
}

NamedMutex::Impl::~Impl() {
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
            print("ERROR: Wait on mutex abandoned.");
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return;
        case WAIT_TIMEOUT:
            print("ERROR: Wait on mutex timed out.");
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return;
        case WAIT_FAILED:
            print("ERROR: Wait on mutex failed.");
            printf("WINDOWS ERROR: %d\n", (int)GetLastError());
            return;
        }
    }
    else {
        print("ERROR: Mutex is invalid.");
        printf("WINDOWS ERROR: %d\n", (int)GetLastError());
    }
}

void NamedMutex::Impl::unlock() {
    if (!ReleaseMutex(mutex_)) {
        if (GetLastError() != ERROR_NOT_OWNER) {
            print("ERROR: Failed to unlock mutex.");
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
    impl_(new NamedMutex::Impl(name, mode))
{
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
