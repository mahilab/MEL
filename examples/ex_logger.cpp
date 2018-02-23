#include <MEL/Logging/Log.hpp>

using namespace mel;

// custom loggers must start at 1 (the default logger is 0)
enum { MyLogger = 1 };

// custom formatters must define two public static functions:
// static std::string header() & static std::string format(const Record& record)
class MyFormatter {
public:
    static std::string header() { return "This is my header."; }

    static std::string format(const Record& record) {
        std::string formatted_message = "";
        formatted_message += "Message: ";
        formatted_message += record.get_message();
        formatted_message += " (";
        formatted_message += record.get_timestamp().yyyy_mm_dd_hh_mm_ss();
        formatted_message += ")\n";
        return formatted_message;
    }
};

// custom writers must implement the Writer class and its single function write
template <class Formatter>
class MyWriter : public Writer {
public:
    virtual void write(const Record& record) override {
        std::string str = Formatter::format(record);
        if (record.get_severity() == Fatal)
            set_text_color(Color::White, Color::Red);
        else if (record.get_severity() == Error)
            set_text_color(Color::Red, Color::DarkRed);
        else if (record.get_severity() == Warning)
            set_text_color(Color::Red, Color::Yellow);
        else if (record.get_severity() == Info)
            set_text_color(Color::Green, Color::DarkGreen);
        else
            set_text_color(Color::Cyan, Color::Blue);
        print_string(str);
        reset_text_color();
    }
};

int main(int argc, char const* argv[]) {
    // This intializes the default MEL Logger. Severity levels Verbose and above
    // will be written to a rolling log file MEL.log, and severity levels
    // Info and above will be written to the console with color formatting.
    init_logger();

    LOG(Debug) << "This is a Debug log";      // goes nowhere by default
    LOG(Verbose) << "This is a Verbose log";  // goes to MEL.log only
    LOG(Info) << "This is an Info log";       // goes to MEL.log and console
    LOG(Warning) << "This is a Warning log";  // goes to MEL.log and console
    LOG(Error) << "This is an Error log";     // goes to MEL.log and console
    LOG(Fatal) << "This is a Fatal log";      // goes to MEL.log and console

    // You can create your own loggers too. First you need to choose a
    // Formatter. A Formatter is responsible for taking a log record and turning
    // it into a string. There are several built-in Formatters, and you can even
    // create your own. Here, we will use the built-in CsvFormatter. Once you've
    // chosen a Formatter, you create a Writer. A Writer is responsible for
    // taking a formatted string from a Formatter and writing it in some
    // prescribed manner, either to the console, a file, memory, etc. Here we
    // will choose the built int RollingFileWriter.
    RollingFileWriter<CsvFormatter> file_writer("my_log.csv");
    // Now, we create the actual Logger. Loggers are paramertized with an
    // integer which can be an enum for convienence. The default logger is 0, so
    // custom loggers should start at 1 or some other number.
    init_logger<MyLogger>(Verbose, &file_writer);
    /// Now we can do some logging. Note the slightly differnt macro name LOG_
    LOG_(MyLogger, Info) << "This is an Info log to MyLogger";
    LOG_(MyLogger, Error) << "This is an Error log to MyLogger";

    /// It's possible to add multiple writers to a single logger. Here, we will
    /// add our own custom Writer and Formatter to our existing logger.
    MyWriter<MyFormatter> my_writer;
    get_logger<MyLogger>()->add_writer(&my_writer);
    // note we could have done this at the beginning like so:
    // init_logger<MyLogger>(Verbose, &file_writer).>add_writer(&my_writer);

    LOG_(MyLogger, Debug) << "This is a Debug log";
    LOG_(MyLogger, Verbose) << "This is a Verbose log";
    LOG_(MyLogger, Info) << "This is an Info log";
    LOG_(MyLogger, Warning) << "This is a Warning log";
    LOG_(MyLogger, Error) << "This is an Error log";
    LOG_(MyLogger, Fatal) << "This is a Fatal log";

    return 0;
}
