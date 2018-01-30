#include <MEL/Utility/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/System.hpp>

using namespace mel;

int main(int argc, char const *argv[]) {

    LOG(INFO) << "This is an INFO log.";
    LOG(WARNING) << "This is a WARNING log.";
    LOG(ERROR) << "This is an ERROR log.";
    LOG(DEBUG) << "This is a DEBUG log.";
    LOG(FATAL) << "This is a FATAL log."; // kills app!

    return 0;
}
