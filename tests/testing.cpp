#include <MEL/Logging/Log.hpp>
#include <MEL/Logging/Writers/ConsoleWriter.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Logging/Writers/ColorConsoleWriter.hpp>
#include <MEL/Logging/Writers/RollingFileWriter.hpp>

using namespace mel;

enum { Console = 1 };

 void performance() {
     // Initialize the logger that will be measured.
     //init_logger(Debug, "Performance.txt");
     // Initialize the logger for printing info messages.
     static ConsoleWriter<TxtFormatter> consoleAppender;
    // init_logger<Console>(Debug, &consoleAppender);
     LOG_(Console, Info) << "Test started";
     const int kCount = 50000;
     // Performance measure loop.
     Clock clock;
     for (int i = 0; i < kCount; ++i) {
         LOG(Debug) << "Hello log!";
     }
     LOG_(Console, Info) << "Test finished: " << (double)clock.get_elapsed_time().as_microseconds() / (double)kCount << " microsec per call";
 }

void color_console() {
    static ColorConsoleWriter<TxtFormatter> consoleAppender;
    init_logger(Verbose, "log.csv").add_writer(&consoleAppender);
    // Log severity levels are printed in different colors.
    LOG(Verbose) << "This is a VERBOSE message";
    LOG(Debug) << "This is a DEBUG message";
    LOG(Info) << "This is an INFO message";
    LOG(Warning) << "This is a WARNING message";
    LOG(Error) << "This is an ERROR message";
    LOG(Fatal) << "This is a FATAL message";
}

int main() {
    color_console();
    return 0;
}
