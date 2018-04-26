#include <MEL/Logging/Table.hpp>
#include <MEL/Logging/Log.hpp>
#include <iostream>

using namespace mel;
int main() {
	
	init_logger();

	Table tab("my_table");

	tab.set_col_names({ "col_0","col_1","col_2" });
	std::cout << tab;

	tab.insert_rows({ { 0, 1, 2 }, {0.5, 1.5, 2.5} }, 0);
	std::cout << tab;

	tab.insert_rows({ {0.1, 1.1, 2.1}, {0.2, 1.2, 2.2} }, 1);
	std::cout << tab;

	tab.insert_col("col_6", { 6, 6.1, 6.2, 6.5 }, tab.col_count());
	std::cout << tab;

	tab.insert_col("col_5", { 5, 5.1, 5.2, 5.5 }, 3);
	std::cout << tab;

	tab.insert_cols({ "col_3", "col_4" }, { {3, 4}, {3.1, 4.1}, {3.2, 4.2}, {3.5, 4.5} }, 3);
	std::cout << tab;


    return 0;
}

