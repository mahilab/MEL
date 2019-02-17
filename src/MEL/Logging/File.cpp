#include <fcntl.h>
#include <MEL/Logging/File.hpp>
#include <sys/stat.h>
#include <MEL/Utility/System.hpp>

#ifdef _WIN32
#include <io.h>
#include <share.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

namespace mel {

File::File() : m_file(-1) {}

File::File(const std::string& filepath) : m_file(-1) {
    open(filepath);
}

File::~File() {
    close();
}

off_t File::open(const std::string& filepath) {
    auto tidy = tidy_path(filepath, true);
#if defined(_WIN32) && (defined(__BORLANDC__) || defined(__MINGW32__))
    m_file = ::_sopen(tidy.c_str(), _O_CREAT | _O_WRONLY | _O_BINARY, SH_DENYWR, _S_IREAD | _S_IWRITE);
#elif defined(_WIN32)
    ::_sopen_s(&m_file, tidy.c_str(), _O_CREAT | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IREAD | _S_IWRITE);
#else
    m_file = ::open(tidy.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
    return seek_end(0);
}

int File::write(const void* data, std::size_t size) {
#ifdef _WIN32
    return m_file != -1 ? ::_write(m_file, data, static_cast<unsigned int>(size)) : -1;
#else
    return m_file != -1 ? static_cast<int>(::write(m_file, data, size)) : -1;
#endif
}

off_t File::seek(off_t offset, int whence) {
#ifdef _WIN32
    return m_file != -1 ? ::_lseek(m_file, offset, whence) : -1;
#else
    return m_file != -1 ? ::lseek(m_file, offset, whence) : -1;
#endif
}

off_t File::seek_set(off_t offset) {
    return seek(offset, SEEK_SET);
}

off_t File::seek_cur(off_t offset) {
    return seek(offset, SEEK_CUR);
}

off_t File::seek_end(off_t offset) {
    return seek(offset, SEEK_END);
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

int File::unlink(const std::string& filepath) {
#ifdef _WIN32
    return ::_unlink(filepath.c_str());
#else
    return ::unlink(filepath.c_str());
#endif
}

int File::rename(const std::string& old_filename, const std::string& new_filename) {
#ifdef _WIN32
    return MoveFileA(old_filename.c_str(), new_filename.c_str());
#else
    return ::rename(old_filename.c_str(), new_filename.c_str());
#endif
}

}  // namespace mel
