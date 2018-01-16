#include <MEL/Utility/DataLog.hpp>

using namespace mel;

int main(int argc, char const *argv[])
{
    DataLog<double, double, std::string> my_log({"a", "b", "c"});
    my_log.add_row(2.0, 3.0, "my_string");
    my_log.add_row(5.12, 6.45, "evan");
    my_log.add_row({6, 7, "monkey"});
    print(my_log.get_row(2));
    my_log.save_data("my_log1", "logs");
    return 0;
}



