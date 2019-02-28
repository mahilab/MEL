#include <MEL/Communications/SharedMemory.hpp>
#include <MEL/Logging/Log.hpp>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace mel {

//==============================================================================
// COMMON IMPLEMENTATION
//==============================================================================

SharedMemory::SharedMemory(const std::string& name, OpenMode mode, std::size_t size)
    : name_(name),
      size_(size),
      buffer_(nullptr),
      is_mapped_(false)
{
    switch (mode) {
        case OpenOrCreate: {
            if (open_or_create(map_, name_, size_)) {
                buffer_ = map_buffer(map_, size_);
                if (buffer_)
                    is_mapped_ = true;
            }
            break;
        }
        case OpenOnly: {
            if (open_only(map_, name_)) {
                buffer_ = map_buffer(map_, size_);
                if (buffer_)
                    is_mapped_ = true;
            }
            break;
        }
    }
}

SharedMemory::~SharedMemory() {
    unmap_buffer(buffer_, size_);
    close(name_, map_);
}

bool SharedMemory::write(const void* data,
                         std::size_t size,
                         std::size_t offset) {
    if (size + offset > size_) {
        return false;
    }
    std::memcpy(((char*)buffer_) + offset, data, size);
    return true;
}

bool SharedMemory::read(void* data, std::size_t size, std::size_t offset) {
    if (size + offset > size_) {
        return false;
    }
    std::memcpy(data, ((char*)buffer_) + offset, size);
    return true;
}

void* SharedMemory::get_address() const {
    return buffer_;
}

std::string SharedMemory::get_name() const {
    return name_;
}

bool SharedMemory::is_mapped() const {
    return is_mapped_;
}

#ifdef _WIN32

//==============================================================================
// WINDOWS IMPLEMENTATION
//==============================================================================

bool SharedMemory::open_or_create(MapHandle& map, const std::string& name, std::size_t size)
{
    HANDLE hMapFile;
    hMapFile = ::CreateFileMappingA(
        INVALID_HANDLE_VALUE,      // use paging file
        NULL,                      // default security
        PAGE_READWRITE,            // read/write access
        0,                         // maximum object size (high-order DWORD)
        static_cast<DWORD>(size),  // maximum object size (low-order DWORD)
        name.c_str());             // name of mapping object

    if (hMapFile == NULL) {
        LOG(Error) << "Could not create file mapping object " << name
                   << " (Windows Error #" << (int)GetLastError() << ")";
        map = INVALID_HANDLE_VALUE;
        return false;
    }
    map = hMapFile;
    return true;
}

bool SharedMemory::open_only(MapHandle& map, const std::string& name) {
    HANDLE hMapFile;
    hMapFile = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
    if (hMapFile == NULL) {
        LOG(Error) << "Could not open file mapping object " << name
                   << " (Windows Error #" << (int)GetLastError() << ")";
        map = INVALID_HANDLE_VALUE;
        return false;
    }
    map = hMapFile;
    return true;
}

void SharedMemory::close(const std::string& name, MapHandle map) {
    if (::CloseHandle(map) == 0) {
        LOG(Error) << "Failed to close file mapping handle "  << name << " (Windows Error #"
                   << (int)GetLastError() << ")";
    }
    map = INVALID_HANDLE_VALUE;
}

void* SharedMemory::map_buffer(MapHandle map, std::size_t size) {
    void* pBuf;
    pBuf = ::MapViewOfFile(map,                  // handle to map object
                           FILE_MAP_ALL_ACCESS,  // read/write permission
                           0, 0, size);
    if (pBuf == NULL) {
        LOG(Error) << "Could not map view of file (Windows Error #" << (int)GetLastError() << ")";
        return NULL;
    }
    return pBuf;
}

void SharedMemory::unmap_buffer(void* buffer, std::size_t) {
    ::UnmapViewOfFile(buffer);
}

#else

//==============================================================================
// UNIX IMPLEMENTATION
//==============================================================================

bool SharedMemory::open_or_create(MapHandle& map, const std::string& name, std::size_t size) {
    map = shm_open(name.c_str(), O_RDWR, 0666);
    if (map != -1) {
        LOG(Verbose) << "Opened existing file mapping object " << name;
        return true;
    }
    map = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
    if (map == -1) {
        LOG(Error) << "Could not create file mapping object " << name
                   << " (Error # " << errno << " - " << strerror(errno) << ")";
        return false;
    }
    else {
        int r = ftruncate(map, size);
        if (r != 0) {
            LOG(Error) << "Could not truncate file mapping object " << name
                       << " (Error # " << errno << " - " << strerror(errno)
                       << ")";
            return false;
        }
        return true;
        // https://stackoverflow.com/questions/25502229/ftruncate-not-working-on-posix-shared-memory-in-mac-os-x
    }
}

bool SharedMemory::open_only(MapHandle& map, const std::string& name) {
    map = shm_open(name.c_str(), O_RDWR, 0666);
    if (map != -1) {
        LOG(Verbose) << "Opened existing file mapping object " << name;
        return true;
    }
    LOG(Error) << "Failed to open file mapping object " << name;
    return false;
}

void SharedMemory::close(const std::string& name, MapHandle map) {
    ::close(map);
    ::shm_unlink(name.c_str());  // this should probably be called by the LAST process
}

void* SharedMemory::map_buffer(MapHandle map, std::size_t size) {
    void* buffer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, map, 0);
    if (buffer == MAP_FAILED) {
        LOG(Error) << "Could not map view of file"
                   << " (Error # " << errno << " - " << strerror(errno) << ")";
        return 0;
    }
    return buffer;
}

void SharedMemory::unmap_buffer(void* buffer, std::size_t size) {
    if (::munmap(buffer, size) != 0) {
        LOG(Error) << "Could not unmap view of file"
                   << " (Error #" << errno << " - " << strerror(errno) << ")";
    }
}

#endif

}  // namespace mel
