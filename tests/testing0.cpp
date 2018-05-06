#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;
int main() {

    DataLogger log;
    log.set_header({"A","B","C","D","E"});
    log.buffer({1,2,3,4,5});
    log.buffer({6,7,8,9,10});

    print(log.get_col_count());
    print(log.get_row_count());

    log.save_data("test1");

    log.buffer({1,2,3,4,5});
    log.buffer({6,7,8,9,10});

    log.save_data("test2");


    return 0;
}

