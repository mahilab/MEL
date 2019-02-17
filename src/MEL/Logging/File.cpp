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

File::File() : file_handle_(-1) {}

File::File(const std::string& filepath, WriteMode mode) : 
    file_handle_(-1)
{
    open(filepath, mode);
}

File::~File() {
    close();
}

off_t File::open(const std::string& filepath, WriteMode mode) {

    auto tidy = tidy_path(filepath, true);
    if (mode == WriteMode::Truncate)
        unlink(filepath);

#if defined(_WIN32) && (defined(__BORLANDC__) || defined(__MINGW32__))
    file_handle_ = ::_sopen(tidy.c_str(), _O_CREAT | _O_WRONLY | _O_BINARY, SH_DENYWR, _S_IREAD | _S_IWRITE);
#elif defined(_WIN32)
    ::_sopen_s(&file_handle_, tidy.c_str(), _O_CREAT | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IREAD | _S_IWRITE);
#else
    file_handle_ = ::open(tidy.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
    return seek_end(0);
    
}

int File::write(const void* data, std::size_t size) {
#ifdef _WIN32
    return file_handle_ != -1 ? ::_write(file_handle_, data, static_cast<unsigned int>(size)) : -1;
#else
    return file_handle_ != -1 ? static_cast<int>(::write(file_handle_, data, size)) : -1;
#endif
}

off_t File::seek(off_t offset, int whence) {
#ifdef _WIN32
    return file_handle_ != -1 ? ::_lseek(file_handle_, offset, whence) : -1;
#else
    return file_handle_ != -1 ? ::lseek(file_handle_, offset, whence) : -1;
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
    if (file_handle_ != -1) {
#ifdef _WIN32
        ::_close(file_handle_);
#else
        ::close(file_handle_);
#endif
        file_handle_ = -1;
    }
}

int File::unlink(const std::string& filepath) {
    auto tidy = tidy_path(filepath, true);
#ifdef _WIN32
    return ::_unlink(filepath.c_str());
#else
    return ::unlink(filepath.c_str());
#endif
}

int File::rename(const std::string& old_filepath, const std::string& new_filepath) {
    auto old_tidy = tidy_path(old_filepath, true);
    auto new_tidy = tidy_path(new_filepath, true);
#ifdef _WIN32
    return MoveFileA(old_tidy.c_str(), new_tidy.c_str());
#else
    return ::rename(old_tidy.c_str(), new_tidy.c_str());
#endif
}

bool File::parse(const std::string &in, std::string &directory, std::string &filename, std::string &ext, std::string &full)
{
    // can't do anythign with empty string
    if (in == "" || in.empty())
        return false;
    // split path
    auto splits = split_path(in);
    // split filename
    auto filename_ext = splits.back();
    if (filename_ext == "" || filename_ext.empty())
        return false;
    split_filename(filename_ext, filename, ext);
    // make directory string
    directory.clear();
    for (std::size_t i = 0; i < splits.size() - 1; ++i)
        directory += splits[i] + get_separator();
    directory = tidy_path(directory, false);
    full = directory + filename;
    if (ext != "" || !ext.empty())
        full += "." + ext;
    return true;
}



}  // namespace mel
