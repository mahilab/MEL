#include <MEL/Utility/DataLog.hpp>

using namespace mel;

int main() {

    DataLog log;

    log.add_cols({"Col_A", "Col_B", "Col_C"});

    log.add_row({1, 2, 3});
    log.add_row({4, 5, 6});
    log.add_row({7, 8, 9});

    log.save_and_clear_data("my_log0", "logs");
    // logs/my_log0_2018-01-14-21.34.16.csv
    //-------------------------------------
    // Col_A,Col_B,Col_C,
    // 1,2,3,
    // 4,5,6,
    // 7,8,9,

    log.add_col("Col_D");

    log.add_row({10, 11, 12, 13});
    log.add_row({14, 15, 16, 17});

    log.save_data("my_log1", "logs");
    // logs/my_log1_2018-01-14-21.34.16.csv
    //-------------------------------------
    // Col_A,Col_B,Col_C,Col_D,
    // 10,11,12,13,
    // 14,15,16,17,

    log.add_row({18, 19, 20, 21});
    // data_log_backups/log_2018-01-14-21.34.16.csv
    //-------------------------------------
    // Col_A,Col_B,Col_C,Col_D,
    // 10,11,12,13,
    // 14,15,16,17,
    // 18,19,20,21,

    return 0;
}
