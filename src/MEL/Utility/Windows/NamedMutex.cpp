#include <MEL/Utility/Windows/NamedMutex.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>

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
                LOG(Error) << "Failed to create NamedMutex " << name << "(Windows Error #" << (int)GetLastError() << ")";
            }
            else {
                if (GetLastError() == ERROR_ALREADY_EXISTS)
                    LOG(Warning) << "Opened an existing mutex when trying to create NamedMutex " << name;
            }
            break;
        case OpenOnly:
            mutex_ = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, name.c_str());
            if (mutex_ == NULL) {
                LOG(Error) << "Failed to open NamedMutex " << name << "(Windows Error #" << (int)GetLastError() << ")";
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
            LOG(Error) << "Wait on NamedMutex abandoned (Windows Error #" << (int)GetLastError() << ")";
            return;
        case WAIT_TIMEOUT:
            LOG(Error) << "Wait on NamedMutex timed out (Windows Error #" << (int)GetLastError() << ")";
            return;
        case WAIT_FAILED:
            LOG(Error) << "Wait on NamedMutex failed (Windows Error #" << (int)GetLastError() << ")";
            return;
        }
    }
    else {
        LOG(Error) << "NamedMutex is invalid (Windows Error #" << (int)GetLastError() << ")";
    }
}

void NamedMutex::Impl::unlock() {
    if (!ReleaseMutex(mutex_)) {
        if (GetLastError() != ERROR_NOT_OWNER) {
            LOG(Error) << "Failed to unlock NamedMutex (Windows Error #" << (int)GetLastError() << ")";
        }
    }
}

#endif

//==============================================================================
// CLASS DECLARATIONS
//==============================================================================

NamedMutex::NamedMutex(std::string name, NamedMutex::Mode mode) :
    impl_(new NamedMutex::Impl(name, mode)),
    name_(name)
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
