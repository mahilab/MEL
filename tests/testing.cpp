#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Logging/Writers/ConsoleWriter.hpp>

using namespace mel;

enum
{
    Console = 1
};

//int main() {
//    std::vector<double> x{1.0, 2.0, 3.0};
//    std::tuple<std::string, double, int> y("evan", 4.6, 2);
//    init_logger(DEBUG, "Test.txt");
//    LOG(INFO) << "This is a test" << " of two things";
//    LOG(INFO) << x;
//    return 0;
//}

int main()
{
    // Initialize the logger that will be measured.
    init_logger(DEBUG, "Performance.txt");

    // Initialize the logger for printing info messages.
    static ConsoleWriter<TxtFormatter> consoleAppender;
    init_logger<Console>(DEBUG, &consoleAppender);

    LOG_(Console, INFO) << "Test started";

    const int kCount = 50000;

    // Performance measure loop.
    Clock clock;
    for (int i = 0; i < kCount; ++i)
    {
        LOG(DEBUG) << "Hello log!";
    }

    LOG_(Console, INFO) << "Test finished: " <<  (double)clock.get_elapsed_time().as_microseconds() / (double)kCount << " microsec per call";

    return 0;
}

//#include <MEL/Logging/Writers/ColorConsoleAppender.hpp>
//#include <MEL/Logging/Writers/RollingFileAppender.hpp>
//
//int main()
//{
//    static ColorConsoleAppender<TxtFormatter> consoleAppender;
//    init(VERBOSE, "log.csv").add_writer(&consoleAppender);
//
//     Log severity levels are printed in different colors.
//    LOG_VERBOSE << "This is a VERBOSE message";
//    LOG_DEBUG << "This is a DEBUG message";
//    LOG_INFO << "This is an INFO message";
//    LOG_WARNING << "This is a WARNING message";
//    LOG_ERROR << "This is an ERROR message";
//    LOG_FATAL << "This is a FATAL message";
//
//    return 0;
//}