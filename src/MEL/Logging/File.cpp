#include <fcntl.h>
#include <MEL/Logging/File.hpp>
#include <sys/stat.h>

#ifdef __linux__

#elif _WIN32
#include <io.h>
#include <share.h>
#include <windows.h>
#endif

namespace mel {

File::File() : m_file(-1) {}

File::File(const char* fileName) : m_file(-1) {
    open(fileName);
}

File::~File() {
    close();
}

off_t File::open(const char* fileName) {
#if defined(_WIN32) && (defined(__BORLANDC__) || defined(__MINGW32__))
    m_file = ::_sopen(fileName, _O_CREAT | _O_WRONLY | _O_BINARY, SH_DENYWR,
                      _S_IREAD | _S_IWRITE);
#elif defined(_WIN32)
    ::_sopen_s(&m_file, fileName, _O_CREAT | _O_WRONLY | _O_BINARY, _SH_DENYWR,
               _S_IREAD | _S_IWRITE);
#else
    m_file = ::open(fileName, O_CREAT | O_WRONLY,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
    return seek(0, SEEK_END);
}

int File::write(const void* buf, size_t count) {
#ifdef _WIN32
    return m_file != -1
               ? ::_write(m_file, buf, static_cast<unsigned int>(count))
               : -1;
#else
    return m_file != -1 ? static_cast<int>(::write(m_file, buf, count)) : -1;
#endif
}

off_t File::seek(off_t offset, int whence) {
#ifdef _WIN32
    return m_file != -1 ? ::_lseek(m_file, offset, whence) : -1;
#else
    return m_file != -1 ? ::lseek(m_file, offset, whence) : -1;
#endif
}

void File::close() {
    if (m_file != -1) {
#ifdef _WIN32
        ::_close(m_file);
#else
        ::close(m_file);
#endif
        m_file = -1;
    }
}

int File::unlink(const char* fileName) {
#ifdef _WIN32
    return ::_unlink(fileName);
#else
    return ::unlink(fileName);
#endif
}

int File::rename(const char* oldFilename, const char* newFilename) {
#ifdef _WIN32
    return MoveFileA(oldFilename, newFilename);
#else
    return ::rename(oldFilename, newFilename);
#endif
}

}  // namespace mel
