#include <MEL/Utility/DataLog.hpp>

using namespace mel;

int main(int argc, char const *argv[])
{
    DataLog<int, double> my_log({"my_int", "my_double"});
    for (int i = 0; i < 100; ++i)
    {
        my_log.add_row(1, 3.1459);
    }
    my_log.set_format(Format::Scientific);
    my_log.set_precision(10);
    my_log.save_data("log0", "logs");
    my_log.wait_for_save();
    return 0;
}



