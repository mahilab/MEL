#include <MEL/Core/Timer.hpp>
#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>

using namespace mel;

int main() {

    // Basic data logging with DataLogger class

    // instantiate logger
    DataLogger logger;
    // set headers
    logger.set_header({"ColumnA", "ColumnB", "ColumnC", "ColumnD"});
    // generate data
    for (std::size_t i = 0; i < 100; ++i) {
       std::vector<double> row{1.0*i, 2.0*i, 3.0*i, 4.0*i};
       logger.buffer(row);
    }
    // save data
    logger.save_data("datalogger_data.csv");


    // Advanced data logging with Tables

    std::string filename = "table_data.csv";
    Table tab("my_table", { "col 0", "col 1", "col 2" });
    tab.push_back_row({ 1.01, 2.02, 3.03 });
    tab.push_back_row({ 4.04, 5.05, 6.06 });

    std::vector<Table> tabs(3);
    tabs[0] = tab;
    tabs[1] = tab;
    tabs[2].set_col_names({ "col A", "col B" });
    tabs[2].set_values({ {0.03256, -0.23535}, {8, 9}, {-1000, -5000} });

    DataLogger::write_to_csv(tabs, filename, ".", false);

    Table new_tab;
    std::vector<Table> new_tabs;
    if (DataLogger::read_from_csv(new_tabs, filename, ".")) {
        //std::cout << new_tab << std::endl;
        for (std::size_t i = 0; i < new_tabs.size(); ++i) {
            std::cout << new_tabs[i];
        }
    }
}
