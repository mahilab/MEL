#include <MEL/Utility/Log.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

int main(int argc, char const *argv[])
{
    create_directory("tmp");
    initialize_logging(GuaranteedLogger(), "tmp/", argv[1], 1);
    Clock clock;
    for (int i = 0; i < 10000; ++i)
    {
        LOG_INFO << "Information to be logged." << i;
    }
    print(clock.get_elapsed_time());
    return 0;
}
