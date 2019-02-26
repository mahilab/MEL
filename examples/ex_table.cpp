// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#include <MEL/Core/Timer.hpp>
#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Utility/System.hpp>

using namespace mel;

int main() {

    std::string filename = "table_data.tbl";
    Table tab("my_table", { "col 0", "col 1", "col 2" });
    tab.push_back_row({ 1.01, 2.02, 3.03 });
    tab.push_back_row({ 4.04, 5.05, 6.06 });

    std::vector<Table> tabs(3);
    tabs[0] = tab;
    tabs[1] = tab;
    tabs[2].rename("my_table2");
    tabs[2].set_col_names({ "COL A", "COL B" });
    tabs[2].set_values({ {0.03256, -0.23535}, {8, 9}, {-1000, -5000} });

    Table::write(filename, tabs);

    Table new_tab;
    std::vector<Table> new_tabs;
    if (Table::read(filename, new_tabs)) {
        //std::cout << new_tab << std::endl;
        for (std::size_t i = 0; i < new_tabs.size(); ++i) {
            std::cout << new_tabs[i];
        }
    }
}
