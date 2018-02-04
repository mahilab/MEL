#include <MEL/Logging/Log.hpp>
#include <MEL/Logging/Writers/ConsoleWriter.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Timer.hpp>

using namespace mel;

enum { Console = 1 };

// int main() {
//     std::string str = "This is a message I'm using to test printing speed in
//     C++.\n"; Clock clock; for (std::size_t i = 0; i < 100; ++i) {
//         print(str);
//         //std::cout << str << std::flush;
//     }
//     std::cout << clock.get_elapsed_time();
// }

// int main() {
//    std::vector<double> x{1.0, 2.0, 3.0};
//    std::tuple<std::string, double, int> y("evan", 4.6, 2);
//    init_logger(DEBUG, "Test.txt");
//    LOG(INFO) << "This is a test" << " of two things";
//    LOG(INFO) << x;
//    return 0;
//}

// int main()
// {
//     // Initialize the logger that will be measured.
//     init_logger(DEBUG, "Performance.txt");

//     // Initialize the logger for printing info messages.
//     static ConsoleWriter<TxtFormatter> consoleAppender;
//     init_logger<Console>(DEBUG, &consoleAppender);

//     LOG_(Console, INFO) << "Test started";

//     const int kCount = 50000;

//     // Performance measure loop.
//     Clock clock;
//     for (int i = 0; i < kCount; ++i)
//     {
//         LOG(DEBUG) << "Hello log!";
//     }

//     LOG_(Console, INFO) << "Test finished: " <<
//     (double)clock.get_elapsed_time().as_microseconds() / (double)kCount << "
//     microsec per call";

//     return 0;
// }

#include <MEL/Logging/Writers/ColorConsoleWriter.hpp>
#include <MEL/Logging/Writers/RollingFileWriter.hpp>

int main() {
    static ColorConsoleWriter<TxtFormatter> consoleAppender;
    init_logger(Verbose, "log.csv").add_writer(&consoleAppender);

    // Log severity levels are printed in different colors.
    LOG(Verbose) << "This is a VERBOSE message";
    LOG(Debug) << "This is a DEBUG message";
    LOG(Info) << "This is an INFO message";
    LOG(Warning) << "This is a WARNING message";
    LOG(Error) << "This is an ERROR message";
    LOG(Fatal) << "This is a FATAL message";

    return 0;
}
