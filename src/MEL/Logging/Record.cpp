#include <MEL/Logging/Record.hpp>   

namespace mel {

    Record::Record(Severity severity,
        const char* func,
        size_t line,
        const char* file)
        : severity_(severity),
        tid_(mel::get_thread_id()),
        line_(line),
        func_(func),
        file_(file)
    {
    }

    Record::~Record() {

    }


    Record& Record::operator <<(char data) {
        char str[] = { data, 0 };
        return *this << str;
    }

    Record& Record::operator <<(std::ostream& (*data)(std::ostream&))
    {
        message_ << data;
        return *this;
    }

    const mel::Timestamp& Record::get_timestamp() const { return timestamp_; };

    Severity Record::get_severity() const { return severity_; }

    unsigned int Record::get_tid_() const { return tid_; }

    size_t Record::get_line() const { return line_; }

    const char* Record::get_message() const {
        message_str_ = message_.str();
        return message_str_.c_str();
    }

    const char* Record::get_func() const {
        func_str_ = process_function_name(func_);
        return func_str_.c_str();
    }


    const char* Record::get_file() const { return file_; }

    inline std::string process_function_name(const char* func) {
#if (defined(_WIN32) && !defined(__MINGW32__)) || defined(__OBJC__)
        return std::string(func);
#else
        const char* funcBegin = func;
        const char* funcEnd = ::strchr(funcBegin, '(');

        if (!funcEnd) {
            return std::string(func);
        }
        for (const char* i = funcEnd - 1; i >= funcBegin;
            --i)  // search backwards for the first space char
        {
            if (*i == ' ') {
                funcBegin = i + 1;
                break;
            }
        }
        return std::string(funcBegin, funcEnd);
#endif
    }


} // namespace mel