#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <iosfwd>
#include <type_traits>

namespace mel
{
enum class LogLevel : uint8_t {
    INFO,
    WARN,
    CRIT
};

class LogLine {

public:

    LogLine(LogLevel level, char const * file, char const * function, uint32_t line);
    ~LogLine();

    LogLine(LogLine &&) = default;
    LogLine& operator=(LogLine &&) = default;

    void stringify(std::ostream & os);

    LogLine& operator<<(char arg);
    LogLine& operator<<(int32_t arg);
    LogLine& operator<<(uint32_t arg);
    LogLine& operator<<(int64_t arg);
    LogLine& operator<<(uint64_t arg);
    LogLine& operator<<(double arg);
    LogLine& operator<<(std::string const & arg);

    template < size_t N >
    LogLine& operator<<(const char (&arg)[N])
    {
        encode(string_literal_t(arg));
        return *this;
    }

    template < typename Arg >
    typename std::enable_if < std::is_same < Arg, char const * >::value, LogLine& >::type
    operator<<(Arg const & arg)
    {
        encode(arg);
        return *this;
    }

    template < typename Arg >
    typename std::enable_if < std::is_same < Arg, char * >::value, LogLine& >::type
    operator<<(Arg const & arg)
    {
        encode(arg);
        return *this;
    }

    struct string_literal_t
    {
        explicit string_literal_t(char const * s) : m_s(s) {}
        char const * m_s;
    };

private:
    char * buffer();

    template < typename Arg >
    void encode(Arg arg);

    template < typename Arg >
    void encode(Arg arg, uint8_t type_id);

    void encode(char * arg);
    void encode(char const * arg);
    void encode(string_literal_t arg);
    void encode_c_string(char const * arg, size_t length);git
    void stringify(std::ostream & os, char * start, char const * const end);

private:
    size_t m_bytes_used;
    size_t m_buffer_size;
    std::unique_ptr < char [] > m_heap_buffer;
    char m_stack_buffer[256 - 2 * sizeof(size_t) - sizeof(decltype(m_heap_buffer)) - 8 /* Reserved */];
};

/// Non guaranteed logging. Uses a ring buffer to hold log lines.
/// When the ring gets full, the previous log line in the slot will be dropped.
/// Does not block producer even if the ring buffer is full.
/// ring_buffer_size_mb - LogLines are pushed into a mpsc ring buffer whose size
/// is determined by this parameter. Since each LogLine is 256 bytes,
/// ring_buffer_size = ring_buffer_size_mb * 1024 * 1024 / 256
struct NonGuaranteedLogger {
    NonGuaranteedLogger(uint32_t ring_buffer_size_mb_) : ring_buffer_size_mb(ring_buffer_size_mb_) {}
    uint32_t ring_buffer_size_mb;
};

/// Provides a guarantee log lines will not be dropped.
struct GuaranteedLogger {
};

struct Log {
    /// Logs a new LogLine
    bool operator==(LogLine &);
};

void set_log_level(LogLevel level);

bool is_logged(LogLevel level);

/// Ensure initialize_logging() is called prior to any log statements.
/// log_directory - where to create the logs. For example - "logs/"
/// log_file_name - root of the file name. For example - "my_log"
/// This will create log files of the form -
/// /tmp/my_log.1.txt
/// /tmp/my_log.2.txt
/// etc.
/// log_file_roll_size_mb - mega bytes after which we roll to next log file.
void initialize_logging(GuaranteedLogger gl, std::string const & log_directory, std::string const & log_file_name, uint32_t log_file_roll_size_mb);
void initialize_logging(NonGuaranteedLogger ngl, std::string const & log_directory, std::string const & log_file_name, uint32_t log_file_roll_size_mb);

} // namespace mel

// LOGGING MACROS

#define LOG(LEVEL) mel::Log() == mel::LogLine(LEVEL, __FILE__, __func__, __LINE__)
#define LOG_INFO mel::is_logged(mel::LogLevel::INFO) && LOG(mel::LogLevel::INFO)
#define LOG_WARN mel::is_logged(mel::LogLevel::WARN) && LOG(mel::LogLevel::WARN)
#define LOG_CRIT mel::is_logged(mel::LogLevel::CRIT) && LOG(mel::LogLevel::CRIT)
