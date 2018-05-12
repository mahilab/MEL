#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/NamedMutex.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
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
#else
    pthread_mutex_t* ptr_;
    int shm_fd_;
    std::string name_;
    int created_;
#endif
};

#ifdef _WIN32

//==============================================================================
// WINDOWS IMPLEMENTATION
//==============================================================================

NamedMutex::Impl::Impl(const std::string& name, NamedMutex::Mode mode) {
    switch (mode) {
        case OpenOrCreate:
            mutex_ = CreateMutexA(NULL, FALSE, name.c_str());
            if (mutex_ == NULL) {
                LOG(Error) << "Failed to create NamedMutex " << name
                           << "(Windows Error #" << (int)GetLastError() << ")";
            } else {
                if (GetLastError() == ERROR_ALREADY_EXISTS) {
                    LOG(Warning) << "Opened an existing mutex when trying to "
                                    "create NamedMutex "
                                 << name;
                }
            }
            break;
        case OpenOnly:
            mutex_ = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, name.c_str());
            if (mutex_ == NULL) {
                LOG(Error) << "Failed to open NamedMutex " << name
                           << "(Windows Error #" << (int)GetLastError() << ")";
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
                LOG(Error) << "Wait on NamedMutex abandoned (Windows Error #"
                           << (int)GetLastError() << ")";
                return;
            case WAIT_TIMEOUT:
                LOG(Error) << "Wait on NamedMutex timed out (Windows Error #"
                           << (int)GetLastError() << ")";
                return;
            case WAIT_FAILED:
                LOG(Error) << "Wait on NamedMutex failed (Windows Error #"
                           << (int)GetLastError() << ")";
                return;
        }
    } else {
        LOG(Error) << "NamedMutex is invalid (Windows Error #"
                   << (int)GetLastError() << ")";
    }
}

void NamedMutex::Impl::unlock() {
    if (!ReleaseMutex(mutex_)) {
        if (GetLastError() != ERROR_NOT_OWNER) {
            LOG(Error) << "Failed to unlock NamedMutex (Windows Error #"
                       << (int)GetLastError() << ")";
        }
    }
}

#else

//==============================================================================
// UNIX IMPLEMENTATION
//==============================================================================

// https://gist.github.com/yamnikov-oleg/abf61cf96b4867cbf72d

NamedMutex::Impl::Impl(const std::string& name, NamedMutex::Mode mode)
    : ptr_(nullptr), created_(0), name_(name) {
    errno = 0;
    switch (mode) {
        case OpenOrCreate: {
            // try to open existing mutex shared memory
            shm_fd_ = shm_open(name_.c_str(), O_RDWR, 0660);
            if (errno == ENOENT) {
                // create new mutex shared memory
                shm_fd_ = shm_open(name_.c_str(), O_RDWR | O_CREAT, 0660);
                created_ = 1;
                if (ftruncate(shm_fd_, sizeof(pthread_mutex_t)) != 0) {
                    LOG(Error) << "Could not truncate file mapping object "
                               << name_ << " (Error # " << errno << " - "
                               << strerror(errno) << ")";
                }
            }
            if (shm_fd_ == -1) {
                LOG(Error) << "Could not create file mapping object " << name_
                           << " (Error # " << errno << " - " << strerror(errno) << ")";
                break;
            }
            void* addr = mmap(NULL, sizeof(pthread_mutex_t),
                              PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);
            if (addr == MAP_FAILED) {
                LOG(Error) << "Could not map view of file"
                           << " (Error #" << errno << " - "
                           << strerror(errno) << ")";
                break;
            }
            pthread_mutex_t* mutex_ptr = (pthread_mutex_t*)addr;
            if (created_) {
                pthread_mutexattr_t attributes;
                pthread_mutexattr_init(&attributes);
                pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED);
                pthread_mutex_init(mutex_ptr, &attributes);
            }
            ptr_ = mutex_ptr;
            break;
        }
        case OpenOnly: {
            shm_fd_ = shm_open(name_.c_str(), O_RDWR, 0660);
            if (shm_fd_ == -1) {
                LOG(Error) << "Could not create file mapping object " << name_
                           << " (Error #" << errno << " - " << strerror(errno) << ")";
                break;
            }
            void* addr = mmap(NULL, sizeof(pthread_mutex_t),
                              PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);
            if (addr == MAP_FAILED) {
                LOG(Error) << "Could not map view of file"
                           << " (Error #" << errno << " - "
                           << strerror(errno) << ")";
                break;
            }
            pthread_mutex_t* mutex_ptr = (pthread_mutex_t*)addr;
            ptr_ = mutex_ptr;
            break;
        }
    }
}

NamedMutex::Impl::~Impl() {
    if (created_) {
        pthread_mutex_destroy(ptr_);
    }
    if (munmap((void*)ptr_, sizeof(pthread_mutex_t))) {
        LOG(Error) << "Could not unmap view of file"
                   << " (Error #" << errno << " - " << strerror(errno) << ")";
    }
    ptr_ = NULL;
    close(shm_fd_);
    shm_fd_ = 0;
    if (created_) {
        shm_unlink(name_.c_str());
    }
}

void NamedMutex::Impl::lock() {
    pthread_mutex_lock(ptr_);
}

void NamedMutex::Impl::unlock() {
    pthread_mutex_unlock(ptr_);
}

#endif

//==============================================================================
// CLASS DECLARATIONS
//==============================================================================

NamedMutex::NamedMutex(std::string name, NamedMutex::Mode mode)
    : impl_(new NamedMutex::Impl(name, mode)), name_(name) {}

NamedMutex::~NamedMutex() {}

void NamedMutex::lock() {
    impl_->lock();
}

void NamedMutex::unlock() {
    impl_->unlock();
}

}  // namespace mel
