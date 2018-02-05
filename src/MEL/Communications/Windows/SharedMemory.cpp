#include <MEL/Communications/Windows/SharedMemory.hpp>
#include <MEL/Logging/Log.hpp>
#include <iostream>
#include <cstring>

#ifdef __linux__

#elif _WIN32
    #include <windows.h>
    #include <stdio.h>
    #include <conio.h>
    #include <tchar.h>
#endif

namespace mel {

//==============================================================================
// COMMON IMPLEMENTATION
//==============================================================================

SharedMemory::SharedMemory(const std::string& name, std::size_t max_size) :
    name_(name),
    max_size_(max_size),
    map_(create_or_open(name_, max_size_)),
    buffer_(map_buffer(map_, max_size_))
{
}

SharedMemory::~SharedMemory() {
    unmap_buffer(buffer_);
    close(map_);
}

bool SharedMemory::write(const void* data, std::size_t size, std::size_t offset) {
    if (size + offset > max_size_) {
        return false;
    }
    std::memcpy(((char*)buffer_) + offset, data, size);
    return true;
}

bool SharedMemory::read(void* data, std::size_t size, std::size_t offset) {
    if (size + offset > max_size_) {
        return false;
    }
    std::memcpy(data, ((char*)buffer_) + offset, size);
    return true;
}

/// Returns a pointer to the underlying memory block
void* SharedMemory::get_address() const {
    return buffer_;
}

/// Returns the string name of the named memory map
std::string SharedMemory::get_name() const {
    return name_;
}

#ifdef __linux__

//==============================================================================
// LINUX IMPLEMENTATION
//==============================================================================

#elif _WIN32

//==============================================================================
// WINDOWS IMPLEMENTATION
//==============================================================================

MapHandle SharedMemory::create_or_open(std::string name, std::size_t size) {
    HANDLE hMapFile;
    hMapFile = CreateFileMappingA(
                  INVALID_HANDLE_VALUE,     // use paging file
                  NULL,                     // default security
                  PAGE_READWRITE,           // read/write access
                  0,                        // maximum object size (high-order DWORD)
                  static_cast<DWORD>(size), // maximum object size (low-order DWORD)
                  name.c_str());            // name of mapping object

    if (hMapFile == NULL) {
        LOG(Error) << "Could not create file mapping object " << name << " (Windows Error #" << (int)GetLastError() << ")";
        return NULL;
    }
    return hMapFile;
}

void SharedMemory::close(MapHandle map) {
    if (CloseHandle(map) == 0)
        LOG(Error) << "Failed to close handle (Windows Error #" << (int)GetLastError() << ")";
}

void* SharedMemory::map_buffer(MapHandle map, std::size_t size) {
    void* pBuf;
    pBuf = MapViewOfFile(map,          // handle to map object
                FILE_MAP_ALL_ACCESS,   // read/write permission
                0,
                0,
                size);
    if (pBuf == NULL) {
       LOG(Error) << "Could not map view of file (Windows Error #" << (int)GetLastError() << ")";
       return NULL;
    }
    return pBuf;
}

void SharedMemory::unmap_buffer(void* buffer) {
    UnmapViewOfFile(buffer);
}

#endif

} // namespace mel
